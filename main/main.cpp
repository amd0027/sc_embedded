/* Smart Chair Embedded Controller
 * Startup Code
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "SCController.h"
#include "SCDriverADC.h"
#include "SCHeartRate.h"
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

extern "C" void app_main()
{
	printf("Start of main()\n\n");

	//sampleADC1();

	SCController smartChair;
	smartChair.Start();
}
