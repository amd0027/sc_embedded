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

extern "C" void app_main()
{
	SCController smartChair;
	smartChair.Start();
}
