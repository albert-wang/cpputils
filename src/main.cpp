#include <iostream>
#include <boost/format.hpp>
#include <string>
#include <sys/time.h>
#include <sstream>
#include <stdio.h>

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
		std::cout << name << ": " << passed / 1000 << "ms\n";
	}
private:
	const char * name;
	timeval start;
};

void invoke(size_t i)
{
	std::stringstream s;
	s << i << " " << i << " " << i << " " << i << " " << i << " " << i << " " << i;
	s.flush();
}


void invoke(Engine::Memory::StackAllocator& alloc, size_t i)
{
	Engine::Memory::StackScope scope(&alloc);
	Engine::Memory::Stream s(scope);
	s << i << " " << i << " " << i << " " << i << " " << i << " " << i << " " << i;
	s.flush();
}

int main(int argc, char * argv[])
{
	size_t iterations = 1000 * 100;
	Engine::Memory::StackAllocator alloc(1024 * 8 * 8);

	Engine::Memory::StackScope scope(&alloc);
	Engine::Memory::Stream s(scope);

	{
		Microprofiler profiler("format");
		for (size_t i = 0; i < iterations; ++i)
		{
			invoke(alloc, i);
		}
	}

	{
		Microprofiler profiler("sprintf");
		for (size_t i = 0; i < iterations; ++i)
		{
			char buffer[1024]; 
			sprintf(buffer, "%ld %ld %ld %ld %ld %ld %ld", i, i, i, i, i, i, i);
		}
	}

	{
		Microprofiler profiler("sstream");
		for (size_t i = 0; i < iterations; ++i)
		{
			invoke(i);
		}
	}
}
