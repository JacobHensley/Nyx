#pragma once
#include "Nyx/Asset/Asset.h"
//#include "sol/sol.hpp"
#include <String>

namespace Nyx {

	class Script : public Asset
	{
	public:
		Script(const std::string& path);

		template<typename... Args>
		void Run(Args&&... args)
		{
		//	m_Script.call(args...);
		}

		template<typename... Args>
		void CallFunction(const std::string& functionName, Args&&... args)
		{
		//	Application::GetLuaState()[functionName](args...);
		}

	private:
	//	sol::load_result m_Script;
	};

}