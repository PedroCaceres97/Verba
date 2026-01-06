#include <verba/mem.h>

#ifndef NDEBUG
  static Tracker tracker;
#else
  static size_t allocations;
#endif

static SLABstr slabstr;

void    mem_free_(void* ptr) {
  #ifndef NDEBUG
    tracker_free(&tracker, ptr);
  #else
    FREE(ptr);
    allocations--;
  #endif
}
void*   mem_malloc_(size_t size, Context context) {
  #ifndef NDEBUG
    return tracker_alloc(&tracker, context, size);
  #else
    void* ptr = NULL;
    MALLOC(ptr, void, size);
    allocations++;
    return ptr;
  #endif
}
void*   mem_calloc_(size_t count, size_t size, Context context) {
  #ifndef NDEBUG
    return tracker_alloc(&tracker, context, size * count);
  #else
    void* ptr = NULL;
    void* ptr = NULL;
    MALLOC(ptr, void, size * count);
    allocations++;
    return ptr;
  #endif
}
void*   mem_realloc_(void* ptr, size_t size, Context context) {
  #ifndef NDEBUG
    return tracker_realloc(&tracker, context, ptr, size);
  #else
    REALLOC(ptr, void, size);
    return ptr;
  #endif
}

void    mem_string_free(String* string) {
  string_destroy(string);
  slabstr_free(&slabstr, string);
}
String* mem_string_alloc(void) {
  String* string = slabstr_alloc(&slabstr);
  string_create(string);
  return string;
}

void    mem_init(void) {
#ifndef NDEBUG
  tracker_create(&tracker, CONTEXT("Verba Memory Tracker"));
#else
  allocations = 0;
#endif

  slabstr_create(&slabstr);

  LOG_SUCCESS("Mem initialized");
}
void    mem_cleanup(void) {
#ifndef NDEBUG
  if (tracker.bytes != 0) {
    LOG_FATAL("Memory leaks detected (Debug)");
    tracker_print_err(&tracker);
    tracker_clear(&tracker);
    ABORT();
  }

  tracker_destroy(&tracker);
#else
  ASSERT_FORCED(allocations == 0, "Memory leaks detected (Release)");
#endif

  if (slabstr.total > 0) {
    LOG_FATAL("Memory leaks detected (Slab String)");
    slabstr_print_err(&slabstr);
    slabstr_clear(&slabstr);
    ABORT();
  }
  slabstr_destroy(&slabstr);

  LOG_SUCCESS("Mem terminated");
}
