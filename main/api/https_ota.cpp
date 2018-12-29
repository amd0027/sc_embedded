/* Smart Chair Embedded Controller
 * Secure OTA Update Controller
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "https_ota.h"

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <esp_http_client.h>
#include <esp_https_ota.h>

#include <json.hpp>

#include "api_defines.h"

static const char *TAG = "https_ota";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
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

void perform_ota_update(char* update_url)
{
	esp_http_client_config_t config = { };
	config.url = update_url;
	config.event_handler = _http_event_handler;
	config.cert_pem = (char*)API_CERT_PEM_START;

	esp_err_t ret = esp_https_ota(&config);
	if (ret == ESP_OK)
	{
		esp_restart();
	}
	else
	{
		ESP_LOGE(TAG, "Firmware Upgrade Failed");
	}

	while (1)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

bool check_update_needed()
{
	ESP_LOGI(TAG, API_GET_URL(WEB_API_UPDATE_CHECK_URL));

	esp_http_client_config_t config = { };
	config.url = API_GET_URL(WEB_API_UPDATE_CHECK_URL);
	config.event_handler = _http_event_handler;
	config.cert_pem = (char*)API_CERT_PEM_START;

	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_err_t err = esp_http_client_perform(client);

	if (err == ESP_OK)
	{
		ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %d",
				esp_http_client_get_status_code(client),
				esp_http_client_get_content_length(client));

		int response_length = esp_http_client_get_content_length(client);
		char* response_data = (char*)malloc(response_length);
		if (response_data == NULL)
		{
			ESP_LOGE(TAG, "Could not allocate buffer for update check response");
			return false;
		}

		esp_http_client_read(client, response_data, response_length);

		ESP_LOGI(TAG, "%s", response_data);
	}
	else
	{
		ESP_LOGE(TAG, "Error checking for update: %s", esp_err_to_name(err));
	}

	esp_http_client_cleanup(client);

	return false;
}

void https_ota_check()
{
	check_update_needed();
}
