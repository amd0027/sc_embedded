/* Smart Chair Embedded Controller
 * Web Administration Server
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef SCWEBSERVER_H
#define SCWEBSERVER_H

#include <esp_http_server.h>

#include "SCWifi.h"

class SCWebServer
{
public:
	SCWebServer();
	void Start();
	void Stop();

private:


private:
	httpd_handle_t server;

	static constexpr const char* TAG = "webserver";
};


#endif /* SCWEBSERVER_H */
