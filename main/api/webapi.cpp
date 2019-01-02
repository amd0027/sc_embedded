/* Smart Chair Embedded Controller
 * Web API Definitions - Helper Functions
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <esp_http_client.h>
#include <esp_https_ota.h>

#include <tcpip_adapter.h>

#include "webapi.h"

namespace webapi
{
	static const char *TAG = "api_defines";

	esp_err_t api_http_event_handler(esp_http_client_event_t *evt)
	{
		switch(evt->event_id) {
			case HTTP_EVENT_ERROR:
				ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
				break;
			case HTTP_EVENT_ON_CONNECTED:
				ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
				break;
			case HTTP_EVENT_HEADER_SENT:
				ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
				break;
			case HTTP_EVENT_ON_HEADER:
				ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
				break;
			case HTTP_EVENT_ON_DATA:
				ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
				break;
			case HTTP_EVENT_ON_FINISH:
				ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
				break;
			case HTTP_EVENT_DISCONNECTED:
				ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
				break;
		}
		return ESP_OK;
	}

	char* api_get_response(const char* url)
	{
		ESP_LOGI(TAG, "API: %s", url);

		esp_http_client_config_t config = { };
		config.url = url;
		config.event_handler = api_http_event_handler;
		config.cert_pem = API_CERT_PEM_START;

		esp_http_client_handle_t client = esp_http_client_init(&config);
		esp_err_t err = esp_http_client_perform(client);

		char* response_data = NULL;

		if (err == ESP_OK)
		{
			ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %d",
					esp_http_client_get_status_code(client),
					esp_http_client_get_content_length(client));

			int response_length = esp_http_client_get_content_length(client);
			response_data = (char*)malloc(response_length + 1);
			if (response_data == NULL)
			{
				ESP_LOGE(TAG, "Could not allocate buffer for api response");
			}
			else
			{
				esp_http_client_read(client, response_data, response_length);
				response_data[response_length] = '\0';
			}
		}
		else
		{
			ESP_LOGE(TAG, "Error checking for update: %s", esp_err_to_name(err));
		}

		esp_http_client_cleanup(client);

		return response_data;
	}

	std::string mac_to_string(uint8_t mac_data[])
	{
		std::stringstream os;
		char oldFill = os.fill('0');

		os << std::setw(2) << std::hex << static_cast<unsigned int>(mac_data[0]);
		for (uint i = 1; i < 6; i++) {
			os << ':' << std::setw(2) << std::hex << std::uppercase << static_cast<unsigned int>(mac_data[i]);
		}

		os.fill(oldFill);

		return os.str();
	}

	std::string api_calculate_uuid()
	{
		uint8_t mac[6];
		esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
		std::string mac_string = mac_to_string(mac);

		// TODO: add some more stuff on the end of the MAC to form the UUID

		return mac_string;
	}

} /*webapi::*/