#include "stackfwd.h"
#include "stackstream.h"
#include <locale>

namespace Engine
{
	namespace Detail
	{
		template<typename Ch>
		struct Partial
		{
			const Ch * partial;
			size_t index;
			boost::uint8_t flags[4];
		};

		template<typename Ch>
		struct ParsedFormat
		{
			Partial<Ch> * partials;
			size_t length;
		};

		template<typename C>
		size_t commandCount(const C * begin, const C * end)
		{
			size_t length = end - begin;

			//Minimum command is {0}, so any string of length 2 or less must have zero commands.
			if (begin == end || length <= 2)
			{
				return 0;
			}

			


			while (begin != end - 1) 
		}

		template<typename C>
		size_t stringlength(const C * begin)
		{
			size_t result = 0;
			while (*begin)
			{
				result++;
				begin++;
			}
			return result;
		}
	}

	/*
		Takes 
			Words {0 | flags}, escape is {{ and }}. Note that {{{1}}} is output as {<contents-of-1>}
	*/

	//These are string formatting methods. Up to 15 arguments
	template<typename T0, template Ch>
	const Ch * format(Memory::StackScope& scope, const Ch * format, T0&& t0)
	{
		using namespace Detail;

		size_t length = stringlength(format);

		ParsedFormat<Ch> parsed;

		size_t partials = commandCount(format, format + length);
		parsed.partials = scope.createArrayPOD(partials);
		parsed.length = partials;



		Engine::Memory::Stream stream(scope);

		Partial<Ch> * start = parsed.partials;
		for (size_t i = 0; i < parsed.length; ++i)
		{
			Partial<Ch>& partial = parsed.partials[i];
			stream << partial.partial;

			switch (partial.index) 
			{



			}
		}

		return NULL;
	};

	template<typename T0>
	const wchar_t * format(Memory::StackScope& scope, const wchar_t * format, T0&& t0)
	{
		return NULL;
	};
}
