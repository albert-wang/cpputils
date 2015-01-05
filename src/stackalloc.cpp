#include "stackalloc.h"
#include <memory.h>
#include <iostream>

namespace Engine
{
	//Stack Allocator
	namespace Memory
	{
		StackAllocator::StackAllocator(size_t memoryBytes)
			:consumed(0)
			,avaliableSpace(((memoryBytes / 16) + 1) * 16)
			,mostRecentMark(NULL)
		{
			assert(memoryBytes > 0);
			base = static_cast<boost::uint8_t *>(malloc(avaliableSpace));
		}

		StackAllocator::~StackAllocator()
		{
			free(base);
		}

		boost::uint32_t location(boost::uint8_t * base, void * target)
		{
			return static_cast<boost::uint32_t>(reinterpret_cast<uint8_t *>(target) - base);
		}

		StackAllocator::Mark StackAllocator::mark()
		{
			boost::mutex::scoped_lock lock(mutex);

			MarkEntry * entry = allocate<MarkEntry>(1);
			entry->prev = 0;
			entry->next = 0;
			entry->release = location(base, entry);
			entry->flags = 0;

			if (mostRecentMark) 
			{
				entry->prev = location(base, mostRecentMark);
				mostRecentMark->next = location(base, entry);
			}

			std::cout << "Mark " << entry->release << "\n";

			mostRecentMark = entry;
			return reinterpret_cast<Incomplete *>(entry);
		}

		void StackAllocator::release(StackAllocator::Mark b)
		{
			MarkEntry * entry = reinterpret_cast<MarkEntry *>(b);

			if (entry->flags == 1)
			{
				return;
			}

			boost::mutex::scoped_lock lock(mutex);
			if (entry->next) 
			{
				MarkEntry * next = reinterpret_cast<MarkEntry *>(base + entry->next);
				MarkEntry * prev = reinterpret_cast<MarkEntry *>(base + entry->prev);

				prev->next = entry->next;
				next->prev = entry->prev;

				std::cout << "Forward " << entry->release << " -> " << next->release << "\n";

				next->release = entry->release;
				entry->flags = 1;

			} 
			else
			{
				MarkEntry * prev = reinterpret_cast<MarkEntry *>(base + entry->prev);
				mostRecentMark = prev;

				std::cout << "Release " << entry->release << "\n";
				consumed.exchange(entry->release);
			}
		}

		const boost::uint8_t * StackAllocator::currentAllocation() const
		{
			return base + consumed.load();
		}

		void StackAllocator::reset(boost::uint8_t * point)
		{
			size_t bytes = point - base;
			assert(bytes < avaliableSpace);

			consumed.exchange(bytes);
		}

		size_t StackAllocator::remaining() const
		{
			return avaliableSpace - consumed;
		}
	}

	//Stack Scope
	namespace Memory
	{
		StackScope::StackScope(StackAllocator * base)
			:base(base)
			,mark(NULL)
			,finalizer(NULL)
		{
			mark = base->mark();
		}

		StackScope::~StackScope()
		{
			close();
		}

		void StackScope::close()
		{
			FinalizerEntry * fin = finalizer;
			while (fin)
			{
				fin->finalizer(reinterpret_cast<boost::uint8_t *>(fin));
				fin = fin->next;
			}

			base->release(mark);
		}
	}
}
