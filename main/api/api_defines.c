/* Smart Chair Embedded Controller
 * Web API Definitions - Helper Functions
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "api_defines.h"

#include <esp_system.h>
#include <esp_http_client.h>
#include <esp_log.h>

static const char *TAG = "api_defines";

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

char* api_get_response(const char* url)
{
	ESP_LOGI(TAG, "API: %s", url);

	esp_http_client_config_t config = { };
	config.url = url;
	config.event_handler = _http_event_handler;
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
