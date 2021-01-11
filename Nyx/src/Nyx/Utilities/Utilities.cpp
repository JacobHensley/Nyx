#include "NXpch.h"
#include "Utilities.h"

namespace Nyx {

	std::string OpenFileExplorer(const char* filter)
	{
		OPENFILENAMEA ofn;
		char szFile[100];

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		GetOpenFileNameA(&ofn);

		return (std::string)(ofn.lpstrFile);
	}

	std::string SaveFileExplorer(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		GetSaveFileNameA(&ofn);

		return (std::string)(ofn.lpstrFile);
	}

	std::vector<std::string> Tokenize(const std::string& str, const char delimiter)
	{
		std::vector<std::string> tokens;
		std::string token;

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