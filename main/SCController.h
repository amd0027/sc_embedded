/* Smart Chair Embedded Controller
 * Smart Chair Operations Controller Header File
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef SCCONTROLLER_H
#define SCCONTROLLER_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_event.h>
#include <sys/time.h>

#include <thread>

#include "SCSettings.h"
#include "SCWifi.h"
#include "SCWebServer.h"
#include "api/SCDataClient.h"
#include "api/SensorDataModels.h"
#include "drivers/SCPosture.h"
#include "drivers/SCHeartRate.h"
#include "drivers/SCMotion.h"

class SCController
{
public:
	SCController();
	void Start();

private:
	void InitWifi();
	int SamplePosture();
	void SampleMotion();
	void SampleHeartRate();
	void SampleAirQuality();

	void PostOccupancySession(int elapsedSeconds);

	void EnterDeepSleep();

	static esp_err_t event_handler(void *ctx, system_event_t *event);

private:
	SCSettings settings;
	SCWifi wifi;
	SCWebServer webserver;

	SCDataClient dataClient;
	SCPosture postureSensor;
	SCHeartRate heartSensor;
	SCMotion motionSensor;

	std::thread motionSensorThread;
	std::thread heartSensorThread;
	std::thread airQualitySensorThread;

	bool isOccupied;
	struct timeval occupiedBeginTime;

	static constexpr const char* TAG = "main app";

	static EventGroupHandle_t wifi_event_group;
	static constexpr const int WIFI_CONNECTED_BIT = BIT0;
};

#endif /*SCCONTROLLER_H*/
