/* Smart Chair Embedded Controller
 * WiFi Controller Header
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef WIFI_H
#define WIFI_H

#include <stdbool.h>

void connect_sta_wifi(bool delay_init);
void connect_softap_wifi(bool delay_init);

#endif /*WIFI_H*/
