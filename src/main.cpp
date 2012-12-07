#include <iostream>
#include <boost/format.hpp>
#include <string>

#include "stackalloc.h"
#include "stackstream.h"
#include "tinyformat.h"

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

void printText(const char * text, const Engine::ParsedText& parsed)
{
	for (size_t i = 0; i < parsed.length; ++i)
	{
		const Engine::PartialText& part = parsed.parts[i];
		std::cout << "[" << i << "] (" << part.start << ", " << part.length << ") " << std::string(text + part.start, part.length) << "\n";
		std::cout << "\tID: " << part.commandID << "(" << part.argument << ")\n";
	}
}

int main(int argc, char * argv[])
{
	using namespace Engine::Memory;
	using namespace Engine;

	//16KB
	StackAllocator alloc(16 * 1024 * 1024);
	StackScope scope(&alloc);

	const char * text = "This is a test[[setx: 32451.0243]] of something. [[setx: 0x0000FFFF]]There should only be one.";
	ParsedText parsed = parseCommandText(scope, text, text + strlen(text));

	printText(text, parsed);
	
	std::cout.precision(10);
	std::cout << 12351.325 << "\n";
	std::cout << atof("23414.0145") << "\n";

	return 0;
}
