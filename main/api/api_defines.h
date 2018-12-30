/* Smart Chair Embedded Controller
 * Web API Definitions
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef API_DEFINES_H
#define API_DEFINES_H


#ifdef __cplusplus
extern "C" {
#endif


/*------Web API Endpoint URLs-------------------------------------------*/
#define WEB_API_BASE_URL 			"https://microcube.info/api/"
#define WEB_API_UPDATE_CHECK_URL	"PairingOperations/GetLatestVersion"


/*------Web API Macros--------------------------------------------------*/
#define API_GET_URL(op) WEB_API_BASE_URL "" op


/*------Web API Root Certificates----------------------------------------*/
extern const char API_CERT_PEM_START[]	asm("_binary_api_ca_cert_pem_start");
extern const char API_CERT_PEM_END[] 	asm("_binary_api_ca_cert_pem_end");


/*------Web API Helper Function Prototype Defines-------------------------*/
char* api_get_response(const char* url);


#ifdef __cplusplus
}
#endif

#endif /*API_DEFINES_H*/
