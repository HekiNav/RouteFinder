#include "TimeTable.h"

#include "RouteBuilder.h"

rf::TimeTableEntry::TimeTableEntry(Weekday _weekday, std::map< uint16_t, rf::TripStopReference >::const_iterator _it, const std::map< uint16_t, rf::TripStopReference >& _table)
	: weekday(_weekday)
	, it(_it)
	, table(_table)
{
}

rf::TripStopReference rf::TimeTableEntry::getTrip() const
{
	return it->second;
}

rf::TimeTableEntry rf::TimeTableEntry::next(const RouteContainer& container) const
{
	auto nextIt = it;
	++nextIt;

	// Next trip not found so check next day
	if (nextIt == table.end())
	{
		return container.nextTrip(it->second.value->stopID, Time(rf::nextDay(weekday), 0));
	}
	else
	{
		return rf::TimeTableEntry(weekday, nextIt, table);
	}
}