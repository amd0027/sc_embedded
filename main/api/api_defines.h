/* Smart Chair Embedded Controller
 * Web API Definitions
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef API_DEFINES_H
#define API_DEFINES_H

#define WEB_API_BASE_URL "https://microcube.info/api/"
#define WEB_API_UPDATE_CHECK_URL "PairingOperations/GetLatestVersion"

#define API_GET_URL(op) WEB_API_BASE_URL "" op


extern const uint8_t API_CERT_PEM_START[] asm("_binary_api_ca_cert_pem_start");
extern const uint8_t API_CERT_PEM_END[] asm("_binary_api_ca_cert_pem_end");

#endif /*API_DEFINES_H*/
