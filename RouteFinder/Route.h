#pragma once

#include <string>

namespace rf
{
	class Route
	{
	public:
		Route() = default;
		Route(const std::string& _shortName)
			: shortName(_shortName)
		{
		}

		const std::string& getShortName() const
		{
			return shortName;
		}

	private:
		std::string shortName;
	};
}