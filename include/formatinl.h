// sample.h
#if !BOOST_PP_IS_ITERATING
#ifndef FORMAT_INL_H
#define FORMAT_INL_H

#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_shifted_params.hpp>

#define BOOST_PP_ITERATION_LIMITS (0, 16) 
#define BOOST_PP_FILENAME_1 "formatinl.h"
#include BOOST_PP_ITERATE()

#endif
#else 


#define GENERATE_TYPENAME(z, n, d) \
      typename BOOST_PP_CAT(T, n)

#define GENERATE_ARGS(z, n, d) \
      BOOST_PP_CAT(T, n)&& BOOST_PP_CAT(t, n)

#define GENERATE_SELECT_ARGS(z, n, d) \
      BOOST_PP_CAT(t, n)
      
 #define N BOOST_PP_ITERATION()

template<typename Ch BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM(N, GENERATE_TYPENAME, ~)>
const Ch * format(Memory::StackScope& scope, const Ch * format BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM(N, GENERATE_ARGS, ~))
{
   using namespace Detail;
   std::locale locale;

   size_t length = stringlength(format);

   Ch startDelimiter = static_cast<Ch>('{');
   Ch endDelimiter = static_cast<Ch>('}');
   Ch flagDelimter = static_cast<Ch>('|');

   const Ch * partialStart = format;
   const Ch * commandBegin = format;

   const Ch * begin = format; 
   const Ch * end = format + length;

   bool insideCommand = false;
   size_t currentIndexBuffer = 0;
   Ch indexBuffer[2];

   size_t commandsSeen[N];
   for (size_t i = 0; i < N; ++i)
   {
      commandsSeen[i] = 0;
   }
   
   Engine::Memory::BasicStream<Ch> out(scope);

   while (begin != end)
   {
      if (insideCommand)
      {
         size_t targetIndex = ~0u;

         if (begin[0] == endDelimiter)
         {
            if (begin == commandBegin + 1)
            {
               targetIndex = countSeenCommands(commandsSeen);
            } else 
            {
               targetIndex = limitedAtoi(indexBuffer);
            }

            assert(targetIndex < N);

            begin++;
            insideCommand = false;

            out.write(partialStart, commandBegin - partialStart);
            select(out, targetIndex BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM(N, GENERATE_SELECT_ARGS, ~));\

            commandsSeen[targetIndex] = true;

            partialStart = begin;
            continue;
         }
         else if (std::isdigit(*begin, locale))
         {
            if (currentIndexBuffer == 2)
            {
               //Error state.
            }
            else
            {
               indexBuffer[currentIndexBuffer++] = *begin;
            }
         }
         else if (*begin == flagDelimter)
         {
            //No flags supported
         }

         ++begin;
      }
      else 
      {
         //Escaped {. 
         if (begin[0] == startDelimiter && begin[1] == startDelimiter) 
         {
            out.write(partialStart, begin - partialStart);
            partialStart = begin + 1;
            begin++;
         }
         //Escaped }}
         else if (begin[0] == endDelimiter && begin[1] == endDelimiter)
         {
            out.write(partialStart, begin - partialStart);
            partialStart = begin + 1;
            begin++; 
         }
         else if (!insideCommand && begin[0] == startDelimiter)
         {
            insideCommand = true;
            commandBegin = begin;

            currentIndexBuffer = 0;
            indexBuffer[0] = indexBuffer[1] = 0;
         }

         ++begin;
      }
   }
   
   if (partialStart != end)
   {
      out.write(partialStart, end - partialStart);
   }

#ifdef _DEBUG
   //Make sure that every parameter was used.
#endif

   return out.c_str();
};
#endif 