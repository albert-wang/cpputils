#include "stackfwd"

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
	struct PartialText
	{
		size_t start;
		size_t end;
	};

	struct Command
	{
		size_t position;
		size_t type;
		double args[2];
		Command * next;
	};

	struct ParsedText
	{
		PartialText * parts;
		size_t length;
	};

	namespace Detail
	{
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
	}

	template<typename Ch>
	ParsedText parseCommandText(Memory::StackScope& scope, const Ch * start, const Ch * end)
	{
		ParsedText result;
		result.parts = scope.create<PartialText>();

		//Parse the text.
		bool parsingCommand = false;
		while (data != end)
		{
			const Ch * start = find(data, '['); 
			if (start == end)
			{
				//Finish it up, quit.
			}

			//Not a command
			if (start[1] == '[')
			{
			}

			//Its a command - parse it.
		}

		return result;
	}
}
