#ifndef __VERBA_TRACK_H__
#define __VERBA_TRACK_H__

#include <verba/blop.h>

#define mem_free(ptr)                              mem_free_((void*)ptr)
#define mem_malloc(type, size, alias)       (type*)mem_malloc_(size, CONTEXT(alias))
#define mem_calloc(type, count, alias)      (type*)mem_calloc_(count, sizeof(type), CONTEXT(alias))
#define mem_realloc(ptr, type, size, alias) (type*)mem_realloc_(ptr, size, CONTEXT(alias))

void    mem_free_(void* ptr);
void*   mem_malloc_(size_t size, Context context);
void*   mem_calloc_(size_t count, size_t size, Context context);
void*   mem_realloc_(void* ptr, size_t size, Context context);

void    mem_slabstr_free(String* string);
String* mem_slabstr_alloc();

void    mem_init();
void    mem_cleanup();

#endif /* __VERBA_TRACK_H__ */