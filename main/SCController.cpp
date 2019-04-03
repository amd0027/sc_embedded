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
	heartSensor(ADC1_CHANNEL_6 /*TODO: Correct Channel*/),
	motionSensor(),
	isOccupied(false)
{
}

void SCController::Start()
{
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

	ESP_LOGI(TAG, "Beginning chair initialization process");
	settings.Load();

	if (!settings.auth_key.empty())
	{
		// Before we begin to initialize the chair, check and see if it is occupied
		// If not, return to Deep Sleep mode before Wi-Fi connects to save power
		SamplePosture();
		if (!isOccupied)
		{
			EnterDeepSleep();
		}
	}

	InitWifi();
	webserver.Start();

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

		cfg.thread_name = "motion_thread";
		esp_pthread_set_cfg(&cfg);
		motionSensorThread = std::thread(&SCController::SampleMotion, this);

		cfg.thread_name = "heart_thread";
	    esp_pthread_set_cfg(&cfg);
		heartSensorThread = std::thread(&SCController::SampleHeartRate, this);

		cfg.thread_name = "airquality_thread";
	    esp_pthread_set_cfg(&cfg);
		airQualitySensorThread = std::thread(&SCController::SampleAirQuality, this);

		// run the posture sensors on the main thread
		while (true)
		{
			int postureData = SamplePosture();

			// Post Posture Data
			if (isOccupied)
			{
				ESP_LOGI(TAG, "Posting Posture Data");
				PostureSensorModel postureDataModel;
				postureDataModel.PostureData = postureData;
				//ESP_LOGI(TAG, "Posted data was %d", postureData.PostureData);
				bool success = dataClient.PostPostureData(postureDataModel);
				if (!success) ESP_LOGE(TAG, "Error posting Posture data");
			}

			// Delay or Sleep
			if (isOccupied)
			{
				// delay?
			}
			else
			{
				EnterDeepSleep();
			}
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

int SCController::SamplePosture()
{
	static const int POSTURE_AVERAGING_COUNT = 5;
	static const int POSTURE_AVERAGING_DELAY_SEC = 8;

	/*---- Sample Posture Data ---------*/
	int postureData = postureSensor.getPosture();
	std::this_thread::sleep_for(std::chrono::seconds(POSTURE_AVERAGING_DELAY_SEC));
	for (int i = 0; i < POSTURE_AVERAGING_COUNT-1; i++)
	{
		int newSample = postureSensor.getPosture();
		postureData = SCPosture::AveragePostureSamples(postureData, newSample);
		std::this_thread::sleep_for(std::chrono::seconds(POSTURE_AVERAGING_DELAY_SEC));
	}

	ESP_LOGI(TAG, "The averaged value is %d", postureData);

	isOccupied = SCPosture::PredictOccupied(postureData);
	if (isOccupied) ESP_LOGI(TAG, "The SmartChair is currently occupied");
	else			ESP_LOGI(TAG, "The SmartChair is currently NOT occupied");

	return postureData;
}

void SCController::SampleMotion()
{
	while (true)
	{
		/*---- Sample Motion Data ---------*/
		motionSensor.SampleAccelerometer();

		ESP_LOGI(TAG, "Posting Motion Data");
		MotionEventModel motionData;
		// TODO: implement

		//bool success = dataClient.PostMotionData(motionData);
		//if (!success) ESP_LOGE(TAG, "Error posting Motion data");

		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

void SCController::SampleHeartRate()
{
	while (true)
	{
		ESP_LOGI(TAG, "Sampling Heart Rate Data");
		int measuredBpm = 0; //= heartSensor.getHeartRate();
		if (measuredBpm > 0)
		{
			ESP_LOGI(TAG, "Posting Heart Rate Data");
			HeartRateSensorModel data;
			data.MeasuredBPM = measuredBpm;

			//bool success = dataClient.PostHeartRateData(data);
			//if (!success) ESP_LOGE(TAG, "Error posting Heart Rate data");
		}

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

void SCController::EnterDeepSleep()
{
	ESP_LOGI(TAG, "Entering Deep Sleep Mode....");

	esp_wifi_stop();

    const int deepSleepMinutes = 5;
    int deepSleepSec = deepSleepMinutes * 60;
    ESP_LOGI(TAG, "Entering deep sleep for %d seconds", deepSleepSec);
    esp_deep_sleep(1000000LL * deepSleepSec);
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
