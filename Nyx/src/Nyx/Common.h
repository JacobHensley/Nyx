#pragma once
#include "NXpch.h"

typedef std::string String;

#ifdef NX_ENABLE_ASSERTS
	#define NX_ASSERT(x, ...) { if(!(x)) { NX_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define NX_CORE_ASSERT(x, ...) { if(!(x)) { NX_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define NX_ASSERT(x, ...) x
	#define NX_CORE_ASSERT(x, ...) x
#endif