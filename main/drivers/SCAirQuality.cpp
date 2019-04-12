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

	// Used to set Air Quality sensor to running mode. A contentless write into the APP_START register (0xf4)
	i2c.startAQSApplication();
	// Change data mode from idle (default) to update data every 60 seconds in the MEAS_MODE register (0x01)
	i2c.writeReg(0x5b, 0x01, 0x30);
}

SCAirQuality::~SCAirQuality() {
	// TODO Auto-generated destructor stub
}

SCAirRawData SCAirQuality::Sample()
{
	uint8_t *buffer = (uint8_t*)malloc(4);

	// Read 4 bytes from ALG_RESULT_DATA register (0x02)
	i2c.beginTransaction();
	i2c.master_read_AQS(buffer, 4, 0x02);
	i2c.endTransaction();

	SCAirRawData result;
	result.CO2 = (buffer[0] << 8) | buffer[1];
	result.VOC = (buffer[2] << 8) | buffer[3];

	free(buffer);
	return result;
}

// Debug info can be deprecated but useful for verifying status and error codes during execution
void SCAirQuality::debugInfo()
{
	uint8_t *buffer = (uint8_t*)malloc(4);
	uint8_t *single = (uint8_t*)malloc(1);
	//uint8_t *temp = (uint8_t*)malloc(1);
	int16_t CO2, VOC;

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
