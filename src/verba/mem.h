#ifndef __VERBA_TRACK_H__
#define __VERBA_TRACK_H__

#include <verba/blop.h>

#define mem_free(ptr)                       mem_free_((void*)ptr)
#define mem_malloc(type, size)       (type*)mem_malloc_(size, CONTEXT(#type"*"))
#define mem_calloc(type, count)      (type*)mem_calloc_(count, sizeof(type), CONTEXT(#type))
#define mem_realloc(ptr, type, size) (type*)mem_realloc_(ptr, size, CONTEXT(#type"*"))

void    mem_free_(void* ptr);
void*   mem_malloc_(size_t size, Context context);
void*   mem_calloc_(size_t count, size_t size, Context context);
void*   mem_realloc_(void* ptr, size_t size, Context context);

void    mem_string_free(String* string);
String* mem_string_alloc(void);

void    mem_init(void);
void    mem_cleanup(void);

#endif /* __VERBA_TRACK_H__ */
