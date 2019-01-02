/* Smart Chair Embedded Controller
 * Active Page Rendering Logic
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "webrender.h"

#include <string>
#include <cstring>

#include <stdio.h>
#include <stdint.h>

#include <esp_wifi.h>
#include <esp_image_format.h>
#include <esp_ota_ops.h>
#include <tcpip_adapter.h>

#include "api/webapi.h"

namespace webrender
{
	std::string ParsePostData (char* sourceData, std::string key)
	{
		std::string data(sourceData);
		std::size_t keyIndex = data.find(key);
		if (keyIndex == std::string::npos)
		{
			return std::string();
		}

		std::size_t valueIndex = data.find("=", keyIndex);
		std::size_t endIndex = data.find("&", valueIndex);
		if (endIndex == std::string::npos)
		{
			endIndex = data.size();
		}

		std::size_t valueLength = endIndex - valueIndex - 1;
		return data.substr(valueIndex + 1, valueLength);
	}

	std::string RenderInfoPage(const char* pageTemplate)
	{
		std::string source(pageTemplate);

		// IP address
		char ip_chars[20];
		tcpip_adapter_ip_info_t ipInfo;
		tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);
		snprintf(ip_chars, 20, IPSTR, IP2STR(&ipInfo.ip));
		std::string ip_string(ip_chars);

		// MAC Address
		uint8_t mac[6];
		esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
		std::string mac_string = webapi::MacToString(mac);

		// Firmware Version
		const esp_app_desc_t *app_desc = esp_ota_get_app_description();
		std::string sw_version(app_desc->version);

		source.replace(source.find("{IPADDRESS}"), std::string("{IPADDRESS}").size(), ip_string);
		source.replace(source.find("{MACADDRESS}"), std::string("{MACADDRESS}").size(), mac_string);
		source.replace(source.find("{SWVERSION}"), std::string("{SWVERSION}").size(), sw_version);

		return source;
	}

	std::string RenderPairingPage(const char* pageTemplate, std::string pairingKey)
	{
		std::string source(pageTemplate);

		std::string userPairingLink(WEB_USER_CHAIR_PAIRING);
		userPairingLink += pairingKey;

		// TODO: put the URL in
		source.replace(source.find("{PAIRINGCODE}"), std::string("{PAIRINGCODE}").size(), pairingKey);
		source.replace(source.find("{SMARTCHAIRURL}"), std::string("{SMARTCHAIRURL}").size(), userPairingLink); // the hyperlink
		source.replace(source.find("{SMARTCHAIRURL}"), std::string("{SMARTCHAIRURL}").size(), userPairingLink); // the popup link

		return source;
	}

} /*webrender::*/
