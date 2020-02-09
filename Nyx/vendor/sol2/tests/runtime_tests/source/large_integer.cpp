// sol3

// The MIT License (MIT)

// Copyright (c) 2013-2019 Rapptz, ThePhD and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "sol_test.hpp"

#include <catch.hpp>

#include <cstdint>
#include <limits>

inline namespace sol2_test_large_integer {
	inline bool bool_roundtrip(bool num) {
		REQUIRE(num == true);
		return num;
	}

	inline void trigger_passthrough_crash(sol::state& lua) {
		sol::protected_function pf = lua["f"];
		auto result = pf(0xFFFFFFFFFFFFFFFFull);
		(void)result;
	}

	template <typename T>
	inline T intT_passthrough(T num) {
		return num;
	}

	template <typename T>
	inline T intT_roundtrip(T num) {
		REQUIRE(num == std::numeric_limits<T>::max());
		return num;
	}

	inline std::uint64_t int53_roundtrip(std::uint64_t num) {
		REQUIRE(num == 0x1FFFFFFFFFFFFFull);
		return num;
	}
} // namespace sol2_test_large_integer

TEST_CASE("large_integer/bool", "pass bool integral value to and from lua") {
	sol::state lua;
	lua.open_libraries();
	lua.set_function("f", bool_roundtrip);
	sol::optional<sol::error> result1 = lua.safe_script(
	     "x = f(true)\n"
	     "assert(x == true)",
	     sol::script_pass_on_error);
	REQUIRE_FALSE(result1.has_value());
	sol::object x = lua["x"];
	REQUIRE(x.is<bool>());
	REQUIRE(x.as<bool>() == true);
	REQUIRE_FALSE(x.is<std::int32_t>());
	{
		auto result = lua.safe_script("f(1)", sol::script_pass_on_error);
		REQUIRE_FALSE(result.valid());
	}
}

TEST_CASE("large_integers/unsigned32", "pass large unsigned 32bit values to and from lua") {
	using T = std::uint32_t;
	sol::state lua;
	lua.open_libraries();
	lua.set_function("f", intT_roundtrip<T>);
	auto result1 = lua.safe_script(
	     "x = f(0xFFFFFFFF)\n"
	     "assert(x == 0xFFFFFFFF)",
	     sol::script_pass_on_error);
	REQUIRE(result1.valid());
	sol::object x = lua["x"];
	REQUIRE(x.is<T>());
	REQUIRE(x.as<T>() == 0xFFFFFFFF);
}

TEST_CASE("large_integer/unsigned53", "pass large unsigned 53bit value to and from lua") {
	using T = std::uint64_t;
	sol::state lua;
	lua.open_libraries();
	lua.set_function("f", int53_roundtrip);
	auto result1 = lua.safe_script(
	     "x = f(0x1FFFFFFFFFFFFF)\n"
	     "assert(x == 0x1FFFFFFFFFFFFF)");
	REQUIRE(result1.valid());
	sol::object x = lua["x"];
	REQUIRE(x.is<T>());
	REQUIRE(x.as<T>() == 0x1FFFFFFFFFFFFFull);
}

TEST_CASE("large_integer/unsigned64", "pass too large unsigned 64bit value to lua") {
	using T = std::int64_t;
	sol::state lua;
	lua.set_function("f", intT_passthrough<T>);
	REQUIRE_THROWS(trigger_passthrough_crash(lua));
}

TEST_CASE("large_integer/double", "pass negative and large positive values as signed and unsigned from and to lua") {
	sol::state lua;
	lua.open_libraries();
	lua.set_function("s32", intT_passthrough<std::int32_t>);
	lua.set_function("s64", intT_passthrough<std::int64_t>);
	lua.set_function("u32", intT_passthrough<std::uint32_t>);
	lua.set_function("u64", intT_passthrough<std::uint64_t>);
	{
		// signed 32bit
		auto result1 = lua.safe_script("x = s32(-1)", sol::script_pass_on_error);
		REQUIRE(result1.valid());
		auto result2 = lua.safe_script("assert(x == -1)", sol::script_pass_on_error);
		REQUIRE(result2.valid());
		auto result3 = lua.safe_script("x = s32(0xFFFFFFFF)", sol::script_pass_on_error);
		REQUIRE(result3.valid());
		auto result4 = lua.safe_script("assert(x == -1)", sol::script_pass_on_error);
		REQUIRE(result4.valid());

		sol::object x = lua["x"];
		REQUIRE(x.is<std::int32_t>());
		REQUIRE(x.as<std::int32_t>() == -1);
		REQUIRE(x.is<std::uint32_t>());
		REQUIRE(x.as<std::uint32_t>() == 0xFFFFFFFF);
	}

	// unsigned 32bit
	{
		auto result1 = lua.safe_script("x = u32(0xFFFFFFFF)", sol::script_pass_on_error);
		REQUIRE(result1.valid());
		auto result2 = lua.safe_script("assert(x == 0xFFFFFFFF)", sol::script_pass_on_error);
		REQUIRE(result2.valid());
		auto result3 = lua.safe_script("x = u32(-1)", sol::script_pass_on_error);
		REQUIRE(result3.valid());
		auto result4 = lua.safe_script("assert(x == 0xFFFFFFFF)", sol::script_pass_on_error);
		REQUIRE(result4.valid());
		sol::object x = lua["x"];
		REQUIRE(x.is<std::int32_t>());
		REQUIRE(x.as<std::int32_t>() == -1);
		REQUIRE(x.is<std::uint32_t>());
		REQUIRE(x.as<std::uint32_t>() == 0xFFFFFFFF);
	}
	// signed 64bit
	{
		auto result1 = lua.safe_script("x = s64(-1)", sol::script_pass_on_error);
		REQUIRE(result1.valid());
		auto result2 = lua.safe_script("assert(x == -1)", sol::script_pass_on_error);
		REQUIRE(result2.valid());
		sol::object x = lua["x"];
		REQUIRE(x.is<std::int64_t>());
		REQUIRE(x.as<std::int64_t>() == -1);
		REQUIRE(x.is<std::uint64_t>());
		REQUIRE(x.as<std::uint64_t>() == 0xFFFFFFFFFFFFFFFFull);
	}
}
