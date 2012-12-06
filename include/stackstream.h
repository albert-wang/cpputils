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
			static const size_t BUFFER_SIZE = 32;
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

				Node * node = scope->createPOD<Node>();
				node->buffer = scope->createArray<Ch>(n);
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
		private:
			void finish()
			{
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
		};
	};

	class MemoryStream 
		: public boost::iostreams::stream<Detail::MemorySink<char>>
		, public Detail::StreamOperations<char, MemoryStream>
	{
	public:
		explicit MemoryStream(Memory::StackScope& scope)
		{
			this->open(Detail::MemorySink<char>(scope));
		}
	};

	class WideMemoryStream 
		: public boost::iostreams::stream<Detail::MemorySink<wchar_t>>
		, public Detail::StreamOperations<wchar_t, WideMemoryStream>
	{
	public:
		explicit WideMemoryStream(Memory::StackScope& scope)
		{
			this->open(Detail::MemorySink<wchar_t>(scope));
		}
	};
}
