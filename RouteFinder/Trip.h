#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include "Time.h"

namespace rf
{
	struct Trip;

	struct TripStop
	{
		std::map<size_t, Trip>::const_iterator trip;
		uint32_t stopID;
		Time arrivalTime;
		Time departureTime;
	};

	struct Trip
	{
		size_t routeID;
		std::vector< TripStop > stops;
	};

	struct TripStopReference
	{
		std::vector< TripStop >::const_iterator value;

		uint32_t getStopID() const
		{
			return value->stopID;
		}

		Time getTime() const
		{
			return value->arrivalTime;
		}
	};
}