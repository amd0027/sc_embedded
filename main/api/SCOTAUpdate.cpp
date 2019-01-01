/* Smart Chair Embedded Controller
 * Secure OTA Update Controller
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "SCOTAUpdate.h"

#include <string>
#include <cstring>
#include <sstream>

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <esp_http_client.h>
#include <esp_https_ota.h>

#include <cjson/cJSON.h>

#include "webapi.h"

void SCOTAUpdate::RunUpdateCheck()
{
	char* update_url = GetUpdateUrl();

	if (update_url != NULL)
	{
		PerformOTAUpdate(update_url);
	}

	free(update_url);

	ESP_LOGI(TAG, "Finished with update check");
}

// Checks for the latest version available from the update server
// If currently installed version is up-to-date, NULL is returned
// If an update is available, the URL of the firmware blob is returned
char* SCOTAUpdate::GetUpdateUrl()
{
	char* latest_version_data = webapi::api_get_response(API_GET_URL(WEB_API_UPDATE_CHECK_URL));

	cJSON* response = cJSON_Parse(latest_version_data);
	if (response == NULL)
	{
		ESP_LOGE(TAG, "Could not parse the JSON data");
		return NULL;
	}

	free(latest_version_data);

	cJSON* latest_major_version_json = cJSON_GetObjectItemCaseSensitive(response, "LatestMajorVersion");
	cJSON* latest_minor_version_json = cJSON_GetObjectItemCaseSensitive(response, "LatestMinorVersion");
	cJSON* latest_fw_url_json = cJSON_GetObjectItemCaseSensitive(response, "FirmwareURL");

	if (!cJSON_IsNumber(latest_major_version_json) || !cJSON_IsNumber(latest_minor_version_json))
	{
		ESP_LOGE(TAG, "Error parsing the latest FW revision");

		cJSON_Delete(response);
		return NULL;
	}

	int latest_major_version = latest_major_version_json->valueint;
	int latest_minor_version = latest_minor_version_json->valueint;
	char* latest_fw_url = strdup(latest_fw_url_json->valuestring); // make a copy that can be returned after json_delete is called
	cJSON_Delete(response);

	int current_major_version;
	int current_minor_version;

	const esp_app_desc_t *app_desc = esp_ota_get_app_description();
	std::string sw_version(app_desc->version);
	if (sw_version.find("-") != std::string::npos)
	{
		// this is a development version, where the format is v0.1-XX-abcdef, where abcedef is the commit hash, and XX is the number of commits past the parent
		// compare only the "real" part of the version number and update to only release versions
		sw_version = sw_version.substr(0, sw_version.find("-"));
	}

	std::stringstream current_string(sw_version);
	current_string.ignore(1, '\0'); // skip the 'v' at the beginning of the version number
	current_string >> current_major_version;
	current_string.ignore(1, '\0'); // skip the '.' at the between major and minor rev.
	current_string >> current_minor_version;

	ESP_LOGI(TAG, "Latest FW is Major:%d Minor%d, installed is Major:%d Minor:%d",
			latest_major_version, latest_minor_version,
			current_major_version, current_minor_version);

	if ((latest_major_version > current_major_version) || /*a new major release is available*/
		(latest_major_version == current_major_version && latest_minor_version >  current_minor_version)) /*same major release, but newer minor*/
	{
		ESP_LOGI(TAG, "Update is available");
		return latest_fw_url;
	}
	else
	{
		ESP_LOGI(TAG, "Installed version is latest");
		return NULL;
	}
}

void SCOTAUpdate::PerformOTAUpdate(char* update_url)
{
	esp_http_client_config_t config = { };
	config.url = update_url;
	config.event_handler = webapi::api_http_event_handler;
	config.cert_pem = webapi::API_CERT_PEM_START;

	esp_err_t ret = esp_https_ota(&config);
	if (ret == ESP_OK)
	{
		esp_restart();
	}
	else
	{
		ESP_LOGE(TAG, "Firmware Upgrade Failed");
	}
}
