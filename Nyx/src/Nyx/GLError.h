#pragma once
#include "NXpch.h"
#include "glad/glad.h"

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
	  "High", "Medium", "Low", "Unknown"
	};

	int index = std::min((int)(severity - GL_DEBUG_SEVERITY_HIGH), (int)(sizeof(severities) / sizeof(const char*)));

	return severities[index];
}

void APIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	int n = 0;
	String shortMessage;
	for (int i = 0; i < length; i++)
	{
		if (n >= 2)
		{
			shortMessage += message[i];
		}
		if (message[i] == ')')
			n++;
	}

	NX_CORE_ERROR("{0}: ({1}): {2}", GLenumToNameType(type), id, shortMessage);

	if (severity == GL_DEBUG_SEVERITY_HIGH)
		__debugbreak();
}

void GLErrorInit() 
{
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLErrorCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}