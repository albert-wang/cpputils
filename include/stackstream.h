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
		public:
			MemorySink(Memory::StackScope& scope)
				:scope(&scope)
				,base(NULL)
				,length(0)
			{}

			std::streamsize write(const Ch * s, std::streamsize n)
			{
				if (!scope) 
				{
					return 0;
				}

				Ch * target = scope->createArray<Ch>(n);

				memcpy(target, s, sizeof(Ch) * n);
				length += n;

				if (!base)
				{
					base = target;
				}

				return n;
			}

			std::streamsize optimal_buffer_size() const
			{
				return 32;
			}

			const Ch * data()
			{
				return base;
			}

			size_t size() const
			{
				return length;
			}

			void zeroTerminate() 
			{
				write("\0", 1);
				scope = NULL;
			}
		private:
			Memory::StackScope * scope;
			Ch * base;
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
				(*self)->zeroTerminate();
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
