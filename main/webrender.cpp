/* Smart Chair Embedded Controller
 * Active Page Rendering Logic
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#include "webrender.h"

#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <stdio.h>
#include <stdint.h>

#include <esp_wifi.h>
#include <esp_image_format.h>
#include <esp_ota_ops.h>

#include <tcpip_adapter.h>

std::string parse_post_data (std::string data, std::string key)
{
    std::size_t key_index = data.find(key);
    if (key_index == std::string::npos)
    {
        return std::string();
    }

    std::size_t value_index = data.find("=", key_index);
    std::size_t end_index = data.find("&", value_index);
    if (end_index == std::string::npos)
    {
        end_index = data.size();
    }

    std::size_t value_length = end_index - value_index - 1;
    return data.substr(value_index + 1, value_length);
}

// must call free() on the returned string to avoid leaking memory
char* parse_post_data (char* data, char* key)
{
	// the ugly duckling version
	std::string result = parse_post_data(std::string(data), std::string(key));
	char* copy = strdup(result.c_str());
	return copy;
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

char* render_info_page(const char* page_template)
{
	std::string source(page_template);

	// IP address
	char ip_chars[20];
	tcpip_adapter_ip_info_t ipInfo;
	tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);
	snprintf(ip_chars, 20, IPSTR, IP2STR(&ipInfo.ip));
	std::string ip_string(ip_chars);

	// MAC Address
	uint8_t mac[6];
	esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
	std::string mac_string = mac_to_string(mac);

	// Firmware Version
	const esp_app_desc_t *app_desc = esp_ota_get_app_description();
	std::string sw_version(app_desc->version);

	source.replace(source.find("{IPADDRESS}"), std::string("{IPADDRESS}").size(), ip_string);
	source.replace(source.find("{MACADDRESS}"), std::string("{MACADDRESS}").size(), mac_string);
	source.replace(source.find("{SWVERSION}"), std::string("{SWVERSION}").size(), sw_version);

	char* copy = strdup(source.c_str());
	return copy;
}
