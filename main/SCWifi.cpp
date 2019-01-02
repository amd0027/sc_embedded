/* Smart Chair Embedded Controller
 * WiFi Controller
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "SCWifi.h"

#include <cstring>

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_log.h>

#include "SCSettings.h"

#define SOFT_AP_NAME "Smart Chair"

SCWifi::SCWifi(SCSettings& config):
	configuration(config)
{

}

void SCWifi::ConnectSTAWifi(bool delay_init)
{
	if (configuration.Load())
	{
		ESP_LOGD(TAG, "Beginning WiFi STA Connect");

		wifi_config_t wifi_config_sta = { };

		memcpy(wifi_config_sta.sta.ssid, configuration.wifi_ssid.c_str(), 64);
		memcpy(wifi_config_sta.sta.password, configuration.wifi_pass.c_str(), 32);

		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta));

		if (!delay_init)
		{
			ESP_ERROR_CHECK(esp_wifi_start());
			ESP_ERROR_CHECK(esp_wifi_connect());
		}
	}
}

void SCWifi::ConnectSoftAPWifi(bool delay_init)
{
	ESP_LOGD(TAG, "Beginning SoftAP Connect");

	// Assign an IP Address of the SoftAP interface
	tcpip_adapter_ip_info_t info = {  };
	IP4_ADDR(&info.ip, 192, 168, 123, 1);
	IP4_ADDR(&info.gw, 192, 168, 123, 1);
	IP4_ADDR(&info.netmask, 255, 255, 255, 0);
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
	ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));

	wifi_config_t wifi_config_ap = { };
	wifi_config_ap.ap.ssid_len = strlen(SOFT_AP_NAME);
	memcpy(wifi_config_ap.sta.ssid, SOFT_AP_NAME, wifi_config_ap.ap.ssid_len);
	wifi_config_ap.ap.ssid_len = strlen(SOFT_AP_NAME);
	wifi_config_ap.ap.channel = 1;
	wifi_config_ap.ap.authmode = WIFI_AUTH_OPEN;
	wifi_config_ap.ap.beacon_interval = 400; 	// TODO: can we remove this?
	wifi_config_ap.ap.max_connection = 16; 		// TODO: should we lower this?

	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_ap));

	if (!delay_init)
	{
		ESP_ERROR_CHECK(esp_wifi_start());
		ESP_ERROR_CHECK(esp_wifi_connect());
	}
}

