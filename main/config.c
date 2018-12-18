/* Smart Chair Embedded Controller
 * Configuration Structure and NVS Interface
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "config.h"

#include <stdbool.h>

#include <esp_system.h>
#include <esp_spi_flash.h>
#include <esp_log.h>
#include <esp_err.h>
#include <nvs_flash.h>

static const char* TAG = "config";

sc_config_t* config;

esp_err_t config_init_flash()
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

sc_config_t* config_get_handle()
{
	return config;
}

bool config_load()
{
	esp_err_t err;
	err = config_init_flash();

	ESP_LOGI(TAG, "Opening NVS to load config. data");
	nvs_handle nvs;
	err = nvs_open("storage", NVS_READWRITE, &nvs);

	config = (sc_config_t*)malloc(sizeof(sc_config_t));
	if (config == NULL)
	{
		ESP_LOGE(TAG, "Could not allocate space for config structure");
		return false;
	}

	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error opening NVS: (%s)", esp_err_to_name(err));

		return false;
	}
	else
	{
		ESP_LOGI(TAG, "Reading values from NVS");
		size_t required_size;

		// Read the stored Wifi SSID
		err = nvs_get_str(nvs, "wifi_ssid", NULL, &required_size);
		//config->wifi_ssid = malloc(required_size);
		nvs_get_str(nvs, "wifi_ssid", config->wifi_ssid, &required_size);

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
		//config->wifi_pass = malloc(required_size);
		nvs_get_str(nvs, "wifi_pass", config->wifi_pass, &required_size);
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

		return true;
	}
}

bool config_save()
{
	esp_err_t err;
	err = config_init_flash();

	ESP_LOGI(TAG, "Opening NVS to load config. data");
	nvs_handle nvs;
	err = nvs_open("storage", NVS_READWRITE, &nvs);

	if (config == NULL)
	{
		ESP_LOGE(TAG, "Config Data is NULL");
		return false;
	}

	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error opening NVS: (%s)", esp_err_to_name(err));

		return false;
	}
	else
	{
		bool success = true;

		ESP_LOGI(TAG, "Reading values from NVS");

		// Write the Wifi SSID
		err = nvs_set_str(nvs, "wifi_ssid", config->wifi_ssid);
		if (err != ESP_OK)
		{
			ESP_LOGE(TAG, "Failed to write SSID to NVS");
			success = false;
		}

		// Write the Wifi SSID
		err = nvs_set_str(nvs, "wifi_pass", config->wifi_pass);
		if (err != ESP_OK)
		{
			ESP_LOGE(TAG, "Failed to write Wifi Password to NVS");
			success = false;
		}

		return success;
	}
}
