 /* Smart Chair Embedded Controller
 * Active Page Rendering Logic Header
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef WEBRENDER_H
#define WEBRENDER_H

#include <string>

namespace webrender
{
	std::string ParsePostData(char* sourceData, std::string key);
	std::string RenderInfoPage(const char* pageTemplate);
	std::string RenderPairingPage(const char* pageTemplate, std::string pairingKey);
}


#endif /*WEBRENDER_H*/
