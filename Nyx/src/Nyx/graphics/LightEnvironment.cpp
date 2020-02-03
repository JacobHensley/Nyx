#include "NXpch.h"
#include "LightEnvironment.h"

namespace Nyx {

	LightEnvironment::LightEnvironment()
	{
	}

	LightEnvironment::~LightEnvironment()
	{
	}

	void LightEnvironment::AddLight(Light* light)
	{
		m_Lights.push_back(light);
	}

	bool LightEnvironment::RemoveLight(Light* light)
	{
		for (int i = 0; i < m_Lights.size(); i++)
		{
			if (m_Lights[i] == light) 
			{
				m_Lights.erase(m_Lights.begin() + i);
				return true;
			}
		}
		return false;
	}

}