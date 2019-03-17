/*
 * SCPosture.h
 *
 *  Created on: Mar 2, 2019
 *      Author: Chris Taylor
 */

#ifndef MAIN_SCPOSTURE_H_
#define MAIN_SCPOSTURE_H_

#include "SCDriverADC.h"

// Scaling factor = 256 (2^8) to reduce 12 bits to 4 bits
#define SCALING_FACTOR 256

class SCPosture {
public:
	SCPosture(adc1_channel_t channel1, adc1_channel_t channel2, adc1_channel_t channel3, \
			adc1_channel_t channel4, adc1_channel_t channel5, adc1_channel_t channel6);

	int getPosture();

private:
	SCDriverADC s[6];
};

#endif /* MAIN_SCPOSTURE_H_ */
