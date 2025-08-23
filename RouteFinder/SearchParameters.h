#pragma once

#include <cstdint>

namespace rf
{
	struct SearchParameters
	{
		uint32_t transferTimeMinutes = 1;
		float transferSpeedMetersPerMinute = 71.0f;
	};
}