/* Smart Chair Embedded Controller
 * Configuration Structure and NVS Interface
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "SCSettings.h"

#include <cstddef>

#include <esp_system.h>
#include <esp_spi_flash.h>
#include <esp_log.h>
#include <esp_err.h>
#include <nvs_flash.h>

static const char* TAG = "config";

SCSettings::SCSettings():
		wifi_ssid(),
		wifi_pass(),
		auth_key()
{
	InitFlash();
}

bool SCSettings::Load()
{
	if (nvsStatus != ESP_OK)
	{
		return false;
	}
	else
	{
		ESP_LOGI(TAG, "Reading values from NVS");
		bool success = true;

		success |= LoadSetting("wifi_ssid", this->wifi_ssid);
		success |= LoadSetting("wifi_pass", this->wifi_pass);
		success |= LoadSetting("auth_key", this->auth_key);

		return true;
	}
}

bool SCSettings::LoadSetting(const char* keyName, std::string& result)
{
	size_t bufferSize = 100;
	char buffer[bufferSize];

	esp_err_t err = nvs_get_str(nvs, keyName, buffer, &bufferSize);
	switch (err) {
		case ESP_OK:
			ESP_LOGI(TAG, "%s read from NVS OK", keyName);
			result = std::string(buffer);
			return true;
		case ESP_ERR_NVS_NOT_FOUND:
			ESP_LOGI(TAG, "%s not found in NVS", keyName);
			return false;
		default:
			ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
			return false;;
	}
}

bool SCSettings::Save()
{
	if (nvsStatus != ESP_OK) {
		return false;
	}
	else
	{
		esp_err_t err;
		bool success = true;

		ESP_LOGI(TAG, "Writing values to NVS");

		// Write the Wifi SSID
		err = nvs_set_str(nvs, "wifi_ssid", wifi_ssid.c_str());
		if (err != ESP_OK)
		{
			ESP_LOGE(TAG, "Failed to write SSID to NVS");
			success = false;
		}

		// Write the Wifi SSID
		err = nvs_set_str(nvs, "wifi_pass", wifi_pass.c_str());
		if (err != ESP_OK)
		{
			ESP_LOGE(TAG, "Failed to write Wifi Password to NVS");
			success = false;
		}

		// Write the AuthKey
		err = nvs_set_str(nvs, "auth_key", auth_key.c_str());
		if (err != ESP_OK)
		{
			ESP_LOGE(TAG, "Failed to write auth_key to NVS");
			success = false;
		}

		return success;
	}
}


void SCSettings::InitFlash()
{
	//Initialize NVS
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);

	ESP_LOGI(TAG, "Opening NVS for configuration data");

	err = nvs_open("storage", NVS_READWRITE, &nvs);

	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error opening NVS: (%s)", esp_err_to_name(err));

	}

	nvsStatus = err;
}
