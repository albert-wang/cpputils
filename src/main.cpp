#include <iostream>
#include <boost/format.hpp>
#include <string>

#include "stackalloc.h"
#include "stackstream.h"

struct RAII
{
	size_t id;

	RAII(size_t i)
		:id(i)
	{
		std::cout << "RAII: " << id << "\n";
	}

	~RAII()
	{
		std::cout << "~RAII: " << id << "\n";
	}
};

int main(int argc, char * argv[])
{
	using namespace Engine::Memory;
	using namespace Engine;

	//16KB
	StackAllocator alloc(16 * 1024 * 1024);
	StackScope scope(&alloc);

	Engine::WideMemoryStream sink(scope);

	for (size_t i = 0; i < 32; ++i)
	{
		scope.create<RAII>(i);
		sink << L"First: " << i << L"\n";
	}

	std::wcout << sink.c_str();

	return 0;
}
