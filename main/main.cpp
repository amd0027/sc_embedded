/* Smart Chair Embedded Controller
 * Startup Code
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "SCController.h"
#include <cstdlib>

#include "drivers/SCDriverADC.h"
#include "drivers/SCDriverI2C.h"
#include "drivers/SCHeartRate.h"
#include "drivers/SCMotion.h"
#include "drivers/SCAirQuality.h"

#include <stdio.h>
#include <stdlib.h>

// Example code to create SCDriverADC object and sample continuously
void sampleADC1()
{
    SCHeartRate HRSensor(ADC1_CHANNEL_6, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 10, 10);

    //Continuously sample myADC
    int heartrate;
    int temp;
    temp = HRSensor.getThreshold();
    printf("Threshold: %d\n\n", temp);
    while (1) {
    	heartrate = HRSensor.getHeartRate();
    	printf("Heartrate: %d\n", heartrate);
    }
}

void sampleI2C()
{
	SCMotion motion;
	SCAirQuality air_quality;

	SCMotionRawData XL_data;
	SCAirRawData AQS_data;

	// continuously sample from slave
	while(1)
	{
		XL_data = motion.Sample();
		AQS_data = air_quality.Sample();

		printf("XL:  %7d x %7d y %7d z\n", XL_data.x, XL_data.y, XL_data.z);
		printf("AQS: %7d c %7d v\n", AQS_data.CO2, AQS_data.VOC);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

extern "C" void app_main()
{
	SCController smartChair;
	smartChair.Start();
}
