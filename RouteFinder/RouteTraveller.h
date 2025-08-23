#pragma once

#include "RouteBuilder.h"
#include "RouteLog.h"
#include "SearchParameters.h"

#include <ctime>
#include <variant>

namespace rf
{
	using StopReference = std::map< uint32_t, Stop >::const_iterator;

	class RouteTraveller
	{
	public:
		RouteTraveller(const RouteContainer& _container, uint32_t stopID, const Time& time = Time())
			: container(_container)
			, position(OnStop(stopID, time))
			, transfers(position.transfers(_container))
		{
			log.addPosition(position);
		}

		uint32_t getStopID() const
		{
			return position.getStopID();
		}

		Time getTime() const
		{
			return position.getTime();
		}

		bool isOnRoute() const
		{
			return position.isOnRoute();
		}

		bool isOnStop() const
		{
			return position.isOnStop();
		}

		bool canWaitNext() const
		{
			return position.canWaitNext();
		}

		RouteTraveller waitNext() const
		{
			return RouteTraveller(container, position.waitNext(container), log);
		}

		bool canEmbark() const
		{
			return position.canEmbark();
		}

		RouteTraveller embark() const
		{
			return RouteTraveller(container, position.embark(), log);
		}

		bool canDisembark() const
		{
			return position.canDisembark();
		}

		RouteTraveller disembark(uint32_t transferTimeMinutes) const
		{
			return RouteTraveller(container, position.disembark(container, transferTimeMinutes), log);
		}

		bool canTravelNext() const
		{
			return position.canTravelNext();
		}

		RouteTraveller travelNext() const
		{
			return RouteTraveller(container, position.travelNext(), log);
		}

		bool canTransferNext() const
		{
			return position.canTransfer() && transfers.hasNext();
		}

#pragma optimize("", off)
		RouteTraveller transferNext(const SearchParameters& params)
		{
			auto [distance, stopID] = transfers.next();
			Time nextTime = getTime().addMinutes(uint32_t(ceil(distance / params.transferSpeedMetersPerMinute)));
			return RouteTraveller(container, container.nextTrip(stopID, nextTime), log);
		}

		const RouteLog& getLog() const
		{
			return log;
		}

	private:
		RouteTraveller(const RouteContainer& _container, const RoutePosition& _position, const RouteLog& _log)
			: container(_container)
			, position(_position)
			, log(_log)
		{
			if (position.isOnStop())
			{
				transfers = position.transfers(_container);
			}

			log.addPosition(position);
		}

		const RouteContainer& container;
		RoutePosition position;
		TransferEnumerator transfers;
		RouteLog log;
	};
}