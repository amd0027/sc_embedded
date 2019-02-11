/*
 * SCDataClient.h
 *
 *  Created on: Feb 7, 2019
 *      Author: Alex Dillon
 */

#ifndef SCDATACLIENT_H_
#define SCDATACLIENT_H_

#include <string>

#include "SensorDataModels.h"

class SCDataClient {
public:
	SCDataClient(std::string key);
	bool PostHeartRateData(HeartRateSensorModel data);
	bool PostPostureData(PostureSensorModel data);
	bool PostMotionData(MotionEventModel data);
	bool PostOccupancyData(OccupancySessionModel data);

private:
	static const std::string DATETIME_MIN_VALUE;
	static const constexpr char* TAG = "SCDataClient";
	std::string authKey;
};

#endif /* SCDATACLIENT_H_ */
