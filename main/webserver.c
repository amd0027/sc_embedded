/* Smart Chair Embedded Controller
 * Web Administration Server
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "webserver.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_https_server.h>

#include "config.h"
#include "webrender.h"
#include "wifi.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

static const char* TAG = "webserver";

httpd_handle_t server = NULL;

// Since we only have a couple of static files, we can directly embed them into the
// application firmware as we are doing here. If we end up needing more embedded files,
// we should move to using the SPIFS file system to store them, and flashing a partition
// on the flash with the required files

esp_err_t network_get_handler(httpd_req_t *req)
{
	extern const char network_start[] asm("_binary_network_html_start");
	extern const char network_end[]   asm("_binary_network_html_end");
	int len = network_end - network_start;

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, network_start, -1);

    return ESP_OK;
}

const httpd_uri_t network = {
    .uri       = "/network.html",
    .method    = HTTP_GET,
    .handler   = network_get_handler
};

esp_err_t info_get_handler(httpd_req_t *req)
{
	extern const char index_start[] asm("_binary_info_html_start");
	extern const char index_end[]   asm("_binary_info_html_end");
	int len = index_end - index_start;

	char* response = render_info_page(index_start);

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, response, -1);

    free(response);

    return ESP_OK;
}

const httpd_uri_t info = {
    .uri       = "/info.html",
    .method    = HTTP_GET,
    .handler   = info_get_handler
};

esp_err_t css_base_min_get_handler(httpd_req_t *req)
{
	extern const char pure_min_start[] asm("_binary_puremin_css_start");
	extern const char pure_min_end[]   asm("_binary_puremin_css_end");
	int len = pure_min_end - pure_min_start;

    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, pure_min_start, len);

    return ESP_OK;
}

const httpd_uri_t css_pure_min = {
    .uri       = "/puremin.css",
    .method    = HTTP_GET,
    .handler   = css_base_min_get_handler
};

esp_err_t done_handler(httpd_req_t *req)
{
	extern const char done_start[] asm("_binary_done_html_start");
	extern const char done_end[]   asm("_binary_done_html_end");
	int len = done_end - done_start;

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, done_start, len);

    return ESP_OK;
}

const httpd_uri_t done = {
    .uri       = "/done.html",
    .method    = HTTP_GET,
    .handler   = done_handler
};


esp_err_t setnetwork_post_handler(httpd_req_t *req)
{
	char content[100];

	/* Truncate if content length larger than the buffer */
	size_t recv_size = MIN(req->content_len, sizeof(content));

	int ret = httpd_req_recv(req, content, recv_size);
	if (ret <= 0) {  /* 0 return value indicates connection closed */
		/* Check if timeout occurred */
		if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
			httpd_resp_send_408(req);
		}

		return ESP_FAIL; // ensure socket is closed
	}

	char* pw = parse_post_data(content, "pw=");
	char* ssid = parse_post_data(content, "ssid=");


	printf("ssid=%s\n", ssid);
	printf("pw=%s\n", pw);
	printf("------DONE------;");

	int ssid_len = strlen(ssid);
	int pw_len = strlen(pw);

	if (ssid_len > 0 && ssid_len < 65 &&
			pw_len > 0 && pw_len < 33)
	{
		// valid entries submitted
		sc_config_t* config = config_get_handle();
		strcpy(config->wifi_ssid, ssid);
		strcpy(config->wifi_pass, pw);
		config_save();

		free(ssid);
		free(pw);

		httpd_resp_set_hdr(req, "Location", "/done.html?err=Rebooting%20Now");
		httpd_resp_set_status(req, "302");
		httpd_resp_send(req, "", 0);

		esp_wifi_disconnect();
		connect_sta_wifi(false); // force immediate re-initialization of the adapter
	}
	else
	{
		httpd_resp_set_hdr(req, "Location", "/done.html?err=Invalid%20Entry%2C%20Please%20Try%20Again");
		httpd_resp_set_status(req, "302");
		httpd_resp_send(req, "", 0);
	}

    return ESP_OK;
}

const httpd_uri_t setnetwork = {
    .uri       = "/setnetwork",
    .method   = HTTP_POST,
    .handler  = setnetwork_post_handler,
    .user_ctx = NULL
};


const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = network_get_handler
};

void webserver_start()
{
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server");

    if (server != NULL)
    {
    	ESP_LOGW(TAG, "HTTPS server is already running");
    	return;
    }

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

    extern const unsigned char cacert_pem_start[] asm("_binary_cacert_pem_start");
    extern const unsigned char cacert_pem_end[]   asm("_binary_cacert_pem_end");
    conf.cacert_pem = cacert_pem_start;
    conf.cacert_len = cacert_pem_end - cacert_pem_start;

    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
    conf.prvtkey_pem = prvtkey_pem_start;
    conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

    esp_err_t ret = httpd_ssl_start(&server, &conf);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting server!");

        server = NULL;
        return;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &info);
    httpd_register_uri_handler(server, &network);

    httpd_register_uri_handler(server, &done);

    httpd_register_uri_handler(server, &css_pure_min);

    httpd_register_uri_handler(server, &setnetwork);
}

void webserver_stop()
{
    // Stop the httpd server
	if (server != NULL)
	{
		httpd_ssl_stop(server);
	}
}
