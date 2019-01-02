/* Smart Chair Embedded Controller
 * Pairing Controller Header
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "SCPairing.h"

#include <string>
#include <cstring>

#include <esp_system.h>
#include <esp_log.h>

#include <cjson/cjson.h>

bool SCPairing::BeginPairing()
{
	std::string url = std::string(API_GET_URL(WEB_API_GET_PAIRING_CODE));
	std::string uuid = webapi::APICalculateUUID();
	url += uuid;

	char* pairingCodeResponse = webapi::APIGetResponse(url.c_str());

	// since this is just a single string, we don't need to use cJSON to parse it
	std::string pairingCodeStr(pairingCodeResponse);
	if (pairingCodeStr.size() == 8)
	{
		pairingCodeStr = pairingCodeStr.substr(1, 6);
	}
	else
	{
		ESP_LOGE(TAG, "Error decoding the Pairing Code Response");
		return false;
	}

	this->pairingKey = pairingCodeStr;
	ESP_LOGI(TAG, "Got pairing code: %s", pairingCodeStr.c_str());
	return true;
}

bool SCPairing::PollPairingStatus()
{
	std::string url = std::string(API_GET_URL(WEB_API_GET_PAIRING_STATUS));
	std::string uuid = webapi::APICalculateUUID();
	url += this->pairingKey;
	url += "/";
	url += uuid;

	char* statusData = webapi::APIGetResponse(url.c_str());

	cJSON* response = cJSON_Parse(statusData);
	if (response == NULL)
	{
		ESP_LOGE(TAG, "Could not parse the JSON data");
		return false;
	}

	free(statusData);

	cJSON* authKeyJson = cJSON_GetObjectItemCaseSensitive(response, "AuthKey");
	if (!cJSON_IsString(authKeyJson) || (authKeyJson->valuestring == NULL))
	{
		// setup has not yet completed
		return false;
	}

	std::string authKey(authKeyJson->valuestring);
	cJSON_Delete(response);

	ESP_LOGI(TAG, "Got a AuthKey in the Status Response message, AuthKey=%s", authKey.c_str());

	this->authKey = authKey;
	return true;
}

bool SCPairing::FinishPairing()
{
	std::string url = std::string(API_GET_URL(WEB_API_GET_FINISH_PAIRING));
	std::string uuid = webapi::APICalculateUUID();
	url += this->pairingKey;
	url += "/";
	url += uuid;

	int statusCode;
	char* statusData = webapi::APIGetResponse(url.c_str(), statusCode);
	free(statusData);

	return (statusCode == 200);
}

bool SCPairing::HasPairingBegun()
{
	return !this->pairingKey.empty();
}

bool SCPairing::IsPairingComplete()
{
	return !this->authKey.empty();
}

std::string SCPairing::GetPairingKey()
{
	return this->pairingKey;
}

std::string SCPairing::GetAuthKey()
{
	return this->authKey;
}
