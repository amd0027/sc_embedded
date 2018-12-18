/* Smart Chair Embedded Controller
 * Active Page Rendering Logic Header
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef WEBRENDER_H
#define WEBRENDER_H

#ifdef __cplusplus
extern "C" {
#endif

char* render_info_page(const char* page_template);
char* parse_post_data (char* data, char* key);

#ifdef __cplusplus
}
#endif

#endif /*WEBRENDER_H*/
