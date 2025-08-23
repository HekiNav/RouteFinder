#pragma once

#include <map>

#include "Time.h"
#include "Trip.h"

namespace rf
{
	class RouteContainer;

	class TimeTableEntry
	{
	public:
		TimeTableEntry(Weekday _weekday, std::map< uint16_t, rf::TripStopReference >::const_iterator _it, const std::map< uint16_t, rf::TripStopReference >& _table);

		TripStopReference getTrip() const;
		Time getTime() const { return Time(weekday, it->first); }
		TimeTableEntry next(const RouteContainer& container) const;

		uint32_t getStopID() const
		{
			return it->second.value->stopID;
		}


	private:
		Weekday weekday;
		std::map< uint16_t, rf::TripStopReference >::const_iterator it;
		const std::map< uint16_t, rf::TripStopReference >& table;
	};
}