#include <iostream>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <string>
#include <sys/time.h>
#include <sstream>
#include <stdio.h>

#include "vector.h"
#include "matrix.h"

int main(int argc, char * argv[])
{
	using namespace Math;

	Vector4 a = Vector4(0, 0, 0) * Matrix4::translate(10, 0, 0) * Matrix4::scale(2, 2, 1);
	std::cout << a[0] << "\n";

	return 0;
}
