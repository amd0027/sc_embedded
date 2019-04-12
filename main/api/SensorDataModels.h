#include <string>
#include <cstdint>

#ifndef SENSORDATAMODELS_H_
#define SENSORDATAMODELS_H_

class HeartRateSensorModel
{
public:
	std::string Timestamp;
	int MeasuredBPM;
};

class PostureSensorModel
{
public:
	std::string Timestamp;
	int PostureData;
};

class OccupancySessionModel
{
public:
	std::string Timestamp;
	std::string SitDownTime;
	uint32_t ElapsedTimeSeconds;
};

class MotionEventModel
{
public:
	std::string Timestamp;

	enum MotionAxis
	{
		X = 1,
		Y = 2,
		Z = 4,
		RotateSideSide  = 8,
		RotateFrontBack = 16
	};

	uint16_t Axis;
	uint16_t Level;
};

class AirQualityModel
{
public:
	std::string Timestamp;

	int CO2;
	int VOC;
};

#endif /*SENSORDATAMODELS_H_*/
