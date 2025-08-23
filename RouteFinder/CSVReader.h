#pragma once

#include <tuple>
#include <vector>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <ctime>
#include <cassert>

namespace rf
{
	class CSVReaderBase
	{
	public:
		CSVReaderBase(char delim)
			: delimiter(delim)
		{
		}

		void setTimeFormat(std::string_view str)
		{
			timeFormat = str;
		}

	protected:
		std::string timeFormat = "%T";
		char delimiter;

		template< typename T > void readElement(std::istream& stream, T& value)
		{
			stream >> value;
		}

		template<> void readElement(std::istream& stream, std::string& value)
		{
			bool quotation = false;
			value.reserve(16);
			while (true)
			{
				char ch = stream.peek();

				if (quotation)
				{
					if (ch == '"')
					{
						quotation = false;
					}
				}
				else
				{
					if (ch == '"')
					{
						quotation = true;
					}
					else if (ch == delimiter || ch == '\n' || ch == EOF)
					{
						break;
					}
				}

				ch = stream.get();
				if (ch != '"')
				{
					value += ch;
				}
			}
		}

		template<> void readElement(std::istream& stream, std::tm& value)
		{
			stream >> std::get_time(&value, timeFormat.c_str());
		}

		template<> void readElement(std::istream& stream, std::_Ignore&)
		{
			bool quotation = false;
			while (true)
			{
				char ch = stream.peek();
				if (quotation)
				{
					if (ch == '"')
					{
						quotation = false;
					}
				}
				else
				{
					if (ch == '"')
					{
						quotation = true;
					}
					else if (ch == delimiter || ch == '\n' || ch == EOF)
					{
						break;
					}
				}
				
				stream.get();
			}
		}

		void readDelimiter(std::istream& stream)
		{
			char ch = stream.get();
			assert(ch == delimiter || ch == '\n' || ch == EOF);
		}
	};

	template< typename Parser, typename... Ts > class CSVReader : public CSVReaderBase
	{
	public:
		CSVReader(Parser& p, char delimiter = ',', bool _hasHeader = true)
			: CSVReaderBase(delimiter)
			, parser(p)
			, hasHeader(_hasHeader)
		{
		}

		void read(std::istream& stream)
		{
			if (hasHeader)
			{
				readHeader(stream);
			}


			while (stream.peek() != EOF)
			{
				readRow(stream);
			}

			parser.finalize();
		}

	private:
		static constexpr size_t ElementCount = sizeof...(Ts);

		void readHeader(std::istream& stream)
		{
			std::string header[ElementCount];
			for (size_t i = 0; i < ElementCount; ++i)
			{
				readElementAndDelimiter(stream, header[i]);
			}

			//parser.header( header );
		}

		template< typename T > void readElementAndDelimiter(std::istream& stream, T& value)
		{
			readElement(stream, value);
			readDelimiter(stream);
		}

		void readRow(std::istream& stream)
		{
			std::tuple< Ts... > ts;
			std::apply([&](auto&... args)
				{
					(readElementAndDelimiter(stream, args), ...);
				}, ts);

			std::apply([&](auto&... args)
				{
					parser.row(args...);
				}, ts);
		}

		bool hasHeader;
		Parser& parser;
	};
}