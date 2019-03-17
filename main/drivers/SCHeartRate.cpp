/*
 * SCHeartRate.cpp
 *
 *  Created on: Feb 20, 2019
 *      Author: Chris Taylor
 */

#include "SCHeartRate.h"

SCHeartRate::SCHeartRate() {
	// Auto-generated constructor stub

}

SCHeartRate::~SCHeartRate() {
	// Auto-generated destructor stub
}

SCHeartRate::SCHeartRate(adc1_channel_t channel, adc_atten_t atten, adc_bits_width_t bits, int rate, int period)
	: SCDriverADC(channel, atten, bits)
{
	// Let SCDriverADC Constructor do all the work. Store customizeable samplerate and sample period
	sample_rate = rate;
	sample_period = period;
	threshold = 0;
}

int SCHeartRate::getThreshold()
{
	int count = sample_period*1000/sample_rate;
	uint32_t reading;
	long int sum = 0;
	for (int i = 0; i < count; i++)
	{
		reading = this->Sample();
		sum += reading;

		vTaskDelay(pdMS_TO_TICKS(sample_rate));
	}
	threshold = sum/count + THRESHOLD_OFFSET;
	return threshold;
}

int SCHeartRate::getHeartRate()
{
	int count = sample_period*1000/sample_rate;
	uint32_t reading;
	bool IsHeartbeat = false;
	int beats = 0;
	int lastbeat = 0;
	int average_period = 0;
	for (int i = 0; i < count; i++)
	{
		reading = this->Sample();
		// If pass threshold for first time, add beat
		if (reading > threshold && !IsHeartbeat)
		{
			beats++;
			IsHeartbeat = true;
			// First beat - track time. Following beats, add difference between current time and last to average
			if (beats == 1)
				lastbeat = i;
			else
			{
				average_period = average_period + (i - lastbeat);
				lastbeat = i;
			}
		}
		else if (reading < threshold && IsHeartbeat)
			IsHeartbeat = false;

		vTaskDelay(pdMS_TO_TICKS(sample_rate));
	}
	// Calculate average by dividing by number of intermediate beats
	average_period = average_period / (beats - 1);
	/*
	 *  Calculation Explained for return value:
	 *  1 beat * 1000 ms  * 60 s  => 6000/AVG -> BPM
	 *  AVG ms      1 s      1 m
	 */
	//printf("Basic rate: %d", beats*(60/sample_period));
	return 6000/(average_period);
	//return beats*(60/sample_period);
}
