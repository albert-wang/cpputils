#include <iostream>
#include <boost/format.hpp>
#include <string>
#include <sys/time.h>

#include "stackalloc.h"
#include "stackstream.h"
#include "tinyformat.h"

class Microprofiler
{
public:
	Microprofiler(const char * name)
		:name(name)
	{
		gettimeofday(&start, nullptr);
	}

	~Microprofiler()
	{
		timeval end; 
		gettimeofday(&end, nullptr);

		size_t passed = 1000 * 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
		std::cout << name << ": " << passed << "\n";
	}
private:
	const char * name;
	timeval start;
};



int main(int argc, char * argv[])
{
	size_t n = 1000 * 10;
	{
		Microprofiler p = Microprofiler("Dongs");

		
		for (size_t i = 0; i < n; ++i)
		{
			std::stringstream s;
			for (size_t j = 0; j < 5; ++j)
			{
				s << i << "hello, world" << i << i << i << i << i << i << "\n";
			}
			s.str().c_str();
			s.str("");
		}
	}

	{
		Microprofiler p = Microprofiler("Dongs");
		Engine::Memory::StackAllocator alloc(1024 * 8 * 8);
		
		for (size_t i = 0; i < n; ++i)
		{
			Engine::Memory::StackScope scope(&alloc);	
			Engine::Memory::Stream s(scope);
			for (size_t j = 0; j < 5; ++j)
			{
				s << i << "hello, world" << i << i << i << i << i << i << "\n";
			}

			s.c_str();
		}
	}
}
