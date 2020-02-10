#include "NXpch.h"
#include "Script.h"

namespace Nyx {

	Script::Script(const String& path)
	{
		Application::GetLuaState().open_libraries(sol::lib::base);
		m_Script = Application::GetLuaState().load_file(path);
	}

}