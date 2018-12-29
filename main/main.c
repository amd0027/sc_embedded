/* Smart Chair Embedded Controller
 * Startup Code
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_spi_flash.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include "lwip/err.h"
#include "lwip/sys.h"

#include "wifi.h"
#include "config.h"
#include "webserver.h"
#include "api/https_ota.h"

static const char *TAG = "main app";

static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

void init_wifi();
void update_check(void* pvParameter);
static esp_err_t event_handler(void *ctx, system_event_t *event);

void app_main()
{
	bool configured = config_load();

	init_wifi();
	webserver_start();

    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    for (int i = 1; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    // esp_restart();

    xTaskCreate(&update_check, "update_check", 8192, NULL, 5, NULL);
}

void init_wifi()
{
	wifi_event_group = xEventGroupCreate();

	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA) );
	connect_softap_wifi(true);
	connect_sta_wifi(true);

	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "init_wifi completed");
}

void update_check(void* pvParameter)
{
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Checking for available firmware update");

    https_ota_check();

    while(1)
    {

    }
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	// If a client connects to the SoftAP, the AP should remain active
	// until setup has finished completely and reboots.
	// Otherwise, the SoftAP should be disabled as soon as a primary connection
	// is established.
	static bool keep_softap_active = false;

    switch(event->event_id) {
		case SYSTEM_EVENT_STA_START:
			esp_wifi_connect();
			break;

		case SYSTEM_EVENT_STA_GOT_IP:
			ESP_LOGI(TAG, "Connected to network and got ip:%s",
					 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
			xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);

			// turn SoftAP off
			if (!keep_softap_active)
			{
				ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
			}

			break;

		case SYSTEM_EVENT_STA_DISCONNECTED:
			{
				ESP_LOGI(TAG, "Network connection lost, reconnecting...");

				// turn SoftAP back on
				ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA) );

				esp_wifi_connect();
				xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
				ESP_LOGI(TAG,"retry to connect to the AP");
				vTaskDelay(6000 / portTICK_PERIOD_MS);

				break;
			}

		 case SYSTEM_EVENT_AP_STACONNECTED:
			ESP_LOGI(TAG, "Client connected to SoftAP, station:"MACSTR" join, AID=%d",
					 MAC2STR(event->event_info.sta_connected.mac),
					 event->event_info.sta_connected.aid);

			keep_softap_active = true;
			break;

		case SYSTEM_EVENT_AP_STADISCONNECTED:
			ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d",
					 MAC2STR(event->event_info.sta_disconnected.mac),
					 event->event_info.sta_disconnected.aid);

			//webserver_stop();
			break;

		default:
			break;
    }

    return ESP_OK;
}
