/*
 * SCAirQuality.h
 *
 *  Created on: Apr 10, 2019
 *      Author: Logan Johnson
 */

#ifndef MAIN_DRIVERS_SCAIRQUALITY_H_
#define MAIN_DRIVERS_SCAIRQUALITY_H_

#include "SCDriverI2C.h"

struct SCAirRawData
{
	int CO2;
	int VOC;
};

class SCAirQuality {
public:
	SCAirQuality();
	virtual ~SCAirQuality();

	SCAirRawData SampleAQS();

	//TEMP FUNCTIONS
	void debugInfo();

private:
	SCDriverI2C i2c = SCDriverI2C(0);
};

#endif /* MAIN_DRIVERS_SCAIRQUALITY_H_ */
