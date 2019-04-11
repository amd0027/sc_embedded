/*
 * SCAirQuality.cpp
 *
 *  Created on: Apr 10, 2019
 *      Author: Logan Johnson
 */

#include "SCAirQuality.h"

SCAirQuality::SCAirQuality()
{
	// Establish address of Air Quality Sensor
	i2c.setAddress(0x5b);

	i2c.writeReg(0x5b, 0xf4, 0xf4);	// change mode of AQS from boot to running
	i2c.writeReg(0x5b, 0x01, 0x10);	// Air Quality Sensor: change data mode from idle (default) to update data every second
}

SCAirQuality::~SCAirQuality() {
	// TODO Auto-generated destructor stub
}

SCAirRawData SCAirQuality::SampleAQS()
{
	uint8_t *buffer = (uint8_t*)malloc(4);

	i2c.beginTransaction();
	i2c.master_read_AQS(buffer, 4, 0x02);
	i2c.endTransaction();

	SCAirRawData result;
	result.CO2 = (buffer[0] << 8) | buffer[1];
	result.VOC = (buffer[2] << 8) | buffer[3];

	free(buffer);
	return result;
}


void SCAirQuality::debugInfo()
{
	uint8_t *buffer = (uint8_t*)malloc(4);
	uint8_t *single = (uint8_t*)malloc(1);
	//uint8_t *temp = (uint8_t*)malloc(1);
	int16_t CO2, VOC;

	i2c.beginTransaction();
	i2c.master_read_AQS(single, 1, 0x20);
	i2c.endTransaction();
	printf("HWID:   %d\n", *single);

	i2c.beginTransaction();
	i2c.master_read_AQS(single, 1, 0x00);
	i2c.endTransaction();
	printf("STATUS:   %d\n", *single);

	// ERROR REGISTER CHECK
	i2c.beginTransaction();
	i2c.master_read_AQS(single, 1, 0xe0);
	i2c.endTransaction();
	printf("ERROR:  %d\n", *single);

	// RESULT REGISTER CHECK
	i2c.beginTransaction();
	i2c.master_read_AQS(buffer, 4, 0x02);
	i2c.endTransaction();

	CO2 = (buffer[0] << 8) | buffer[1];
	VOC = (buffer[2] << 8) | buffer[3];

	printf("AQS: %7d CO2 ppm %7d VOC ppb\n\n", CO2, VOC);

	//vTaskDelay(1000 / portTICK_PERIOD_MS);
	free(single);
	free(buffer);
}
