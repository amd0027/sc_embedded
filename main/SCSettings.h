/* Smart Chair Embedded Controller
 * Configuration Structure and NVS Interface Header File
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef SCSETTINGS_H
#define SCSETTINGS_H

#include <string>

#include <esp_err.h>

class SCSettings
{
public:
	char wifi_ssid[32];
	char wifi_pass[64];

	char auth_key[37];
	//TODO: more items and stuff

	bool Load();
	bool Save();

private:
	esp_err_t InitFlash();
};

#endif /*SCSETTINGS_H*/
