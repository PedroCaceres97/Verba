#ifndef __VERBA_IA_H__
#define __VERBA_IA_H__

#include <curl/curl.h>
#include <cJSON/cJSON.h>

void        ai_init(void);
void        ai_cleanup(void);

void        ai_message(const char* message);
const char* ai_last(void);

void        ai_attach_disk(const char* disk);
void        ai_dettach_disk(void);

#endif /* __VERBA_IA_H__ */
