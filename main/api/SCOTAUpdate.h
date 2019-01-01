/* Smart Chair Embedded Controller
 * Secure OTA Update Controller Header
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef HTTPS_OTA_H
#define HTTPS_OTA_H

class SCOTAUpdate
{
public:
	static void RunUpdateCheck();

private:
	static void PerformOTAUpdate(char* update_url);
	static char* GetUpdateUrl();

	static constexpr const char* TAG = "ota_update";
};

#endif /*HTTPS_OTA_H*/
