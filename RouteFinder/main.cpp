// RouteFinder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "CSVReader.h"
#include "RouteFinder.h"

#include <iostream>
#include <fstream>
#include <filesystem>

using TimeStopsReader = rf::CSVReader< rf::RouteBuilder, std::string, std::tm, std::tm, uint32_t, uint32_t, std::_Ignore, bool, bool, float, bool >;
using RoutesReader = rf::CSVReader< rf::RouteBuilder, std::string, std::_Ignore, std::string, std::_Ignore, std::_Ignore, std::_Ignore, std::_Ignore >;
using StopsReader = rf::CSVReader< rf::RouteBuilder, uint32_t, std::string, std::string, std::_Ignore, float, float, std::_Ignore, std::_Ignore, std::_Ignore, std::_Ignore, std::_Ignore, std::_Ignore, std::_Ignore >;

static constexpr size_t ReadBufferSize = 20 * 1024 * 1024;

std::ifstream getFile(const char* filename, char* pBuffer)
{
	std::ifstream file(filename, std::ifstream::in);
	file.rdbuf()->pubsetbuf(pBuffer, ReadBufferSize);

	return file;
}

int main(int argc, char* argv[])
{
	std::ios_base::sync_with_stdio(false);

	std::cout << "Working directory: " << argv[0] << std::endl;

	static char* pBuffer = (char*)malloc(ReadBufferSize);

	const char* StopTimesFilename = "C:\\Users\\laria\\source\\repos\\RouteFinder\\stop_times.txt";
	std::ifstream stopTimesFile = getFile( StopTimesFilename, pBuffer );
	if (!stopTimesFile)
	{
		std::cerr << "File not accessible" << std::endl;
		return 1;
	}

	const char* RoutesFilename = "C:\\Users\\laria\\source\\repos\\RouteFinder\\routes.txt";
	std::ifstream routesFile = getFile(RoutesFilename, pBuffer );
	if (!routesFile)
	{
		std::cerr << "File not accessible" << std::endl;
		return 1;
	}

	const char* StopsFilename = "C:\\Users\\laria\\source\\repos\\RouteFinder\\stops.txt";
	std::ifstream stopsFile = getFile(StopsFilename, pBuffer);
	if (!stopsFile)
	{
		std::cerr << "File not accessible" << std::endl;
		return 1;
	}

	rf::BuildSettings settings;
	settings.maxTransferDistance = 1000.f;
	rf::RouteContainer container;
	rf::RouteBuilder builder(container, settings);

	TimeStopsReader timeStopsReader(builder);
	timeStopsReader.read(stopTimesFile);

	RoutesReader routesReader(builder);
	routesReader.read(routesFile);

	StopsReader stopsReader(builder);
	stopsReader.read(stopsFile);

	stopTimesFile.close();
	routesFile.close();
	stopsFile.close();

	rf::RouteFinder<> finder(container);

	std::string strStartCode, stdEndCode;
	uint32_t startCode, endCode;
	std::optional<uint32_t> stopID;
	while (true)
	{
		start:
		std::cout << "Start stop:";
		std::cin >> strStartCode;
		stopID = container.findStopID(strStartCode);
		if (!stopID)
		{
			std::cout << "Stop not found" << std::endl;
			goto start;
		}
		startCode = stopID.value();

		end:
		std::cout << "End stop:";
		std::cin >> strStartCode;
		stopID = container.findStopID(strStartCode);
		if (!stopID)
		{
			std::cout << "Stop not found" << std::endl;
			goto end;
		}
		endCode = stopID.value();

		rf::RouteLog log = finder.search(std::time(0), startCode, endCode);
		log.print(std::cout, container);
		std::cout << std::endl;
	}
	

	free(pBuffer);

	return 0;
}