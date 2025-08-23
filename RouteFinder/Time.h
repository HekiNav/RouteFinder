#pragma once

#include <cstdint>
#include <ctime>
#include <string>

namespace rf
{
	enum class Weekday
	{
		Unknown,
		Monday,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday,
		Sunday,
		Count
	};

	static constexpr size_t WeekDayCount = (size_t)Weekday::Count;

	inline Weekday getWeekday(const std::tm& time)
	{
		int weekday = time.tm_wday;

		switch (weekday)
		{
		case 0:
			return Weekday::Sunday;
		case 1:
			return Weekday::Monday;
		case 2:
			return Weekday::Tuesday;
		case 3:
			return Weekday::Wednesday;
		case 4:
			return Weekday::Thursday;
		case 5:
			return Weekday::Friday;
		case 6:
			return Weekday::Saturday;
		default:
			return Weekday::Unknown;
		}
	}

	inline Weekday nextDay(Weekday weekday)
	{
		if (weekday >= Weekday::Monday)
		{
			if (weekday < Weekday::Sunday)
				return (Weekday)((int)weekday + 1);
			else
				return Weekday::Monday;
		}
		else
			return weekday;
	}

	inline uint16_t getMinutes(const std::tm& tm)
	{
		return tm.tm_hour * 60 + tm.tm_min;
	}

	class Time
	{
	public:
		Time()
		{
			std::time_t t = std::time(0);
			std::tm tm;
			localtime_s(&tm, &t);
			weekday = rf::getWeekday(tm);
			minutes = rf::getMinutes(tm);
		}

		Time(Weekday _weekday, uint16_t _minutes)
			: weekday(_weekday)
			, minutes(_minutes)
		{
		}

		Time(const std::time_t& time)
		{
			std::tm tm;
			localtime_s(&tm, &time);
			weekday = rf::getWeekday(tm);
			minutes = rf::getMinutes(tm);
		}

		Time(Weekday _weekday, const std::tm& time)
			: weekday(_weekday)
			, minutes(rf::getMinutes(time))
		{
		}

		Time(const std::tm& time)
			: weekday(rf::getWeekday(time))
			, minutes(rf::getMinutes(time))
		{
		}

		Time nextDay() const
		{
			return Time{ rf::nextDay(weekday), 0 };
		}

		Time addMinutes(uint32_t _minutes) const
		{
			uint32_t totalMinutes = minutes + _minutes;

			Weekday nextWeekday = weekday;
			while (totalMinutes > 1440)
			{
				nextWeekday = rf::nextDay(nextWeekday);
				totalMinutes -= 1440;
			}

			return Time(nextWeekday, totalMinutes);
		}

		Weekday getWeekday() const { return weekday; }
		uint16_t getMinutes() const { return minutes; }

		uint32_t getWeekMinutes() const
		{
			return ((uint32_t)weekday - 1) * 1440 + minutes;
		}

		std::string str()
		{
			std::string str;

			uint16_t hour = minutes / 60;

			if (hour < 10)
			{
				str += "0";
			}

			str += std::to_string( hour );
			str += ":";

			uint16_t min = minutes % 60;
			if (min < 10)
			{
				str += "0";
			}

			str += std::to_string(min);
			return str;
		}

	private:
		Weekday weekday;
		uint16_t minutes;
	};

	inline size_t diffMinutes(Time from, Time to)
	{
		int32_t minutes = (int32_t)to.getWeekMinutes() - (int32_t)from.getWeekMinutes();

		if (minutes < 0)
		{
			minutes += 60 * 24 * 7;
		}

		return minutes;
	}
}