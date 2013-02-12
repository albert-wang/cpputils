#include "stackfwd.h"
#include "stackstream.h"
#include <locale>

#define MAXIMUM_FORMAT_ARGUMENTS 16
namespace Engine
{
	namespace Detail
	{
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

		template<typename C>
		size_t value(C c)
		{
			if (c >= static_cast<C>('0') && c <= static_cast<C>('9'))
			{
				return c - static_cast<C>('0');
			}

			return 0;
		}

		template<typename C>
		bool isDigit(C c)
		{
			return c >= static_cast<C>('0') && c <= static_cast<C>('9');
		}

		template<typename C>
		size_t limitedAtoi(const C * buffer)
		{
			return value(buffer[1]) * 10 + value(buffer[0]);
		}

		size_t countSeenCommands(const bool * buffer)
		{
			size_t count = 0;
			for (size_t i = 0; i < MAXIMUM_FORMAT_ARGUMENTS; ++i)
			{
				if (buffer[i])
				{
					count++;
				}
			}
			return count;
		}

#define GENERATE_TYPENAME(z, n, d) \
		typename BOOST_PP_CAT(T, n)

#define GENERATE_ARGS(z, n, d) \
		BOOST_PP_CAT(T, n)&& BOOST_PP_CAT(t, n)

#define GENERATE_SWITCH(z, n, d) \
		case n : { out << BOOST_PP_CAT(t, n); break; }

#define CREATE_SELECT(N) \
		template<typename Stream BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM(N, GENERATE_TYPENAME, ~)> \
		void select(Stream& out, size_t index BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM(N, GENERATE_ARGS, ~)) { \
			switch(index) { BOOST_PP_REPEAT(N, GENERATE_SWITCH, ~) }}

#define BOOST_PP_LOCAL_LIMITS (0, 16)
#define BOOST_PP_LOCAL_MACRO CREATE_SELECT
#include BOOST_PP_LOCAL_ITERATE() 

#undef GENERATE_SELECT
#undef GENERATE_SWITCH
#undef GENERATE_ARGS
#undef GENERATE_TYPENAME
	}

	/*
		Takes 
			Words {0 | flags}, escape is {{ and }}. Note that {{{1}}} is output as {<contents-of-1>}

			An empty set of brackets implies {<number-of-unique-commands-thus-far>}, that is, 
				"{} {} {}" is equivilent to "{0} {1} {2}". 

			The maximum number of values is 15.
	*/

	//These are string formatting methods. Up to 16 arguments
#include "formatinl.h"
}
