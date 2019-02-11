/*
 * SCDataClient.cpp
 *
 *  Created on: Feb 7, 2019
 *      Author: Alex Dillon
 */

#include "SCDataClient.h"

#include <cjson/cJSON.h>

#include "webapi.h"

const std::string SCDataClient::DATETIME_MIN_VALUE = "0001-01-01T00:00:00.000Z";

SCDataClient::SCDataClient(std::string key):
	authKey(key)
{
}

bool SCDataClient::PostHeartRateData(HeartRateSensorModel data)
{
	cJSON* json = cJSON_CreateObject();

	// for now, we are ignoring the timestamp field and substituting DateTime.MinValue
	if (cJSON_AddStringToObject(json, "Timestamp", DATETIME_MIN_VALUE.c_str()) == NULL)
	{
		cJSON_Delete(json);
		return false;
	}

	if (cJSON_AddNumberToObject(json, "MeasuredBPM", data.MeasuredBPM) == NULL)
	{
		cJSON_Delete(json);
		return false;
	}

	char* jsonPayloadData = cJSON_Print(json);


	int response = webapi::APIPostData(API_GET_URL(WEB_API_POST_HEARTRATE),
			jsonPayloadData,
			this->authKey.c_str());

	ESP_LOGI(TAG, "Heart Rate POST Operation returned with status code %d", response);

	return (response == 200);
}

bool SCDataClient::PostPostureData(PostureSensorModel data)
{
	cJSON* json = cJSON_CreateObject();

	// for now, we are ignoring the timestamp field and substituting DateTime.MinValue
	if (cJSON_AddStringToObject(json, "Timestamp", DATETIME_MIN_VALUE.c_str()) == NULL)
	{
		cJSON_Delete(json);
		return false;
	}

	if (cJSON_AddNumberToObject(json, "PostureData", data.PostureData) == NULL)
	{
		cJSON_Delete(json);
		return false;
	}

	char* jsonPayloadData = cJSON_Print(json);


	int response = webapi::APIPostData(API_GET_URL(WEB_API_POST_POSTURE),
			jsonPayloadData,
			this->authKey.c_str());

	ESP_LOGI(TAG, "Posture POST Operation returned with status code %d", response);

	return (response == 200);
}

bool SCDataClient::PostMotionData(MotionEventModel data)
{
	cJSON* json = cJSON_CreateObject();

	// for now, we are ignoring the timestamp field and substituting DateTime.MinValue
	if (cJSON_AddStringToObject(json, "Timestamp", DATETIME_MIN_VALUE.c_str()) == NULL)
	{
		cJSON_Delete(json);
		return false;
	}

	if (cJSON_AddNumberToObject(json, "Axis", data.Axis) == NULL)
	{
		cJSON_Delete(json);
		return false;
	}

	if (cJSON_AddNumberToObject(json, "Level", data.Level) == NULL)
		{
			cJSON_Delete(json);
			return false;
		}

	char* jsonPayloadData = cJSON_Print(json);


	int response = webapi::APIPostData(API_GET_URL(WEB_API_POST_MOTION),
			jsonPayloadData,
			this->authKey.c_str());

	ESP_LOGI(TAG, "Motion Event POST Operation returned with status code %d", response);

	return (response == 200);
}
