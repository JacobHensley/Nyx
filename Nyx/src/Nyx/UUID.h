#pragma once
#include "Nyx/Common.h"
#include <random>

namespace Nyx {

	class UUID
	{
	public:
		UUID();

		UUID(uint64_t UUID);

		bool operator==(const UUID& other) const
		{
			return other.m_UUID == m_UUID;
		}

		operator uint64_t()
		{
			return m_UUID;
		}

		inline uint64_t GetUUID() { return m_UUID; }

	private:
		uint64_t m_UUID;
	};

	struct UUIDHash {
		std::size_t operator()(UUID e) const {
			return static_cast<std::size_t>(e);
		}
	};

}