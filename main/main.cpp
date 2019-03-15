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
	//size_t d_size;

	printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
	printf("00:         ");
	for (uint8_t i = 3; i < 0x78; i++)
	{
		if (i % 16 == 0) {
			printf("\n%.2x:", i);
		}
		if (i2c.checkSlaveAddress(i)) {
			printf(" %.2x", i);
			//i2c.setAddress(i);
		} else {
			printf(" --");
		}
	}
	printf("\n\n");

	i2c.setAddress(0x6b); // 0x6b is address of accelerometer

/*	for (int k = 0; k < 200; k++)
	{
		i2c.i2c_master_sensor_test(I2C_NUM_0, i2c.data_high, i2c.data_low);
		printf("%d %d\n", i2c.data_high[0], i2c.data_low[0]);
	}*/

	// the following code actually sets the Output data rate. Names deceiving.

	i2c.beginTransaction();
	i2c.setContinuousMode();
	i2c.endTransaction();


	// the following loop is supposed to read "buffer_size" bytes of data from slave "k" times.
	for(int k = 0; k < 200; k++)
	{
		sleep(1);
		i2c.beginTransaction();

		i2c.master_read_slave(i2c.data_rd, i2c.buffer_size);
		//TODO: How does program respond when print is before or after endTransaction() ?
		int16_t temp = (i2c.data_rd[1] << 8) | i2c.data_rd[0];
		printf("%d\n", temp);

		i2c.endTransaction();

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
