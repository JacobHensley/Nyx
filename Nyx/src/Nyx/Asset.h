#pragma once
#include "Nyx/UUID.h"

namespace Nyx {

    class Asset
    {
    public:
		virtual ~Asset() = default;

		bool operator==(const Asset& other) const
		{
			return false;
		}
	};

}