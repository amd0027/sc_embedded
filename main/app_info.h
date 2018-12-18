/* Smart Chair Embedded Controller
 * Application ID and Shared Constants Header
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef APP_INFO_H
#define APP_INFO_H

typedef struct app_info_t app_info_t;

struct app_info_t
{
	int major_version;
	int minor_version;

	char compile_data[];
};

#endif /*APP_INFO_H*/
