#pragma once
#include "Nyx/UUID.h"

namespace Nyx {

	enum class AssetType
	{
		INVAILD = 0, MESH, TEXTURE, SCRIPT
	};

    class Asset
    {
    public:
		virtual ~Asset() = default;

		bool operator==(const Asset& other) const
		{
			return false;
		}

		AssetType m_AssetType;
	};

}