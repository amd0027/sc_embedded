/* Smart Chair Embedded Controller
 * Startup Code
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "SCController.h"
#include "SCDriverADC.h"
#include "SCHeartRate.h"
#include "SCDriverI2C.h"
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
	SCDriverI2C i2c;

	printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
	printf("00:         ");
	for (uint8_t i = 3; i < 0x78; i++) {
		if (i % 16 == 0) {
			printf("\n%.2x:", i);
		}
		if (i2c.checkSlaveAddress(i)) {
			printf(" %.2x", i);
		} else {
			printf(" --");
		}
	}
	printf("\n\n");

	size_t bufsize = 4;
	uint8_t* buf = (uint8_t*)malloc(bufsize);

	i2c.setAddress(0x6b);
	for (int i = 0; i<5; i++)
	{
		sleep(1);
		i2c.beginTransaction();
		i2c.read(buf, bufsize, true);
		i2c.endTransaction();
		for (int j=0; j < bufsize; j++)
			printf("%.2x\t", buf[j]);
		printf("\n");
	}
}

extern "C" void app_main()
{
	printf("Start of main()\n\n");

	//sampleADC1();
	sampleI2C();

	//SCController smartChair;
	//smartChair.Start();
}
