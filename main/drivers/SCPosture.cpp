/*
 * SCPosture.cpp
 *
 *  Created on: Mar 2, 2019
 *      Author: Chris Taylor
 */

#include "SCPosture.h"

SCPosture::SCPosture(adc1_channel_t channel1, adc1_channel_t channel2, adc1_channel_t channel3, adc1_channel_t channel4, adc1_channel_t channel5, adc1_channel_t channel6)
	:s { SCDriverADC(channel1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12),
	SCDriverADC(channel2, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12),
	SCDriverADC(channel3, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12),
	SCDriverADC(channel4, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12),
	SCDriverADC(channel5, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12),
	SCDriverADC(channel6, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12)}
{
}

int SCPosture::getPosture()
{
	int result = 0;
	uint32_t reading = 0;

	for(int i = 0; i < 6; i++)
	{
		reading = s[i].Sample();
		reading = reading / SCALING_FACTOR;
		reading = 0xF - reading; // inverted for Active-Low arrangement on chair

		// If reading does not fit within 4 bits, error message and set to 0
		if (reading > 16)
		{
			printf("Invalid sensor reading: %d\n", reading);
			reading = 0;
		}
		printf("Sensor %d: %d\n", i, reading);
		result = result | reading;
		result = result << 4;
	}
	return result;
}

/*static*/ int SCPosture::AveragePostureSamples(int sample1, int sample2)
{
	int result = 0;
	for (int i = 0; i < 6; i++)
	{
		int val1 = sample1 & 0xF;
		int val2 = sample2 & 0xF;
		int valAvg = (val1 + val2) / 2;

		sample1 = sample1 >> 4;
		sample2 = sample2 >> 4;

		result = result | (valAvg << (4 * i));
	}

	return result;
}

/*static*/ bool SCPosture::PredictOccupied(int sample)
{
	bool result = true;
	for (int i = 0; i < 6; i++)
	{
		int value = sample & 0xF;

		result &= (value != 0);
	}

	return result;
}
