#pragma once

#include <string>

#include "Point.h"
#include "TimeTable.h"

namespace rf
{
	class Stop
	{
	public:
		Stop() = default;
		Stop(const std::string& _code, const std::string& _name, const Point& _position)
			: code(_code)
			, name(_name)
			, position(_position)
		{
		}

		const std::string& getCode() const
		{
			return code;
		}

		const std::string& getName() const
		{
			return name;
		}

		float distanceTo(const Stop& other) const
		{
			return distance(position, other.position);
		}

		const std::multimap< float, std::map< uint32_t, Stop >::const_iterator >& getTransfers() const
		{
			return transfers;
		}

	private:
		friend class RouteBuilder;

		Point position;
		std::string code;
		std::string name;
		std::multimap< float, std::map< uint32_t, Stop >::const_iterator > transfers;
	};

	class RouteContainer;

	class StopView
	{
	public:
		StopView(const RouteContainer& _container, uint32_t _stopID);

		TimeTableEntry nextTrip(const Time& time) const;

		uint32_t getStopID() { return stopID; }

	private:
		friend class StopTimeTable;
		const RouteContainer& container;
		uint32_t stopID;
	};
}