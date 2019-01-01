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

esp_err_t SCSettings::InitFlash()
{
	//Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	return ret;
}

bool SCSettings::Load()
{
	esp_err_t err;
	err = InitFlash();

	ESP_LOGI(TAG, "Opening NVS to load config. data");
	nvs_handle nvs;
	err = nvs_open("storage", NVS_READWRITE, &nvs);

	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error opening NVS: (%s)", esp_err_to_name(err));

		return false;
	}
	else
	{
		ESP_LOGI(TAG, "Reading values from NVS");
		std::size_t required_size;

		// Read the stored Wifi SSID
		err = nvs_get_str(nvs, "wifi_ssid", NULL, &required_size);
		nvs_get_str(nvs, "wifi_ssid", wifi_ssid, &required_size);

		switch (err) {
			case ESP_OK:
				ESP_LOGD(TAG, "SSID read from NVS OK");
				break;
			case ESP_ERR_NVS_NOT_FOUND:
				ESP_LOGW(TAG, "SSID not found in NVS");
				return false;
			default:
				ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
				return false;
		}

		// Read the stored Wifi Password
		nvs_get_str(nvs, "wifi_pass", NULL, &required_size);
		nvs_get_str(nvs, "wifi_pass", wifi_pass, &required_size);
		switch (err) {
			case ESP_OK:
				ESP_LOGD(TAG, "Password read from NVS OK");
				break;
			case ESP_ERR_NVS_NOT_FOUND:
				ESP_LOGW(TAG, "Password not found in NVS");
				return false;
			default:
				ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
				return false;
		}

		// Read the stored Authorization Key
		nvs_get_str(nvs, "auth_key", NULL, &required_size);
		nvs_get_str(nvs, "auth_key", auth_key, &required_size);
		switch (err) {
			case ESP_OK:
				ESP_LOGD(TAG, "Auth_Key read from NVS OK");
				break;
			case ESP_ERR_NVS_NOT_FOUND:
				ESP_LOGW(TAG, "Auth_Key not found in NVS");
				return false;
			default:
				ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
				return false;
		}

		return true;
	}
}

bool SCSettings::Save()
{
	esp_err_t err;
	err = InitFlash();

	ESP_LOGI(TAG, "Opening NVS to load config. data");
	nvs_handle nvs;
	err = nvs_open("storage", NVS_READWRITE, &nvs);

	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error opening NVS: (%s)", esp_err_to_name(err));

		return false;
	}
	else
	{
		bool success = true;

		ESP_LOGI(TAG, "Writing values to NVS");

		// Write the Wifi SSID
		err = nvs_set_str(nvs, "wifi_ssid", wifi_ssid);
		if (err != ESP_OK)
		{
			ESP_LOGE(TAG, "Failed to write SSID to NVS");
			success = false;
		}

		// Write the Wifi SSID
		err = nvs_set_str(nvs, "wifi_pass", wifi_pass);
		if (err != ESP_OK)
		{
			ESP_LOGE(TAG, "Failed to write Wifi Password to NVS");
			success = false;
		}

		// Write the AuthKey
		err = nvs_set_str(nvs, "auth_key", auth_key);
		if (err != ESP_OK)
		{
			ESP_LOGE(TAG, "Failed to write auth_key to NVS");
			success = false;
		}

		return success;
	}
}
