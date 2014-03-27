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


int main(int argc, char * argv[])
{
	Engine::Memory::StackAllocator alloc(1024 * 10);
	Engine::Memory::StackScope scope(&alloc);

	std::cout << Engine::format(scope, "Hello, {0} {0} {1} {0}\n", "Hi", 42);

	std::cout << Vector4(1, 0, 0, 1) * Matrix4::rotation(0, 0, 3.14159 / 2) << "\n";
	return 0;
}
