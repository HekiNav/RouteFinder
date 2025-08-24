#pragma once

#include <cstdint>
#include <optional>

#include "Route.h"
#include "Stop.h"
#include "Time.h"
#include "TimeTable.h"

namespace rf
{
	struct StopTimetable
	{
		std::map< uint16_t, TripStopReference > trips;
	};

	struct RouteData
	{
		std::map< size_t, Trip > trips;
		std::map< uint32_t, StopTimetable > stops;
	};

	class RouteContainer
	{
	public:
		const StopView stop(uint32_t stopID) const
		{
			return StopView(*this, stopID);
		}

		bool hasTrips(uint32_t stopID) const
		{
			for (Weekday day = Weekday::Monday; day <= Weekday::Sunday; ++(uint32_t&)day)
			{
				auto& stops = getData(day).stops;
				auto it = stops.find(stopID);

				if (it == stops.end())
				{
					continue;
				}

				if (!it->second.trips.empty())
				{
					return true;
				}
			}

			return false;
		}

		TimeTableEntry nextTrip(uint32_t stopID, const Time& time) const
		{
			auto& stops = getData(time).stops;
			auto stopIt = stops.find(stopID);

			if (stopIt == stops.end())
			{
				return nextTrip(stopID, time.nextDay());
			}

			auto& trips = getData(time).stops.find(stopID)->second.trips;
			auto it = trips.upper_bound(time.getMinutes());

			// If we don't find data from current day then check tomorrow
			if (it == trips.end())
			{
				return nextTrip(stopID, time.nextDay());
			}
			else
			{
				return TimeTableEntry(time.getWeekday(), it, trips);
			}
		}

		auto getTrips(uint32_t stopID, Weekday weekday)
		{
			return getData(weekday).stops.find(stopID)->second.trips;
		}

		const Route& getRoute(size_t routeID) const
		{
			return routes.find(routeID)->second;
		}

		const Stop& getStop(uint32_t stopID) const
		{
			return stops.find(stopID)->second;
		}

		const Stop* findStop(const std::string& code) const
		{
			auto it = stopCodes.find(code);
			if (it == stopCodes.end())
			{
				return nullptr;
			}
			else
			{
				return &it->second->second;
			}
		}

		std::optional< uint32_t > findStopID(const std::string& code)
		{
			auto it = stopCodes.find(code);
			if (it == stopCodes.end())
			{
				return std::optional< uint32_t >();
			}
			else
			{
				return std::optional< uint32_t >(it->second->first);
			}
		}

	private:
		const RouteData& getData(Weekday weekday) const
		{
			return data[(int)weekday];
		}

		const RouteData& getData(const Time& time) const
		{
			return getData(time.getWeekday());
		}

		friend class RouteBuilder;
		RouteData data[WeekDayCount];
		std::map< size_t, Route > routes;
		std::map< uint32_t, Stop > stops;
		std::map< std::string, std::map< uint32_t, Stop >::const_iterator > stopCodes;
	};
}