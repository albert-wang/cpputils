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
		std::cout << name << ": " << passed / 1000 << "ms\n";
	}
private:
	const char * name;
	timeval start;
};

int main(int argc, char * argv[])
{
	size_t iterations = 1000 * 100;
	Engine::Memory::StackAllocator alloc(1024 * 8 * 8);
	Engine::Memory::StackScope scope(&alloc);

	size_t start = alloc.remaining();
	std::wcout << Engine::format(scope, L"This is replaced {{{0}}} and this is the literal {{0}}", "Hello, World!") << "\n";

	std::wcout << Engine::format(scope, L"{0} {0} {0}, {1}\n", "Hello", 42.12f);
}
