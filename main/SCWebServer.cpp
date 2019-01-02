/* Smart Chair Embedded Controller
 * Web Administration Server
 *
 * Team Smart Chair - UAH CPE496 2019
*/

// Since we only have a couple of static files, we can directly embed them into the
// application firmware as we are doing here. If we end up needing more embedded files,
// we should move to using the SPIFS file system to store them, and flashing a partition
// on the flash with the required files

#include "SCWebServer.h"

#include <string>
#include <cstring>
#include <algorithm>

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
extern "C"
{
	// esp_https_server has some issues, including a lack of extern-"C" blocks in the header
	#include <esp_https_server.h>
}

#include "SCSettings.h"
#include "SCWifi.h"
#include "webrender.h"
#include "api/SCPairing.h"

SCWebServer::SCWebServer():
		server(NULL)
{
}

esp_err_t network_get_handler(httpd_req_t *req)
{
	extern const char network_start[] asm("_binary_network_html_start");
	extern const char network_end[]   asm("_binary_network_html_end");
	int len = network_end - network_start;

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, network_start, len);

    return ESP_OK;
}

const httpd_uri_t network = {
    .uri       = "/network.html",
    .method    = HTTP_GET,
    .handler   = network_get_handler,
	.user_ctx = NULL
};


esp_err_t info_get_handler(httpd_req_t *req)
{
	extern const char index_start[] asm("_binary_info_html_start");

	std::string response = webrender::RenderInfoPage(index_start);

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, response.c_str(), -1);

    return ESP_OK;
}

const httpd_uri_t info = {
    .uri       = "/info.html",
    .method    = HTTP_GET,
    .handler   = info_get_handler,
	.user_ctx = NULL
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
    .handler   = css_base_min_get_handler,
	.user_ctx = NULL
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
    .handler   = done_handler,
	.user_ctx = NULL
};

esp_err_t pairing_handler(httpd_req_t *req)
{
	extern const char pairing_start[] asm("_binary_pairing_html_start");
	SCSettings config;
	config.Load();

	if (!config.auth_key.empty())
	{
		httpd_resp_set_hdr(req, "Location", "/done.html?err=This%20Chair%20Is%20Already%20Paired");
		httpd_resp_set_status(req, "302");
		httpd_resp_send(req, "", 0);

		return ESP_OK;
	}

	static SCPairing pairing;
	if (!pairing.HasPairingBegun())
	{
		pairing.BeginPairing();
	}
	else
	{
		pairing.PollPairingStatus();
	}

	if (pairing.IsPairingComplete())
	{
		pairing.FinishPairing();

		config.auth_key = pairing.GetAuthKey();
		config.Save();

		httpd_resp_set_hdr(req, "Location", "/done.html?err=Pairing%20Completed%20Successfully");
		httpd_resp_set_status(req, "302");
		httpd_resp_send(req, "", 0);
	}
	else
	{
		std::string response = webrender::RenderPairingPage(pairing_start, pairing.GetPairingKey());

		httpd_resp_set_type(req, "text/html");
		httpd_resp_send(req, response.c_str(), -1);
	}

    return ESP_OK;
}

const httpd_uri_t pairing = {
    .uri       = "/pairing.html",
    .method    = HTTP_GET,
    .handler   = pairing_handler,
	.user_ctx = NULL
};


esp_err_t setnetwork_post_handler(httpd_req_t *req)
{
	// TODO: this is horrible. These should be class members,
	// but esp_https_server is not designed to take member function pointers
	// for url callbacks.
	SCSettings config;
	config.Load();
	SCWifi wifi(config);

	char content[100];

	/* Truncate if content length larger than the buffer */
	std::size_t recv_size = std::min(req->content_len, sizeof(content));

	int ret = httpd_req_recv(req, content, recv_size);
	if (ret <= 0) {  /* 0 return value indicates connection closed */
		/* Check if timeout occurred */
		if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
			httpd_resp_send_408(req);
		}

		return ESP_FAIL; // ensure socket is closed
	}

	std::string ssid = webrender::ParsePostData(content, "ssid=");
	std::string pw = webrender::ParsePostData(content, "pw=");

	int ssid_len = ssid.size();
	int pw_len = pw.size();

	if (ssid_len > 0 && ssid_len < 65 &&
			pw_len > 0 && pw_len < 33)
	{
		// valid entries submitted
		config.wifi_ssid = ssid;
		config.wifi_pass = pw;
		config.Save();

		httpd_resp_set_hdr(req, "Location", "/done.html?err=Rebooting%20Now");
		httpd_resp_set_status(req, "302");
		httpd_resp_send(req, "", 0);

		esp_wifi_disconnect();

		wifi.ConnectSTAWifi(false); // force immediate re-initialization of the adapter
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
    .handler   = network_get_handler,
	.user_ctx = NULL
};

void SCWebServer::Start()
{
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server");

    if (server != NULL)
    {
    	ESP_LOGW(TAG, "HTTPS server is already running");
    	return;
    }

    /*------HTTPD_SSL_CONFIG_DEFAULT does NOT compile under G++ 5.2.0*/
    /*Until Non-Trivial C Initializer Support is added to the CPP Toolkit, manual init must be used*/
    /*Default Config Copied from esp-idf/components/esp_https_server/include/esp_https_server.h*/

    httpd_ssl_config_t conf = { };

    /*---Default Value Configuration from HTTPD_SSL_CONFIG_DEFAULT*/
    conf.httpd.task_priority      = tskIDLE_PRIORITY+5;
    conf.httpd.stack_size         = 10240;
    conf.httpd.server_port        = 0;
    conf.httpd.ctrl_port          = 32768;
    conf.httpd.max_open_sockets   = 4;
    conf.httpd.max_uri_handlers   = 8;
    conf.httpd.max_resp_headers   = 8;
    conf.httpd.backlog_conn       = 5;
    conf.httpd.lru_purge_enable   = true;
    conf.httpd.recv_wait_timeout  = 5;
    conf.httpd.send_wait_timeout  = 5;
	conf.httpd.global_user_ctx = NULL;
	conf.httpd.global_user_ctx_free_fn = NULL;
	conf.httpd.global_transport_ctx = NULL;
	conf.httpd.global_transport_ctx_free_fn = NULL;
	conf.httpd.open_fn = NULL;
	conf.httpd.close_fn = NULL;

	conf.transport_mode = HTTPD_SSL_TRANSPORT_SECURE;
	conf.port_secure = 443;
	conf.port_insecure = 80;
	/*---End Defaults from HTTPD_SSL_CONFIG_DEFAULT--*/

    extern const unsigned char cacert_pem_start[] asm("_binary_cacert_pem_start");
    extern const unsigned char cacert_pem_end[]   asm("_binary_cacert_pem_end");

    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");

    conf.cacert_pem = cacert_pem_start;
    conf.cacert_len = cacert_pem_end - cacert_pem_start;

    conf.prvtkey_pem = prvtkey_pem_start;
    conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

    esp_err_t ret = httpd_ssl_start(&server, &conf);
    if (ESP_OK != ret)
    {
        ESP_LOGI(TAG, "Error starting server!");

        server = NULL;
        return;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &info);
    httpd_register_uri_handler(server, &network);
    httpd_register_uri_handler(server, &pairing);

    httpd_register_uri_handler(server, &done);

    httpd_register_uri_handler(server, &css_pure_min);

    httpd_register_uri_handler(server, &setnetwork);
}

void SCWebServer::Stop()
{
    // Stop the httpd server
	if (server != NULL)
	{
		httpd_ssl_stop(server);
	}
}


