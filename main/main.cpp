/* Smart Chair Embedded Controller
 * Startup Code
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "SCController.h"

extern "C" void app_main()
{
	SCController smartChair;
	smartChair.Start();
}
