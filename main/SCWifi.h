/* Smart Chair Embedded Controller
 * WiFi Controller Header
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef SCWIFI_H
#define SCWIFI_H

#include "SCSettings.h"

class SCWifi
{
public:
	SCWifi(SCSettings& config);
	void ConnectSTAWifi(bool delayInit);
	void ConnectSoftAPWifi(bool delayInit);

private:
	SCSettings& configuration;

	static constexpr const char* TAG = "wifi";
};



#endif /*SCWIFI_H*/
