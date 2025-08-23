#pragma once

#include "RoutePosition.h"

namespace rf
{
	class RouteSection
	{
	public:
		void addPosition(RoutePosition position)
		{
			positions.push_back(position);
		}

		bool isOnRoute() const
		{
			return front().isOnRoute();
		}

		size_t getRouteID() const
		{
			return front().getRouteID();
		}

		const RoutePosition& front() const
		{
			return positions.front();
		}

		const RoutePosition& back() const
		{
			return positions.back();
		}

		size_t size() const
		{
			return positions.size();
		}

		auto begin() const
		{
			return positions.begin();
		}

		auto end() const
		{
			return positions.end();
		}

	private:
		std::vector< RoutePosition > positions;
	};

	class RouteLog
	{
	public:
		void addPosition(RoutePosition position)
		{
			if (log.empty() || lastPosition().isOnRoute() != position.isOnRoute())
			{
				log.emplace_back();
			}

			log.back().addPosition(position);
		}

		void print(std::ostream& stream, const RouteContainer& container) const
		{
			RoutePosition* prevPosition = nullptr;
			for (const RouteSection& section : log)
			{
				if (section.isOnRoute())
				{
					stream << section.front().getTime().str() << " " << container.getStop(section.front().getStopID()).getCode() << " -[" << container.getRoute(section.getRouteID()).getShortName() << "]-> " << container.getStop(section.back().getStopID()).getCode() << std::endl;
				}
				else
				{
					stream << section.front().getTime().str() << " " << container.getStop(section.front().getStopID()).getCode() << " -> " << container.getStop(section.back().getStopID()).getCode() << std::endl;
				}
			}
		}

		const RouteSection& currentSection() const
		{
			return log.back();
		}

		RoutePosition lastPosition()
		{
			return log.back().back();
		}

	private:
		std::vector< RouteSection > log;
	};
}