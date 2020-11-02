#pragma once
#include "nyx/Common.h"
#include "Nyx/Application.h"
//#include "sol/sol.hpp"
#include "Nyx/Asset.h"

namespace Nyx {

	class Script : public Asset
	{
	public:
		Script(const String& path);

		template<typename... Args>
		void Run(Args&&... args)
		{
			m_Script.call(args...);
		}

		template<typename... Args>
		void CallFunction(const String& functionName, Args&&... args)
		{
			Application::GetLuaState()[functionName](args...);
		}

	private:
	//	sol::load_result m_Script;
	};

}