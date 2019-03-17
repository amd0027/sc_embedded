/*
 * SCDriverADC.cpp
 *
 *  Created on: Feb 2, 2019
 *      Author: Chris Taylor
 */

#include "SCDriverADC.h"

// Main constructor: configures desired adc channel for inputs of channel, attenuation, and data bits.
// see /home/esp/esp-idf/components/driver/include/driver/adc.h for type declarations
SCDriverADC::SCDriverADC(adc1_channel_t setchannel, adc_atten_t setatten, adc_bits_width_t setbits) {
	// Store values in private variables
	unit = ADC_UNIT_1;
	channel = setchannel;
	atten = setatten;
	bits = setbits;
	// Configure adc
	adc1_config_width(bits);
	adc1_config_channel_atten(channel, atten);

	// Characterize adc (val_type) is for debugging purposes
	adc_chars = (esp_adc_cal_characteristics_t*) calloc(1, sizeof(esp_adc_cal_characteristics_t));
	val_type = esp_adc_cal_characterize(unit, atten, bits, DEFAULT_VREF, adc_chars);
}

// Use multisampling defined for all sensors to get average 64 readings quickly and return raw value
uint32_t SCDriverADC::Sample() {
	 uint32_t adc_reading = 0;
	for (int i = 0; i < NO_OF_SAMPLES; i++) {
		adc_reading += adc1_get_raw(channel);
	}
	return adc_reading / NO_OF_SAMPLES;
}

// If desired, can change a raw reading to voltage - in testing this value seemed less reliable
uint32_t SCDriverADC::raw_to_voltage(uint32_t reading) {
	return esp_adc_cal_raw_to_voltage(reading, adc_chars);
}

// Used to verify what val_types are supported on the board - ported from sample code for debug purposes
void SCDriverADC::check_efuse()
{
    //Check TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
}
