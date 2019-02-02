/* Smart Chair Embedded Controller
 * Startup Code
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "SCController.h"
#include "SCDriverADC.h"
#include <stdio.h>
#include <stdlib.h>


// Example code to create SCDriverADC object and sample continuously
void sampleADC1()
{
    SCDriverADC myADC(ADC1_CHANNEL_7);
    myADC.check_efuse();

    //Continuously sample myADC
    uint32_t reading;
    while (1) {
        reading = myADC.Sample();
        printf("Sample Value: %d\n", reading);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

extern "C" void app_main()
{
	printf("Start of main()\n\n");

	//sampleADC1();

	SCController smartChair;
	smartChair.Start();
}
