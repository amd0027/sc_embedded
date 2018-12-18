/* Smart Chair Embedded Controller
 * WiFi Controller
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "wifi.h"

#include <string.h>

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_log.h>

#include "config.h"

const char* TAG = "wifi";

void connect_sta_wifi(bool delay_init)
{
	if (config_load())
	{
		ESP_LOGD(TAG, "Beginning WiFi STA Connect");

		sc_config_t* config = config_get_handle();
		wifi_config_t wifi_config_sta = {
			.sta = {
				.ssid = "placeholderSSID",
				.password = "placeholderPASS",
			}
		};

		// SSID and PASS are defined as uint8_t in the ESP-IDF, contrary to the documentation
		// copy the real values in place of the placeholders

		memcpy(wifi_config_sta.sta.ssid, config->wifi_ssid, 64);
		memcpy(wifi_config_sta.sta.password, config->wifi_pass, 32);

		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta));

		if (!delay_init)
		{
			ESP_ERROR_CHECK(esp_wifi_start());
			ESP_ERROR_CHECK(esp_wifi_connect());
		}
	}
}

void connect_softap_wifi(bool delay_init)
{
	ESP_LOGD(TAG, "Beginning SoftAP Connect");

	// Assign an IP Address of the SoftAP interface
	tcpip_adapter_ip_info_t info = { 0, };
	IP4_ADDR(&info.ip, 192, 168, 123, 1);
	IP4_ADDR(&info.gw, 192, 168, 123, 1);
	IP4_ADDR(&info.netmask, 255, 255, 255, 0);
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
	ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));

	#define SOFT_AP_NAME "Smart Chair"

	wifi_config_t wifi_config_ap = {
		.ap = {
			.ssid = SOFT_AP_NAME,
			.ssid_len = strlen(SOFT_AP_NAME),
			.channel = 1,
			.authmode = WIFI_AUTH_OPEN,
			.beacon_interval = 400,
			.max_connection = 16,
		}
	};

	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_ap));

	if (!delay_init)
	{
		ESP_ERROR_CHECK(esp_wifi_start());
		ESP_ERROR_CHECK(esp_wifi_connect());
	}
}

