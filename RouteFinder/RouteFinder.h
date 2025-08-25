#pragma once

#include "RouteTraveller.h"

#include <set>

namespace rf
{
	static uint32_t travellerTime(const RouteTraveller& traveller)
	{
		uint32_t startTime = traveller.getLog().firstPosition().getTime().getWeekMinutes();
		uint32_t endTime = traveller.getLog().lastPosition().getTime().getWeekMinutes();
		return startTime < endTime ? endTime - startTime : 7 * 24 * 60 - startTime + endTime;
	}

	template< auto EvalFunc = travellerTime > class RouteFinder
	{
	public:
		RouteFinder(RouteContainer& _container)
			: container(_container)
		{
		}

		void addTraveller(std::multimap< uint32_t, RouteTraveller >& travellers, const RouteTraveller& traveller)
		{
			auto value = EvalFunc(traveller);
			travellers.insert(std::make_pair(value, traveller));
		}

		RouteLog search(std::time_t time, uint32_t beginStopID, uint32_t endStopID, const SearchParameters& params = SearchParameters{})
		{
			std::multimap< uint32_t, RouteTraveller > travellers;
			std::map< uint32_t, Time > visitedStops;

			Time startTime = Time(time);
			RouteTraveller traveller(container, beginStopID, time);
			addTraveller(travellers, traveller);

			while (true)
			{
				RouteTraveller current = travellers.begin()->second;
				travellers.erase(travellers.begin());

				const uint32_t stopID = current.getStopID();
				if (stopID == endStopID)
				{	
					std::cout << "found route";
					return current.getLog(); 
				}

				if (current.canWaitNext())
				{
					addTraveller(travellers, current.waitNext());
				}

				if (current.canEmbark())
				{
					addTraveller(travellers, current.embark().travelNext());
				}

				auto visitIt = visitedStops.find(stopID);
				const bool stopVisited = visitedStops.find(stopID) != visitedStops.end() && diffMinutes(startTime, visitIt->second) < diffMinutes(startTime, current.getTime());

				if (!stopVisited)
				{
					if (current.isOnStop())
					{
						visitedStops[stopID] = current.getTime();
					}

					if (current.canTravelNext())
					{
						addTraveller(travellers, current.travelNext());
					}

					if (current.canDisembark())
					{
						addTraveller(travellers, current.disembark(params.transferTimeMinutes));
					}

					while (current.canTransferNext(params))
					{
						addTraveller(travellers, current.transferNext(params));
					}
				}
			}
		}

	private:
		const RouteContainer& container;
	};
}