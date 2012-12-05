#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility.hpp>
#include <boost/type_traits.hpp>

#include <cassert>
#include <utility>

namespace Engine
{
	namespace Memory
	{
		//This creates 16-byte aligned allocations.
		class StackAllocator : boost::noncopyable
		{
		public:
			struct Incomplete;
			typedef Incomplete * Mark;

			//Will always allocate a multiple of 16 bytes.
			explicit StackAllocator(size_t memoryBytes);
			~StackAllocator();

			template<typename T>
			T * allocate(size_t count)
			{
				//Make sure we have enough space to allocate.
				assert(sizeof(T) * count + (current - base) < avaliableSpace);
				assert(count > 0);

				T * result = reinterpret_cast<T *>(current);
				
				//The current pointer is always resting at a multiple of 16 bytes.
				
				if ((sizeof(T) * count) % 16 != 0)
				{	
					current += (((sizeof(T) * count) / 16) + 1) * 16;
				}
				else
				{
					current += sizeof(T) * count;
				}

				return result;
			}

			Mark mark();
			void release(Mark b);
		private:
			boost::uint8_t * base;
			boost::uint8_t * current; 

			size_t avaliableSpace;
		};

		//This is an overlay on the stack allocator, which provides things like destructors.
		class StackScope : boost::noncopyable
		{
			//16 bytes for both x86 and x64.
			typedef void (*FinalizerMethod)(boost::uint8_t *);

			struct FinalizerEntry
			{
				FinalizerMethod finalizer;
				FinalizerEntry * next;

#if !defined(__LP64__) && !defined(_DEBUG)
				boost::uint8_t padding[8];
#endif
			};
			BOOST_STATIC_ASSERT(sizeof(FinalizerEntry) % 16 == 0);

			struct ArrayEntry
			{
				boost::uint32_t size;
				boost::uint8_t padding[12];
			};
			BOOST_STATIC_ASSERT(sizeof(ArrayEntry) % 16 == 0);

			template<typename T>
			static void destructor(boost::uint8_t * mem)
			{
				T * result = reinterpret_cast<T *>(mem + sizeof(FinalizerEntry));
				result->~T();
			}

			template<typename T>
			static void destructArray(boost::uint8_t * mem)
			{
				ArrayEntry * meta = reinterpret_cast<ArrayEntry *>(mem + sizeof(FinalizerEntry));
				T * base = reinterpret_cast<T *>(meta + 1);

				for (size_t i = 0; i < meta->size; ++i)
				{
					base[meta->size - 1 - i].~T();
				}
			}
		public:
			explicit StackScope(StackAllocator * base);
			~StackScope();

			//Basic creates
			template<typename T, typename U>
			typename boost::enable_if< 
				boost::is_pod<T>, 
				T *
			>::type create(U&& arg)
			{
				T * result = base->allocate<T>(1);
				new (result) T(arg);
				return result;
			}

			template<typename T>
			typename boost::enable_if< 
				boost::is_pod<T>, 
				T *
			>::type createArray(size_t size)
			{
				T * result = base->allocate<T>(size);
				return result;
			}

			template<typename T, typename U>
			typename boost::disable_if< 
				boost::is_pod<T>, 
				T *
			>::type create(U&& arg)
			{
				FinalizerEntry * entry = base->allocate<FinalizerEntry>(1);
				entry->finalizer = &destructor<T>;
				entry->next = finalizer;
				finalizer = entry;

				T * result = base->allocate<T>(1);

				//XXX: should use std::forward
				new (result) T(arg);
				return result;
			}

			template<typename T, typename U>
			typename boost::disable_if< 
				boost::is_pod<T>, 
				T *
			>::type createArray(size_t size, U&& proto)
			{
				FinalizerEntry * entry = base->allocate<FinalizerEntry>(1);
				entry->finalizer = &destructArray<T>;
				entry->next = finalizer;
				finalizer = entry;

				ArrayEntry * arr = base->allocate<ArrayEntry>(1);
				arr->size = size;

				T * result = base->allocate<T>(size);
				for (size_t i = 0; i < result; ++i)
				{
					new (result + i) T(proto);
				}

				return result;
			}

			template<typename T>
			typename boost::disable_if< 
				boost::is_pod<T>, 
				T *
			>::type createArray(size_t size)
			{
				FinalizerEntry * entry = base->allocate<FinalizerEntry>(1);
				entry->finalizer = &destructArray<T>;
				entry->next = finalizer;
				finalizer = entry;

				ArrayEntry * arr = base->allocate<ArrayEntry>(1);
				arr->size = size;

				T * result = base->allocate<T>(size);
				for (size_t i = 0; i < size; ++i)
				{
					new (result + i) T();
				}

				return result;
			}
		private:
			StackAllocator * base;
			StackAllocator::Mark mark;

			FinalizerEntry * finalizer;
		};
	}
}
