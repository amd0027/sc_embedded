/*
 * SCDriverADC.h
 *
 *  Created on: Feb 2, 2019
 *      Author: Chris Taylor
 *
 *      Purpose: To create object for any given channel and channel characteristics that corresponds to sensors connected to our board.
 *      Tested and verified with pressure sensor using 100(ohm) voltage divider circuit into ADC1 channels
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#ifndef SCDRIVERADC_H_
#define SCDRIVERADC_H_

#define DEFAULT_VREF    3300        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

class SCDriverADC {
public:
	SCDriverADC(adc1_channel_t channel, adc_atten_t atten=ADC_ATTEN_DB_0, adc_bits_width_t bits=ADC_WIDTH_BIT_12);

	uint32_t Sample();
	uint32_t raw_to_voltage(uint32_t reading);
	void check_efuse();

private:
	esp_adc_cal_characteristics_t *adc_chars;	// Stores configured adc values
	adc1_channel_t channel; 						// Ex) ADC1_CHANNEL_7;
	adc_atten_t atten;							// Ex) ADC_ATTEN_DB_0
	adc_bits_width_t bits;						// Ex) ADC_WIDTH_BIT_12
	adc_unit_t unit;								// Defined in constructor as ADC_UNIT_1 since we only use adc1
	esp_adc_cal_value_t val_type;						// Defines how values are stored (2-point, eRef, or default Vref)
};

#endif /* SCDRIVERADC_H_ */
