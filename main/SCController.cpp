/* Smart Chair Embedded Controller
 * Smart Chair Operations Controller
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "SCController.h"

#include <string>
#include <cstring>
#include <thread>
#include <chrono>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_spi_flash.h>
#include <esp_log.h>
#include <esp_pthread.h>
#include <nvs_flash.h>

#include "lwip/err.h"
#include "lwip/sys.h"

#include "api/SCOTAUpdate.h"

#include "api/SCDataClient.h"
#include "api/SensorDataModels.h"

EventGroupHandle_t SCController::wifi_event_group;

SCController::SCController():
	settings(),
	wifi(settings),
	webserver(),
	dataClient(this->settings),
	postureSensor(ADC1_CHANNEL_0, ADC1_CHANNEL_1, ADC1_CHANNEL_2, ADC1_CHANNEL_3, ADC1_CHANNEL_4, ADC1_CHANNEL_5),
	heartSensor(ADC1_CHANNEL_6 /*TODO: Correct Channel*/)
{
}

void SCController::Start()
{
	settings.Load();
	InitWifi();
	webserver.Start();

	/* Print chip information */
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	ESP_LOGI(TAG, "ESP32 chip with %d CPU cores, WiFi%s%s, ",
			chip_info.cores,
			(chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
			(chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

	ESP_LOGI(TAG, "ESP32 Silicon Revision %d, ", chip_info.revision);

	ESP_LOGI(TAG, "Flash Size: %dMB %s", spi_flash_get_chip_size() / (1024 * 1024),
			(chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

	ESP_LOGI(TAG, "Waiting for WiFi to Connect");
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);

    ESP_LOGI(TAG, "Checking for available firmware update");
    SCOTAUpdate::RunUpdateCheck();


    if (settings.auth_key.empty())
	{
		ESP_LOGI(TAG, "No authkey configured for this device");
	}
	else
	{
		ESP_LOGI(TAG, "This device has been configured with an authkey");

		// Only print out the secure AuthKey for internal and debug builds
		// No sensitive data should be logged in production builds
#ifndef PRODUCTION_BUILD
		ESP_LOGI(TAG, "The authkey configured for this device is %s", settings.auth_key.c_str());
#endif

		// Since the device is configured correctly, start all sensors
		// and allow them to POST to the server

		auto cfg = esp_pthread_get_default_config();
		cfg.stack_size = 1024 * 6;

		cfg.thread_name = "heart_thread";
	    esp_pthread_set_cfg(&cfg);
		heartSensorThread = std::thread(&SCController::SampleHeartRate, this);

		cfg.thread_name = "airquality_thread";
	    esp_pthread_set_cfg(&cfg);
		airQualitySensorThread = std::thread(&SCController::SampleAirQuality, this);

		// run the posture sensors on the main thread
		while (true)
		{
			ESP_LOGI(TAG, "Posting Posture Data");
			PostureSensorModel postureData;
			postureData.PostureData = postureSensor.getPosture();
			ESP_LOGI(TAG, "Posted data was %d", postureData.PostureData);

			//bool success = dataClient.PostPostureData(postureData);
			//if (!success) ESP_LOGE(TAG, "Error posting Posture data");

			/*
			ESP_LOGI(TAG, "Posting Motion Data");
			MotionEventModel motionData;
			// TODO: implement

			success = dataClient.PostMotionData(motionData);
			if (!success) ESP_LOGE(TAG, "Error posting Motion data");
			*/

			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
	}

	while (true)
	{
		// endless ultra-slow loop to prevent the device from rebooting
		vTaskDelay(100000 / portTICK_PERIOD_MS);
	}
}

void SCController::InitWifi()
{
	wifi_event_group = xEventGroupCreate();

	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM) );

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA) );
	wifi.ConnectSoftAPWifi(true);
	wifi.ConnectSTAWifi(true);

	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "init_wifi completed");
}

void SCController::SampleHeartRate()
{
	while (true)
	{
		ESP_LOGI(TAG, "Posting Heart Rate Data");
		//HeartRateSensorModel data;
		//data.MeasuredBPM = this->heartSensor.getHeartRate();

		//bool success = dataClient.PostHeartRateData(data);
		//if (!success) ESP_LOGE(TAG, "Error posting Heart Rate data");
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}

void SCController::SampleAirQuality()
{
	while (true)
	{
		ESP_LOGI(TAG, "Posting Air Quality Data");
		// TODO: implement
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
}

/*static*/ esp_err_t SCController::event_handler(void *ctx, system_event_t *event)
{
	// If a client connects to the SoftAP, the AP should remain active
	// until setup has finished completely and reboots.
	// Otherwise, the SoftAP should be disabled as soon as a primary connection
	// is established.
	static bool keep_softap_active = false;

    switch(event->event_id)
    {
		case SYSTEM_EVENT_STA_START:
		{
			esp_wifi_connect();
			break;
		}

		case SYSTEM_EVENT_STA_GOT_IP:
		{
			ESP_LOGI(TAG, "Connected to network and got ip:%s",
					ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

			xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);

			// turn SoftAP off
			if (!keep_softap_active)
			{
				ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
			}

			break;
		}

		case SYSTEM_EVENT_STA_DISCONNECTED:
		{
			xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
			ESP_LOGI(TAG, "Network connection lost, reconnecting...");

			// turn SoftAP back on
			ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
			esp_wifi_connect();

			ESP_LOGI(TAG,"retry to connect to the AP");
			vTaskDelay(6000 / portTICK_PERIOD_MS);

			break;
		}

		 case SYSTEM_EVENT_AP_STACONNECTED:
		 {
			ESP_LOGI(TAG, "Client connected to SoftAP, station:" MACSTR " join, AID=%d",
					MAC2STR(event->event_info.sta_connected.mac),
					event->event_info.sta_connected.aid);

			keep_softap_active = true;
			break;
		 }


		case SYSTEM_EVENT_AP_STADISCONNECTED:
		{
			ESP_LOGI(TAG, "station:" MACSTR "leave, AID=%d",
					MAC2STR(event->event_info.sta_disconnected.mac),
					event->event_info.sta_disconnected.aid);

			break;
		}

		default:
			break;
    }

    return ESP_OK;
}
