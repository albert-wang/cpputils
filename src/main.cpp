#include <iostream>
#include "stackalloc.h"

static size_t idalloc = 0;
struct RAII
{
	RAII()
		:id(idalloc++)
	{
		std::cout << "Construct RAII: " << id << "\n";
	}
	explicit RAII(size_t i)
		:id(i)
	{
		std::cout << "Construct RAII: " << i << "\n";
	}

	~RAII()
	{
		std::cout << "Destruct RAII: " << id << "\n";
	}

	size_t id;
};

int main(int argc, char * argv[])
{
	using namespace Engine::Memory;

	//16KB
	StackAllocator alloc(16 * 1024 * 1024);
	StackScope scope(&alloc);

	scope.createArray<RAII>(42);
	size_t * res = scope.create<size_t>(42);
	std::cout << *res << "\n";

	return 0;
}
