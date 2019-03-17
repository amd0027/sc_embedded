/*
 * SCPosture.cpp
 *
 *  Created on: Mar 2, 2019
 *      Author: Chris Taylor
 */

#include "SCPosture.h"

SCPosture::SCPosture() {
	// TODO Auto-generated constructor stub

}

SCPosture::~SCPosture() {
	// TODO Auto-generated destructor stub
}

SCPosture::SCPosture(adc1_channel_t channel1, adc1_channel_t channel2, adc1_channel_t channel3, adc1_channel_t channel4, adc1_channel_t channel5, adc1_channel_t channel6)
{
	s[0] = SCDriverADC(channel1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12);
	s[1] = SCDriverADC(channel2, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12);
	s[2] = SCDriverADC(channel3, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12);
	s[3] = SCDriverADC(channel4, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12);
	s[4] = SCDriverADC(channel5, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12);
	s[5] = SCDriverADC(channel6, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12);
}

int SCPosture::getPosture()
{
	int result = 0;
	uint32_t reading = 0;

	for(int i = 0; i < 6; i++)
	{
		reading = s[i].Sample();
		reading = reading / SCALING_FACTOR;
		// If reading does not fit within 4 bits, error message and set to 0
		if (reading > 16)
		{
			printf("Invalid sensor reading: %d\n", reading);
			reading = 0;
		}
		result = result | reading;
		result = result << 4;
	}
	return result;
}
