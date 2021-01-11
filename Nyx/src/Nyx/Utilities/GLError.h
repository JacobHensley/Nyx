#pragma once
#include "Nyx/Core/Core.h"
#include <glad/glad.h>
#include <algorithm>

namespace Nyx {

	enum GLLogLevel
	{
		NONE = 0, HIGH = 1, MEDIUM = 2, LOW = 3, NOTIFICATION = 4
	};

	static GLLogLevel s_GLLogLevel = GLLogLevel::NONE;

	static void SetGLLogLevel(GLLogLevel level)
	{
		s_GLLogLevel = level;
		NX_CORE_INFO("Set OpenGL logging level to: {0}", level);
	}

	void APIENTRY GLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION && s_GLLogLevel >= GLLogLevel::NOTIFICATION)
			NX_CORE_INFO(message);
		else if (severity == GL_DEBUG_SEVERITY_LOW && s_GLLogLevel >= GLLogLevel::LOW)
			NX_CORE_WARN(message);
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM && s_GLLogLevel >= GLLogLevel::MEDIUM)
			NX_CORE_ERROR(message);
		else if (severity == GL_DEBUG_SEVERITY_HIGH && s_GLLogLevel >= GLLogLevel::HIGH)
			NX_CORE_CRITICAL(message); 
	}

	void GLEnableErrorLogging()
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback(GLErrorCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);

		NX_CORE_INFO("Enabled OpenGL debug output");
	}

}