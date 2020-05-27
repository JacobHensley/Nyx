#pragma once
#include "NXpch.h"
#include "glad/glad.h"

namespace Nyx {

	const char* GLenumToNameSource(GLenum source)
	{
		static const char* sources[] = {
		  "API",   "Window System", "Shader Compiler", "Third Party", "Application",
		  "Other", "Unknown"
		};

		int index = std::min((int)(source - GL_DEBUG_SOURCE_API), (int)(sizeof(sources) / sizeof(const char*)));

		return sources[index];
	}

	const char* GLenumToNameType(GLenum type)
	{
		static const char* types[] = {
		  "Error",       "Deprecated Behavior", "Undefined Behavior", "Portability",
		  "Performance", "Other",               "Unknown"
		};

		int index = std::min((int)(type - GL_DEBUG_TYPE_ERROR), (int)(sizeof(types) / sizeof(const char*)));
		return types[index];
	}

	const char* GLenumToNameSeverity(GLenum severity)
	{
		static const char* severities[] = {
		  "High", "Medium", "Low", "Notification", "Unknown"
		};

		int index = std::min((int)(severity - GL_DEBUG_SEVERITY_HIGH), (int)(sizeof(severities) / sizeof(const char*)));

		return severities[index];
	}

	enum GLLogLevel
	{
		NONE = 0, HIGH = 1, MEDIUM = 2, LOW = 3, NOTIFICATION = 4
	};

	static GLLogLevel s_GLLogLevel = GLLogLevel::NONE;

	void APIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION && s_GLLogLevel >= GLLogLevel::NOTIFICATION && false)
			NX_CORE_INFO(message);
		else if (severity == GL_DEBUG_SEVERITY_LOW && s_GLLogLevel >= GLLogLevel::LOW)
			NX_CORE_WARN(message);
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM && s_GLLogLevel >= GLLogLevel::MEDIUM)
			NX_CORE_ERROR(message);
		else if (severity == GL_DEBUG_SEVERITY_HIGH && s_GLLogLevel >= GLLogLevel::HIGH)
			NX_CORE_CRITICAL(message);
	}

	static void SetGLLogLevel(GLLogLevel level)
	{
		s_GLLogLevel = level;
		NX_CORE_INFO("Set OpenGL logging level to: {0}", level);
	}

	void GLErrorInit()
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GLErrorCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
		NX_CORE_INFO("Created OpenGL Debug Message Callback");
	}

}