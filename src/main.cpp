#include <iostream>
#include <boost/format.hpp>
#include <string>
#include <sys/time.h>
#include <sstream>
#include <stdio.h>

#include "stackalloc.h"
#include "stackstream.h"
#include "tinyformat.h"
#include "interpolator.h"

#include "matrix.h"
#include "vector.h"

#include "channel.h"

class Microprofiler
{
public:
	Microprofiler(const char * name)
		:name(name)
	{
		gettimeofday(&start, NULL);
	}

	~Microprofiler()
	{
		timeval end; 
		gettimeofday(&end, NULL); 

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
	using namespace Engine::Threading;
	Bidirectional b;

	b.subscribe(&b);
	b.send(1, 2, 3);

	while (Message msg = b.pop())
	{
		std::cout << msg.type << "\n";
	}

	return 0;
}
