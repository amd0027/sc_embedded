/* Smart Chair Embedded Controller
 * Configuration Structure and NVS Interface Header File
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef SCSETTINGS_H
#define SCSETTINGS_H

#include <string>

#include <nvs_flash.h>
#include <esp_err.h>

class SCSettings
{
public:
	std::string wifi_ssid;
	std::string wifi_pass;

	std::string auth_key;
	//TODO: more items and stuff

	SCSettings();
	bool Load();
	bool LoadSetting(const char* keyName, std::string& result);
	bool Save();

private:
	nvs_handle nvs;
	esp_err_t nvsStatus;

	void InitFlash();

};

#endif /*SCSETTINGS_H*/
