#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace Nyx {

	class Log
	{
	public:
		static void Init();

		inline static void SetCoreLevel(spdlog::level::level_enum level) { s_CoreLogger->set_level(level); };
		inline static void SetClientLevel(spdlog::level::level_enum level) { s_ClientLogger->set_level(level); };

		inline static void SetCorePattern(const std::string& pattern) { s_CoreLogger->set_pattern(pattern); }
		inline static void SetClientPattern(const std::string& pattern) { s_ClientLogger->set_pattern(pattern); }

		inline static void DisablePattern() { s_CoreLogger->set_pattern("%v"); }
		inline static void EnablePattern() { s_CoreLogger->set_pattern("%^[%T] [%l] %n: %v%$"); }
	
	public:
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define NX_CORE_TRACE(...)    Nyx::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define NX_CORE_DEBUG(...)    Nyx::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define NX_CORE_INFO(...)     Nyx::Log::GetCoreLogger()->info(__VA_ARGS__)
#define NX_CORE_WARN(...)     Nyx::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define NX_CORE_ERROR(...)    Nyx::Log::GetCoreLogger()->error(__VA_ARGS__)
#define NX_CORE_CRITICAL(...) Nyx::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define NX_TRACE(...)	      Nyx::Log::GetClientLogger()->trace(__VA_ARGS__)
#define NX_DEBUG(...)         Nyx::Log::GetClientLogger()->debug(__VA_ARGS__)
#define NX_INFO(...)	      Nyx::Log::GetClientLogger()->info(__VA_ARGS__)
#define NX_WARN(...)	      Nyx::Log::GetClientLogger()->warn(__VA_ARGS__)
#define NX_ERROR(...)	      Nyx::Log::GetClientLogger()->error(__VA_ARGS__)
#define NX_CRITICAL(...)      Nyx::Log::GetClientLogger()->critical(__VA_ARGS__)

//Level macros
#define NX_LEVEL_TRACE        spdlog::level::trace
#define NX_LEVEL_DEBUG        spdlog::level::debug
#define NX_LEVEL_INFO         spdlog::level::info
#define NX_LEVEL_WARN         spdlog::level::warn
#define NX_LEVEL_ERROR        spdlog::level::err
#define NX_LEVEL_CRITICAL     spdlog::level::critical