/*
 * SCDataClient.h
 *
 *  Created on: Feb 7, 2019
 *      Author: Alex Dillon
 */

#ifndef SCDATACLIENT_H_
#define SCDATACLIENT_H_

#include <string>

#include "../SCSettings.h"
#include "SensorDataModels.h"

class SCDataClient {
public:
	SCDataClient(SCSettings& settings);
	bool PostHeartRateData(HeartRateSensorModel data);
	bool PostPostureData(PostureSensorModel data);
	bool PostMotionData(MotionEventModel data);
	bool PostOccupancyData(OccupancySessionModel data);

private:
	static const std::string DATETIME_MIN_VALUE;
	static const constexpr char* TAG = "SCDataClient";
	SCSettings& settings;
};

#endif /* SCDATACLIENT_H_ */
