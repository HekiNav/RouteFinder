#pragma once

#include <cmath>

namespace rf
{
	struct Point
	{
		float lat;
		float lon;
	};

	inline float distance(const Point& p1, const Point& p2)
	{
		static constexpr double r = 1000.0f * 6371.0; // km
		static constexpr double PI = 3.14159265358979323846;
		static constexpr double p = PI / 180.0;
		const double a = 0.5 - cos((p2.lat - p1.lat) * p) * 0.5 + cos(p1.lat * p) * cos(p2.lat * p) * (1.0 - cos((p2.lon - p1.lon) * p)) * 0.5;
		return (float)(2.0 * r * asin(sqrt(a)));
	}
}