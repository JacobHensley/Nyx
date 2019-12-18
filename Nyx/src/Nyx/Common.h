#pragma once
#include "NXpch.h"

using String = std::string;
using uint = unsigned int;
using byte = unsigned char;

#ifdef NX_ENABLE_ASSERTS
#define NX_ASSERT(x, ...) { if(!(x)) { NX_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define NX_CORE_ASSERT(x, ...) { if(!(x)) { NX_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define NX_ASSERT(x, ...) x
#define NX_CORE_ASSERT(x, ...) x
#endif

namespace Nyx {

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using Scope = std::unique_ptr<T>;

}