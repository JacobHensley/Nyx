#include "NXpch.h"
#include "Utilities.h"

namespace Nyx {

	String OpenFileExplorer()
	{
		OPENFILENAMEA ofn;
		char szFile[100];

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "All\0*.*\0FBX\0*.FBX\0";
		GetOpenFileNameA(&ofn);

		return (String)(ofn.lpstrFile);
	}

	std::vector<String> Tokenize(const String& str, const char delimiter)
	{
		std::vector<String> tokens;
		String token;

		for (int i = 0; i < str.length(); i++)
		{
			if (str[i] == delimiter)
			{
				tokens.push_back(token);
				token = "";
				continue;
			}
			token += str[i];
		}
		tokens.push_back(token);
		return tokens;
	}

}