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

#include "SCSettings.h"
#include "SCWifi.h"
#include "SCWebServer.h"

class SCController
{
public:
	SCController();
	void Start();

private:
	void TestPostData();
	void InitWifi();
	static void ApplicationTaskImpl(void* _this);
	void ApplicationTask();

	static esp_err_t event_handler(void *ctx, system_event_t *event);

private:
	SCSettings settings;
	SCWifi wifi;
	SCWebServer webserver;

	static constexpr const char* TAG = "main app";

	static EventGroupHandle_t wifi_event_group;
	static constexpr const int WIFI_CONNECTED_BIT = BIT0;

};

#endif /*SCCONTROLLER_H*/
