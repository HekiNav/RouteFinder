#pragma once

#include <cstdint>

namespace rf
{
	struct SearchParameters
	{
		uint32_t transferTimeMinutes = 1;
		uint32_t maxTransfers = 2;
		float transferSpeedMetersPerMinute = 71.0f;
	};
}