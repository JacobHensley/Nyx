#include "NXpch.h"
#include "Nyx/Asset.h"

namespace Nyx {

	static std::random_device rd;
	static std::mt19937_64 eng(rd());

	static std::uniform_int_distribution<uint64_t> distr;

	UUID::UUID()
		: m_UUID(distr(eng))
	{
	}

	UUID::UUID(uint64_t UUID)
		: m_UUID(UUID)
	{
	}

}