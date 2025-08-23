#pragma once

#include <map>
#include <variant>

#include "Stop.h"
#include "Time.h"
#include "TimeTable.h"
#include "Trip.h"

namespace rf
{
	class OnStop
	{
	public:
		OnStop(uint32_t _stopID, const Time& _time)
			: stopID(_stopID)
			, time(_time)
		{
		}

		const Time& getTime() const
		{
			return time;
		}

		uint32_t getStopID() const
		{
			return stopID;
		}

		TimeTableEntry next(const RouteContainer& container) const
		{
			return container.stop(stopID).nextTrip(time);
		}

	private:
		uint32_t stopID;
		Time time;
	};

	class TransferEnumerator
	{
	public:
		TransferEnumerator() = default;

		TransferEnumerator(const Time& _time, const std::multimap< float, std::map< uint32_t, Stop >::const_iterator >& transfers)
			: time(_time)
			, it(transfers.begin())
			, end(transfers.end())
		{
		}

		bool hasNext() const
		{
			return it != end;
		}

		std::pair<float, uint32_t> next()
		{
			const float distance = it->first;
			uint32_t nextStopID = it->second->first;
			++it;
			return std::make_pair(distance, nextStopID);
		}

	private:
		Time time;
		std::multimap< float, std::map< uint32_t, Stop >::const_iterator >::const_iterator it;
		std::multimap< float, std::map< uint32_t, Stop >::const_iterator >::const_iterator end;
	};

	class RoutePosition
	{
	public:
		RoutePosition(TimeTableEntry entry)
			: value(entry)
		{
		}

		RoutePosition(TripStopReference stop)
			: value(stop)
		{
		}

		RoutePosition(const OnStop& stop)
			: value(stop)
		{
		}

		uint32_t getStopID() const
		{
			return std::visit([](auto& arg) -> uint32_t { return arg.getStopID(); }, value);
		}

		TripStopReference getEndStop() const
		{
			return TripStopReference{ getTrip().stops.end() };
		}

		const Trip& getTrip() const
		{
			return std::get< TripStopReference >(value).value->trip->second;
		}

		size_t getRouteID() const
		{
			return std::get< TripStopReference >(value).value->trip->second.routeID;
		}

		Time getTime() const
		{
			return std::visit([](auto& arg) -> Time { return arg.getTime(); }, value);

			/*
			if (isOnRoute())
			{
				return std::get< TripStopReference >(value).value->arrivalTime;
			}
			else
			{
				return std::get< TimeTableEntry >(value).getTime();
			}*/
		}

		bool isOnRoute() const
		{
			return std::holds_alternative<TripStopReference>(value);
		}

		bool isOnTimeTable() const
		{
			return std::holds_alternative< TimeTableEntry >(value);
		}

		bool isOnStop() const
		{
			return std::holds_alternative< OnStop >(value);
		}

		bool canWaitNext() const
		{
			return !isOnRoute();
		}

		RoutePosition waitNext(const RouteContainer& container) const
		{
			if (isOnTimeTable())
			{
				return RoutePosition(std::get< TimeTableEntry >(value).next(container));
			}
			else
			{
				return RoutePosition(std::get< OnStop >(value).next(container));
			}
		}

		bool canEmbark() const
		{
			return isOnTimeTable() && !isAtEndStop();
		}

		RoutePosition embark() const
		{
			return RoutePosition(std::get< TimeTableEntry >(value).getTrip());
		}

		bool canDisembark() const
		{
			return isOnRoute();
		}

		RoutePosition disembark(const RouteContainer& container, uint32_t transferTimeMinutes) const
		{
			TripStopReference tripStopRef = std::get< TripStopReference >(value);
			OnStop stop{ tripStopRef.getStopID(), tripStopRef.getTime().addMinutes(transferTimeMinutes) };
			return RoutePosition(stop);
		}

		bool canTravelNext() const
		{
			return isOnRoute() && !isAtEndStop();
		}

		RoutePosition travelNext() const
		{
			TripStopReference tripStopRef = std::get< TripStopReference >(value);
			++tripStopRef.value;
			return RoutePosition(tripStopRef);
		}

		bool canTransfer() const
		{
			return isOnStop();
		}

		TransferEnumerator transfers(const RouteContainer& container)
		{
			return TransferEnumerator(getTime(), container.getStop(std::get< OnStop >(value).getStopID()).getTransfers());
		}

		bool isAtEndStop() const
		{
			TripStopReference tripStopRef;
			if (isOnRoute())
			{
				tripStopRef = std::get< TripStopReference >(value);
			}
			else
			{
				tripStopRef = std::get< TimeTableEntry >(value).getTrip();
			}

			return tripStopRef.value == --tripStopRef.value->trip->second.stops.end();
		}

	private:
		std::variant< TimeTableEntry, TripStopReference, OnStop > value;
	};
}