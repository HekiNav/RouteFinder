#include "Stop.h"

#include "RouteContainer.h"

rf::StopView::StopView(const RouteContainer& _container, uint32_t _stopID)
	: container(_container)
	, stopID(_stopID)
{
}

rf::TimeTableEntry rf::StopView::nextTrip(const Time& time) const
{
	return container.nextTrip(stopID, time);
}