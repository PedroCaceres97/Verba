#ifndef __BLOP_STRING_H__
#define __BLOP_STRING_H__

#include <blop/blop.h>

#ifndef STRING_NAME
  #define STRING_NAME String
#endif /* STRING_NAME */

#ifndef STRING_FN_PREFIX
  #define STRING_FN_PREFIX STRING_NAME
#endif /* STRING_FN_PREFIX */

#ifndef STRING_RESIZE_POLICIE
  #define STRING_RESIZE_POLICIE(size) (size * 2)
#endif /* STRING_RESIZE_POLICIE */

#ifndef STRING_SHRINK_POLICIE
  #define STRING_SHRINK_POLICIE(capacity) (capacity / 4)
#endif

#if !defined(STRING_INITIAL_SIZE) || STRING_INITIAL_SIZE <= 0
  #define STRING_INITIAL_SIZE 10
#endif /* STRING_INITIAL_SIZE */

/** @cond doxygen_ignore */
#define struct_string         STRING_NAME

#define fn_string_create      CONCAT2(STRING_FN_PREFIX, _create)
#define fn_string_destroy     CONCAT2(STRING_FN_PREFIX, _destroy)

#define fn_string_rdlock      CONCAT2(STRING_FN_PREFIX, _rdlock)
#define fn_string_wrlock      CONCAT2(STRING_FN_PREFIX, _wrlock)
#define fn_string_rdunlock    CONCAT2(STRING_FN_PREFIX, _rdunlock)
#define fn_string_wrunlock    CONCAT2(STRING_FN_PREFIX, _wrunlock)

#define fn_string_cstr        CONCAT2(STRING_FN_PREFIX, _cstr)
#define fn_string_size        CONCAT2(STRING_FN_PREFIX, _size)

#define fn_string_set         CONCAT2(STRING_FN_PREFIX, _set)
#define fn_string_get         CONCAT2(STRING_FN_PREFIX, _get)
#define fn_string_resize      CONCAT2(STRING_FN_PREFIX, _resize)
#define fn_string_shrink      CONCAT2(STRING_FN_PREFIX, _shrink)

#define fn_string_clear       CONCAT2(STRING_FN_PREFIX, _clear)
#define fn_string_erase       CONCAT2(STRING_FN_PREFIX, _erase)
#define fn_string_pop_back    CONCAT2(STRING_FN_PREFIX, _pop_back)
#define fn_string_pop_front   CONCAT2(STRING_FN_PREFIX, _pop_front)

#define fn_string_insert      CONCAT2(STRING_FN_PREFIX, _insert)
#define fn_string_push_back   CONCAT2(STRING_FN_PREFIX, _push_back)
#define fn_string_push_front  CONCAT2(STRING_FN_PREFIX, _push_front)

#define fn_string_strcpy      CONCAT2(STRING_FN_PREFIX, _strcpy)
/** @endcond */

#ifdef __cplusplus
extern "C" {
#endif

struct struct_string;
typedef struct struct_string struct_string;

struct_string*  fn_string_create     (struct_string* str);
void            fn_string_destroy    (struct_string* str);

void            fn_string_rdlock     (struct_string* str);
void            fn_string_wrlock     (struct_string* str);
void            fn_string_rdunlock   (struct_string* str);
void            fn_string_wrunlock   (struct_string* str);

char*           fn_string_cstr       (struct_string* str);
size_t          fn_string_size       (struct_string* str);

void            fn_string_set        (struct_string* str, size_t idx, char c);
char            fn_string_get        (struct_string* str, size_t idx);
void            fn_string_resize     (struct_string* str, size_t size);
void            fn_string_shrink     (struct_string* str);

void            fn_string_clear      (struct_string* str);
void            fn_string_erase      (struct_string* str, size_t idx);
void            fn_string_pop_back   (struct_string* str);
void            fn_string_pop_front  (struct_string* str);

void            fn_string_insert     (struct_string* str, size_t idx, char c);
void            fn_string_push_back  (struct_string* str, char c);
void            fn_string_push_front (struct_string* str, char c);

void            fn_string_strcpy     (struct_string* str, size_t idx, const char* src, size_t count);

#if (defined(STRING_STRUCT) || defined(STRING_IMPLEMENTATION)) && !defined(STRING_NOT_STRUCT)
  struct struct_string {
    int               allocated;
    char*             data;
    size_t            size;
    size_t            capacity;
    RWLOCK_TYPE  lock;
  };
#endif /* (defined(STRING_STRUCT) || defined(STRING_IMPLEMENTATION)) && !defined(STRING_NOT_STRUCT) */

#ifdef STRING_IMPLEMENTATION

struct_string*  fn_string_create(struct_string* str) {
  if (!str) {
    CALLOC(str, struct struct_string, 1);
    str->allocated = true;
  } else {
    str->allocated = false;
  }

  str->size = 0;
  str->capacity = STRING_INITIAL_SIZE;
  RWLOCK_INIT(str->lock);
  CALLOC(str->data, char, str->capacity + 1);

  return str;
}
void            fn_string_destroy(struct_string* str) {
  BLOP_ASSERT_PTR(str);

  FREE(str->data);
  RWLOCK_DESTROY(str->lock);

  if (str->allocated) {
    FREE(str);
  }
}

void            fn_string_rdlock(struct_string* str) {
  BLOP_ASSERT_PTR(str);
  RWLOCK_RDLOCK(str->lock);
}
void            fn_string_wrlock(struct_string* str) {
  BLOP_ASSERT_PTR(str);
  RWLOCK_WRLOCK(str->lock);
}
void            fn_string_rdunlock(struct_string* str) {
  BLOP_ASSERT_PTR(str);
  RWLOCK_RDUNLOCK(str->lock);
}
void            fn_string_wrunlock(struct_string* str) {
  BLOP_ASSERT_PTR(str);
  RWLOCK_WRUNLOCK(str->lock);
}

char*           fn_string_cstr(struct_string* str) {
  BLOP_ASSERT_PTR(str);
  return str->data;
}
size_t          fn_string_size(struct_string* str) {
  BLOP_ASSERT_PTR(str);
  return str->size;
}

void            fn_string_set(struct_string* str, size_t idx, char c) {
  BLOP_ASSERT_PTR(str);

  BLOP_ASSERT_BOUNDS(idx, str->size);

  str->data[idx] = c;
}
char            fn_string_get(struct_string* str, size_t idx) {
  BLOP_ASSERT_PTR(str);

  BLOP_ASSERT_BOUNDS(idx, str->size);

  return str->data[idx];
}
void            fn_string_resize(struct_string* str, size_t size) {
  BLOP_ASSERT_PTR(str);

  if (size == str->size) {
    return;
  }

  if (size == 0) {
    fn_string_clear(str);
    return;
  }

  str->capacity = TERNARY(
    size < STRING_INITIAL_SIZE,
    STRING_INITIAL_SIZE,
    STRING_RESIZE_POLICIE(size)
  );

  char* data = NULL;
  CALLOC(data, char, str->capacity + 1);
  memcpy(data, str->data, MIN(str->size, size));

  FREE(str->data);
  str->data = data;
  str->size = size;
  str->data[str->size] = '\0';
}
void            fn_string_shrink(struct_string* str) {
  BLOP_ASSERT_PTR(str);

  if (str->size < STRING_SHRINK_POLICIE(str->capacity) && str->size < STRING_INITIAL_SIZE) {
    str->capacity = STRING_RESIZE_POLICIE(str->size);

    char* data = NULL;
    CALLOC(data, char, str->capacity + 1);
    memcpy(data, str->data, str->size);

    FREE(str->data);
    str->data = data;
    str->data[str->size] = '\0';
  }
}

void            fn_string_clear(struct_string* str) {
  BLOP_ASSERT_PTR(str);

  str->size = 0;
  str->capacity = STRING_INITIAL_SIZE;
  FREE(str->data);
  CALLOC(str->data, char, str->capacity + 1);
}
void            fn_string_erase(struct_string* str, size_t idx) {
  BLOP_ASSERT_PTR(str);

  BLOP_ASSERT_BOUNDS(idx, str->size);      

  if (idx != str->size - 1) {
    memmove(&str->data[idx], &str->data[idx + 1], (str->size - idx - 1));
  }
  
  str->size--;
  str->data[str->size] = '\0';
  fn_string_shrink(str);
}
void            fn_string_pop_back(struct_string* str) {
  BLOP_ASSERT_PTR(str);

  if (str->size == 0) {
    EMPTY_POPPING();
    return;
  }

  fn_string_erase(str, str->size - 1);
}
void            fn_string_pop_front(struct_string* str) {
  BLOP_ASSERT_PTR(str);

  if (str->size == 0) {
    EMPTY_POPPING();
    return;
  }

  fn_string_erase(str, 0);
}

void            fn_string_insert(struct_string* str, size_t idx, char c) {
  BLOP_ASSERT_PTR(str);

  BLOP_ASSERT_BOUNDS(idx, str->size + 1);

  if (str->size != str->capacity) {
    if (idx != str->size) {
      memmove(&str->data[idx + 1], &str->data[idx], (str->size - idx));
    }
  } else {
    str->capacity = STRING_RESIZE_POLICIE(str->size);
    char* data = NULL;
    CALLOC(data, char, str->capacity + 1);

    if (idx != 0) {
      memcpy(data, str->data, idx);
    }
    
    if (idx != str->size) {
      memcpy(&data[idx + 1], &str->data[idx], (str->size - idx));
    }

    FREE(str->data);
    str->data = data;
  }

  str->size++;
  str->data[idx] = c;
  str->data[str->size] = '\0';
}
void            fn_string_push_back(struct_string* str, char c) {
  BLOP_ASSERT_PTR(str);

  fn_string_insert(str, str->size, c);
}
void            fn_string_push_front(struct_string* str, char c) {
  BLOP_ASSERT_PTR(str);

  fn_string_insert(str, 0, c);
}

void            fn_string_strcpy(struct_string* str, size_t idx, const char* src, size_t count) {
  BLOP_ASSERT_PTR(str);
  BLOP_ASSERT_PTR(src);

  if (count == 0) { return; }

  BLOP_ASSERT_BOUNDS(idx, str->size);
  BLOP_ASSERT_BOUNDS(idx + count, str->size + 1);

  memcpy(&str->data[idx], src, count);
}

#endif /* STRING_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* __BLOP_STRING_H__ */