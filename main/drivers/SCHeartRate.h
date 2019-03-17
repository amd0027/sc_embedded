/*
 * SCHeartRate.h
 *
 *  Created on: Feb 20, 2019
 *      Author: Chris Taylor
 */

#ifndef MAIN_SCHEARTRATE_H_
#define MAIN_SCHEARTRATE_H_

#include "SCDriverADC.h"

#define THRESHOLD_OFFSET 100

class SCHeartRate: public SCDriverADC {
public:
	SCHeartRate(adc1_channel_t channel, adc_atten_t atten=ADC_ATTEN_DB_11, adc_bits_width_t bits=ADC_WIDTH_BIT_12, int rate=50, int period=5);

	int getHeartRate();
	int getThreshold();

private:
	// Time (ms) between each sample (default = 50 ms)
	int sample_rate;
	// Time (s) period for all samples for a heartrate calculation (default = 5s)
	int sample_period;
	int threshold;
};

#endif /* MAIN_SCHEARTRATE_H_ */
