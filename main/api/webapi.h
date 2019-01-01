/* Smart Chair Embedded Controller
 * Web API Definitions
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef API_DEFINES_H
#define API_DEFINES_H


#include <string>

#include <esp_log.h>
#include <esp_http_client.h>

namespace webapi
{

	/*------Web API Endpoint URLs-------------------------------------------*/
	#define WEB_API_BASE_URL 			"https://microcube.info/api/"
	#define WEB_API_UPDATE_CHECK_URL	"PairingOperations/GetLatestVersion"
	#define WEB_API_GET_PAIRING_CODE	"PairingOperations/GetPairingCode/"

	/*------Web API Macros--------------------------------------------------*/
	#define API_GET_URL(op) WEB_API_BASE_URL "" op


	/*------Web API Root Certificates----------------------------------------*/
	extern const char API_CERT_PEM_START[]	asm("_binary_api_ca_cert_pem_start");
	extern const char API_CERT_PEM_END[] 	asm("_binary_api_ca_cert_pem_end");


	/*------Web API Helper Function Prototype Defines-------------------------*/
	char* api_get_response(const char* url);
	std::string mac_to_string(uint8_t mac_data[]);
	std::string api_calculate_uuid();

	esp_err_t api_http_event_handler(esp_http_client_event_t* evt);
}

#endif /*API_DEFINES_H*/
