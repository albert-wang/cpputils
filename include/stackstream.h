#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/cstdint.hpp>
#include <memory>

#include "stackalloc.h"
#pragma once
namespace Engine
{
	namespace Detail
	{
		struct MemoryCategory
			: boost::iostreams::output
			, boost::iostreams::optimally_buffered_tag
		{};

		template<typename Ch>
		class MemorySink : public boost::iostreams::device<Detail::MemoryCategory, Ch>
		{
			static const size_t BUFFER_SIZE = 64;
			struct Node
			{
				Ch * buffer;
				Node * next;
				boost::uint8_t length;
			};
		public:
			MemorySink(Memory::StackScope& scope)
				:scope(&scope)
				,base(NULL)
				,last(NULL)
				,compiled(NULL)
				,length(0)
			{}

			std::streamsize write(const Ch * s, std::streamsize n)
			{
				if (compiled)
				{
					return 0;
				}

				if (last && scope->unsafeResizeArray(last->buffer, last->length, (last->length + n + 1)))
				{
					memcpy(last->buffer + last->length, s, sizeof(Ch) * n); 
					last->length = last->length + n;
					length += n;
					return n;
				}

				Node * node = scope->createPOD<Node>();
				node->buffer = scope->createArray<Ch>(n + 1);
				node->length = n;
				node->next = NULL;

				if (last)
				{
					last->next = node;
					last = node;
				}
				else 
				{
					assert(!base);
					base = node;
					last = node;
				}

				memcpy(node->buffer, s, sizeof(Ch) * n);
				length += n;
				return n;
			}

			std::streamsize optimal_buffer_size() const
			{
				return BUFFER_SIZE;
			}

			const Ch * data()
			{
				if (!compiled)
				{
					finish();
				}

				return compiled;
			}

			size_t size() const
			{
				return length;
			}

			void reset() 
			{
				scope->close();
				base = nullptr;
				last = nullptr;
				compiled = nullptr;
				length = 0;
			}
		private:
			void finish()
			{
				if (base == last)
				{
					base->buffer[base->length] = '\0';
					compiled = base->buffer;
					return;
				}
				
				compiled = scope->createArray<Ch>(length + 1);

				Node * current = base;
				size_t written = 0;

				while(current)
				{
					memcpy(compiled + written, current->buffer, sizeof(Ch) * current->length);
					written += current->length;
					current = current->next;
				}

				compiled[written] = '\0';
			}

			Memory::StackScope * scope;
			Node * base;
			Node * last;
			Ch * compiled; 
			size_t length;
		};

		template<typename Ch, typename Stream>
		class StreamOperations
		{
		public:
			const Ch * data() 
			{
				Stream * self = static_cast<Stream *>(this);
				self->flush();
				return (*self)->data();
			}

			size_t size() 
			{
				Stream * self = static_cast<Stream *>(this);
				self->flush();
				return (*self)->size();
			}

			const Ch * c_str() 
			{
				Stream * self = static_cast<Stream *>(this);
				self->flush();
				return (*self)->data();
			}

			void reset()
			{
				Stream * self = static_cast<Stream *>(this);
				(*self)->reset();
			}
		};
	};

	namespace Memory
	{
		class Stream 
			: public boost::iostreams::stream<Detail::MemorySink<char>>
			, public Detail::StreamOperations<char, Stream>
		{
		public:
			explicit Stream(Memory::StackScope& scope)
			{
				this->open(Detail::MemorySink<char>(scope));
			}
		};

		class WideStream 
			: public boost::iostreams::stream<Detail::MemorySink<wchar_t>>
			, public Detail::StreamOperations<wchar_t, WideStream>
		{
		public:
			explicit WideStream(Memory::StackScope& scope)
			{
				this->open(Detail::MemorySink<wchar_t>(scope));
			}
		};
	}
}
