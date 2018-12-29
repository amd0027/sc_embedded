/* Smart Chair Embedded Controller
 * Configuration Structure and NVS Interface Header File
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct sc_config_t sc_config_t;
struct sc_config_t
{
	char wifi_ssid[32];
	char wifi_pass[64];

	char auth_key[37];
	//TODO: more items and stuff
};

sc_config_t* config_get_handle();
bool config_load();
bool config_save();

#endif
