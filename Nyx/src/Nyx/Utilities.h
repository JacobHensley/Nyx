#pragma once
#include "Common.h"

namespace Nyx {
	
	String OpenFileExplorer(const char* filter);
	std::vector<String> Tokenize(const String& str, const char delimiter);

}