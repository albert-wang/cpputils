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
				base = NULL;
				last = NULL;
				compiled = NULL;
				length = 0;
			}
		private:
			void finish()
			{
				if (base == NULL)
				{
					write("", 1);
				}

				if (base == last)
				{
					base->buffer[base->length] = '\0';
					compiled = base->buffer;
					return;
				}

				std::cout << "Create\n";
				
				compiled = scope->createArray<Ch>(length + 1);

				Node * current = base;
				size_t written = 0;

				while(current)
				{
					memcpy(compiled + written, current->buffer, sizeof(Ch) * current->length);
					written += current->length;
					current = current->next;

					std::cout << "Written: " << written << "\n";
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
				return self->getSink()->data();
			}

			size_t size() 
			{
				Stream * self = static_cast<Stream *>(this);
				self->flush();
				return self->getSink()->size();
			}

			const Ch * c_str() 
			{
				Stream * self = static_cast<Stream *>(this);
				self->flush();
				return self->getSink()->data();
			}

			void reset()
			{
				Stream * self = static_cast<Stream *>(this);
				self->getSink()->reset();
			}
		};
	};

	namespace Memory
	{
		template<typename C>
		class BasicStreamBuffer 
			: public std::basic_streambuf<C>
		{
			typedef typename std::basic_streambuf<C>::int_type int_type;
			typedef typename std::char_traits<C> traits;
		public:
			BasicStreamBuffer(Memory::StackScope& scope)
				:sink(scope)
			{
				buffer = scope.createArrayPOD<C>(64);
				this->setp(buffer, buffer + 64);
			}

			int_type overflow(int_type ch)
			{
				if (ch != traits::eof())
				{
					sync();
					*this->pptr() = ch;
					this->pbump(1);
				}

				return ch;
			}

			int sync() 
			{
				int count = this->pptr() - this->pbase();
				this->pbump(-count);
				sink.write(this->pbase(), count);

				return 0;
			}

			Detail::MemorySink<C> * getSink()
			{
				return &sink;
			}
		private:
			Detail::MemorySink<C> sink;
			C * buffer; 
		};

		template<typename C>
		class BasicStream 
			: public std::basic_ostream<C>
			, public Detail::StreamOperations<C, BasicStream<C> >
		{
		public:
			explicit BasicStream(Memory::StackScope& scope)
				:std::basic_ostream<C>(&buffer)
				,buffer(scope)
			{}

			Detail::MemorySink<C> * getSink()
			{
				return buffer.getSink();
			}
		private:
			BasicStreamBuffer<C> buffer; 
		};

		typedef BasicStream<char> Stream;
		typedef BasicStream<wchar_t> WideStream;
	}
}
