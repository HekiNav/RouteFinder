#pragma once

#include <optional>
#include <string>
#include <map>
#include <vector>
#include <sstream>

#include "CSVReader.h"
#include "RouteContainer.h"
#include "Stop.h"
#include "Time.h"
#include "TimeTable.h"
#include "Trip.h"

namespace rf
{
	struct TripReader
	{
		size_t routeID;
		std::tm date;
		Weekday day;

		void row(std::string routeName, std::tm _date, char dayName[3], uint8_t direction, std::_Ignore)
		{
			if (strcmp(&dayName[0], "Ma") == 0)
			{
				day = Weekday::Monday;
			}
			else if (strcmp(&dayName[0], "Ti") == 0)
			{
				day = Weekday::Tuesday;
			}
			else if (strcmp(&dayName[0], "Ke") == 0)
			{
				day = Weekday::Wednesday;
			}
			else if (strcmp(&dayName[0], "To") == 0)
			{
				day = Weekday::Thursday;
			}
			else if (strcmp(&dayName[0], "Pe") == 0)
			{
				day = Weekday::Friday;
			}
			else if (strcmp(&dayName[0], "La") == 0)
			{
				day = Weekday::Saturday;
			}
			else if (strcmp(&dayName[0], "Su") == 0)
			{
				day = Weekday::Sunday;
			}
			else
			{
				day = Weekday::Unknown;
			}

			routeID = std::hash<std::string>{}(routeName);
			date = _date;
		}

		void finalize()
		{
		}
	};

	using StopTimetableReference = std::map< uint32_t, StopTimetable >::const_iterator;

	class Transfer
	{
		uint32_t distanceMeters;
		StopTimetableReference stopTimeTable;
	};

	struct BuildSettings
	{
		float maxTransferDistance;
	};

	class RouteBuilder
	{
	public:
		RouteBuilder(RouteContainer& _container, const BuildSettings& _settings)
			: container(_container)
			, settings(_settings)
			, tripData()
			, tripReader(tripData, '_', false)
		{
			tripReader.setTimeFormat("%Y%m%d");
		}

		void row(const std::string& tripID, const std::tm& arrivalTime, const std::tm& departureTime, uint32_t stopID, std::_Ignore, std::_Ignore, std::_Ignore, float shapeDistTravelled, bool timePoint, std::_Ignore)
		{
			std::stringstream ss(tripID);
			tripReader.read(ss);

			size_t tripHash = std::hash< std::string>{}(tripID);

			auto tempIt = container.data[(size_t)tripData.day].trips.end();

			Trip& trip = container.data[(size_t)tripData.day].trips.insert(std::make_pair(tripHash, Trip{ tripData.routeID })).first->second;
			trip.stops.emplace_back(tempIt, stopID, Time(tripData.day, arrivalTime), Time(tripData.day, departureTime));
			++rowsRead;
		}

		void row(const std::string& routeID, std::_Ignore, std::string routeShortName, std::_Ignore, std::_Ignore, std::_Ignore, std::_Ignore)
		{
			size_t routeHash = std::hash< std::string>{}(routeID);
			container.routes[routeHash] = Route(routeShortName);
		}

		void row(uint32_t stopID, std::string stopCode, std::string stopName, std::_Ignore, float lat, float lon, std::_Ignore, std::_Ignore, std::_Ignore, std::_Ignore, std::_Ignore, std::_Ignore, std::_Ignore, std::_Ignore)
		{
			container.stops[stopID] = Stop(stopCode, stopName, Point(lat, lon));
		}

		void finalize()
		{
			for (size_t day = 0; day < WeekDayCount; ++day)
			{
				auto& trips = container.data[day].trips;
				for (auto tripIt = trips.begin(); tripIt != trips.end(); ++tripIt)
				{
					auto& [tripID, trip] = *tripIt;
					for (auto it = trip.stops.begin(); it != trip.stops.end(); ++it)
					{
						it->trip = tripIt;
						container.data[day].stops[it->stopID].trips[it->arrivalTime.getMinutes()] = TripStopReference{ it };
					}
				}
			}

			for (auto it = container.stops.begin(); it != container.stops.end(); ++it)
			{
				if (!container.hasTrips(it->first))
				{
					continue;
				}

				container.stopCodes[it->second.getCode()] = it;

				auto otherIt = it;
				++otherIt;

				for (; otherIt != container.stops.end(); ++otherIt)
				{
					float dist = it->second.distanceTo(otherIt->second);

					if (dist > settings.maxTransferDistance || !container.hasTrips(otherIt->first) || !container.hasTrips(it->first))
					{
						continue;
					}

					it->second.transfers.insert(std::make_pair(dist, otherIt));
					otherIt->second.transfers.insert(std::make_pair(dist, it));
				}
			}
		}

		RouteContainer& container;
		BuildSettings settings;
		TripReader tripData;
		CSVReader< TripReader, std::string, std::tm, char[3], uint8_t, std::_Ignore > tripReader;

		uint32_t rowsRead = 0;
	};
}