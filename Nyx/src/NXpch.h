#pragma once

/* Nyx pre-compiled header */

#ifdef NX_PLATFORM_WINDOWS
	#define NOMINMAX // what?
	#include <windows.h>
	#include <Commdlg.h>
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <initializer_list>
#include <queue>