#include "stackfwd.h"
#include <locale>

namespace Engine
{
	//These are string formatting methods. Up to 15 arguments
	template<typename T0>
	const char * format(Memory::StackScope& scope, const char * format, T0&& t0)
	{
		while(format)
		{
		}

		return NULL;
	};

	template<typename T0>
	const wchar_t * wformat(Memory::StackScope& scope, const wchar_t * format, T0&& t0)
	{
		return NULL;
	};

	//These parse strings in the format "data [[command:argu,ments]]"
	
	//A partial text is a range of characters, and the command that follows it.	
	struct PartialText
	{
		boost::uint32_t start;
		boost::uint16_t length;
		boost::uint16_t commandID;
		double argument;
	};

	struct ParsedText
	{
		PartialText * parts;
		size_t length;
	};

	namespace Detail
	{
		struct CommandLookup 
		{
			const char * name;
			size_t identifier;
		};

		template<typename Ch>
		const Ch * find(const Ch * data, const Ch * end, Ch target)
		{
			while(data != end)
			{
				if (*data == target)
				{
					return data;
				}
				data++;
			}

			return data;
		}

		template<typename Ch>
		const Ch * findClosingBrackets(const Ch * data, const Ch * end)
		{
			while (data < end - 2) 
			{
				if (data[0] == ']' && data[1] == ']') 
				{
					return data;
				}

				if (data[1] != ']')
				{
					data += 2;
				}
				else 
				{
					data += 1;
				}
			}

			return end;
		}

		template<typename Ch>
		size_t estimateCommands(const Ch * data, const Ch * end)
		{
			size_t count = 0;
			size_t depth = 0;

			while(data != end - 1)
			{
				if (depth == 0 && data[0] == '[' && data[1] == '[')
				{
					count += 1;
					depth = 1;
				}

				if (data[0] == ']' && data[1] == ']')
				{
					depth = 0;
				}

				++data;
			}
			

			//One per command, plus one for every 65k characters, plus one for terminating.
			return count + ((end - data) >> 16) + 1;
		}

		template<typename Ch>
		size_t checkCommand(const Ch * start, const Ch * end)
		{
			static CommandLookup commands[] = {
				{ "setx", 1 }
			};

			for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i)
			{
				const char * cstart = commands[i].name;
				bool found = true;
				for (const Ch * s = start; s != end && *cstart; ++s)
				{
					if (*s != static_cast<Ch>(*cstart++))
					{
						found = false;
						break;
					}
				}

				if (found)
				{
					return commands[i].identifier;
				}
			}

			return 0;
		}
	}

	template<typename Ch>
	ParsedText parseCommandText(Memory::StackScope& scope, const Ch * start, const Ch * end)
	{
		using namespace Detail;

		//Estimate the number of commands.
		size_t estimate = estimateCommands(start, end);

		ParsedText result;
		result.parts = scope.createArrayPOD<PartialText>(estimate);

		PartialText * current = &result.parts[0];
		current->start = 0;

		//Parse the text.
		size_t filled = 0;

		const Ch * data = start;
		while (data != end)
		{
			if (data + 1 < end)
			{
				if (data[0] == '[' && data[1] == '[')
				{
					current->length = (data - start) - current->start;

					const Ch * cbegin = data + 2; 
					const Ch * cend = find(data + 2, end, ':');
					if (cend == end) 
					{
						//This command seems to go to the end of the string, so its not really a command.
						current->length = (end - start) - current->start;
					}
					else 
					{
						size_t command = checkCommand(cbegin, cend);
						current->commandID = command;

						const Ch * closingBracket = findClosingBrackets(cend, end);
						//Find the end of the command.
						if (closingBracket == end)
						{
							//Not really a command then - just some text.
							current->commandID = 0;
							current->length = (end - start) - current->start;
						} 
						else 
						{
							//Parse out the argument.
							const Ch * argBegin = cend + 1;
							current->argument = atof(argBegin);

							filled += 1;
							current = &result.parts[filled];
							current->start = (closingBracket - start) + 2;
						}
					}
				}
			}
			++data;
		}

		current->length = (end - start) - current->start;
		result.length = filled + 1;
		return result;
	}
}
