#pragma once
#include <string>
#include <vector>

namespace Nyx {
	
	std::string OpenFileExplorer(const char* filter);
	std::string SaveFileExplorer(const char* filter);
	std::vector<std::string> Tokenize(const std::string& str, const char delimiter);

}