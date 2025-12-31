#include <blop/blop.h>

#ifndef VECTOR_NAME
  #define VECTOR_NAME Vecint
#endif /* VECTOR_NAME */

#ifndef VECTOR_FN_PREFIX
  #define VECTOR_FN_PREFIX VECTOR_NAME
#endif /* VECTOR_FN_PREFIX */

#ifndef VECTOR_DATA_TYPE
  #define VECTOR_DATA_TYPE int
#endif /* VECTOR_DATA_TYPE */

#ifndef VECTOR_RESIZE_POLICIE
  #define VECTOR_RESIZE_POLICIE(size) (size * 2)
#endif /* VECTOR_RESIZE_POLICIE */

#ifndef VECTOR_SHRINK_POLICIE
  #define VECTOR_SHRINK_POLICIE(capacity) (capacity / 4)
#endif /* VECTOR_SHRINK_POLICIE */

#if !defined(VECTOR_INITIAL_SIZE) || VECTOR_INITIAL_SIZE <= 0
  #define VECTOR_INITIAL_SIZE 10
#endif /* VECTOR_INITIAL_SIZE */

/** @cond doxygen_ignore */
#define struct_vector         VECTOR_NAME

#define fn_vector_create      CONCAT2(VECTOR_FN_PREFIX, _create)
#define fn_vector_destroy     CONCAT2(VECTOR_FN_PREFIX, _destroy)

#define fn_vector_rdlock      CONCAT2(VECTOR_FN_PREFIX, _rdlock)
#define fn_vector_wrlock      CONCAT2(VECTOR_FN_PREFIX, _wrlock)
#define fn_vector_rdunlock    CONCAT2(VECTOR_FN_PREFIX, _rdunlock)
#define fn_vector_wrunlock    CONCAT2(VECTOR_FN_PREFIX, _wrunlock)

#define fn_vector_data        CONCAT2(VECTOR_FN_PREFIX, _data)
#define fn_vector_size        CONCAT2(VECTOR_FN_PREFIX, _size)
#define fn_vector_back        CONCAT2(VECTOR_FN_PREFIX, _back)
#define fn_vector_front       CONCAT2(VECTOR_FN_PREFIX, _front)

#define fn_vector_set         CONCAT2(VECTOR_FN_PREFIX, _set)
#define fn_vector_get         CONCAT2(VECTOR_FN_PREFIX, _get)
#define fn_vector_resize      CONCAT2(VECTOR_FN_PREFIX, _resize)
#define fn_vector_shrink      CONCAT2(VECTOR_FN_PREFIX, _shrink)

#define fn_vector_clear       CONCAT2(VECTOR_FN_PREFIX, _clear)
#define fn_vector_erase       CONCAT2(VECTOR_FN_PREFIX, _erase)
#define fn_vector_pop_back    CONCAT2(VECTOR_FN_PREFIX, _pop_back)
#define fn_vector_pop_front   CONCAT2(VECTOR_FN_PREFIX, _pop_front)

#define fn_vector_insert      CONCAT2(VECTOR_FN_PREFIX, _insert)
#define fn_vector_push_back   CONCAT2(VECTOR_FN_PREFIX, _push_back)
#define fn_vector_push_front  CONCAT2(VECTOR_FN_PREFIX, _push_front)

#define fn_vector_memcpy      CONCAT2(VECTOR_FN_PREFIX, _memcpy)
#define fn_vector_memset      CONCAT2(VECTOR_FN_PREFIX, _memset)
/** @endcond */

#ifdef __cplusplus
extern "C" {
#endif

struct struct_vector;
typedef struct struct_vector struct_vector;

struct_vector*    fn_vector_create    (struct_vector* vec);
void              fn_vector_destroy   (struct_vector* vec);

void              fn_vector_rdlock    (struct_vector* vec);
void              fn_vector_wrlock    (struct_vector* vec);
void              fn_vector_rdunlock  (struct_vector* vec);
void              fn_vector_wrunlock  (struct_vector* vec);

VECTOR_DATA_TYPE* fn_vector_data      (struct_vector* vec);
size_t            fn_vector_size      (struct_vector* vec);
VECTOR_DATA_TYPE  fn_vector_back      (struct_vector* vec);
VECTOR_DATA_TYPE  fn_vector_front     (struct_vector* vec);

void              fn_vector_set       (struct_vector* vec, size_t idx,        VECTOR_DATA_TYPE value);
VECTOR_DATA_TYPE  fn_vector_get       (struct_vector* vec, size_t idx);
void              fn_vector_resize    (struct_vector* vec, size_t size);
void              fn_vector_shrink    (struct_vector* vec);

void              fn_vector_clear     (struct_vector* vec);
void              fn_vector_erase     (struct_vector* vec, size_t idx);
void              fn_vector_pop_back  (struct_vector* vec);
void              fn_vector_pop_front (struct_vector* vec);

void              fn_vector_insert    (struct_vector* vec, size_t idx,        VECTOR_DATA_TYPE value);
void              fn_vector_push_back (struct_vector* vec,                    VECTOR_DATA_TYPE value);
void              fn_vector_push_front(struct_vector* vec,                    VECTOR_DATA_TYPE value);

void              fn_vector_memcpy    (struct_vector* vec, size_t idx, const  VECTOR_DATA_TYPE* src,  size_t count);
void              fn_vector_memset    (struct_vector* vec, size_t idx,        VECTOR_DATA_TYPE value, size_t count);

#if (defined(VECTOR_STRUCT) || defined(VECTOR_IMPLEMENTATION)) && !defined(VECTOR_NOT_STRUCT)
  struct struct_vector {
    VECTOR_DATA_TYPE* data;
    size_t            size;
    size_t            capacity;
    int               allocated;
    RWLOCK_TYPE  lock;
  };
#endif /* (defined(VECTOR_STRUCT) || defined(VECTOR_IMPLEMENTATION)) && !defined(VECTOR_NOT_STRUCT) */

#ifdef VECTOR_IMPLEMENTATION

struct_vector*    fn_vector_create(struct_vector* vec) {
  if (!vec) {
    CALLOC(vec, struct struct_vector, 1);
    vec->allocated = true;
  } else {
    vec->allocated = false;
  }

  vec->size = 0;
  vec->capacity = VECTOR_INITIAL_SIZE;
  CALLOC(vec->data, VECTOR_DATA_TYPE, vec->capacity);
  RWLOCK_INIT(vec->lock);

  return vec;
}
void              fn_vector_destroy(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);

  BLOP_ASSERT(vec->size == 0, "Destroying non empty vector (HINT: Clear the vector)");

  RWLOCK_DESTROY(vec->lock);
  FREE(vec->data);

  if (vec->allocated) {
    FREE(vec);
  }
}

void              fn_vector_rdlock(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);
  RWLOCK_RDLOCK(vec->lock);
}
void              fn_vector_wrlock(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);
  RWLOCK_WRLOCK(vec->lock);
}
void              fn_vector_rdunlock(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);
  RWLOCK_RDUNLOCK(vec->lock);
}
void              fn_vector_wrunlock(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);
  RWLOCK_WRUNLOCK(vec->lock);
}

VECTOR_DATA_TYPE* fn_vector_data(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);
  return vec->data;
}
size_t            fn_vector_size(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);
  return vec->size;
}
VECTOR_DATA_TYPE  fn_vector_back(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);
  
  BLOP_ASSERT_FORCED(vec->size != 0, "Vector has no back (size == 0)");
  return vec->data[vec->size - 1];
}
VECTOR_DATA_TYPE  fn_vector_front(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);
  
  BLOP_ASSERT_FORCED(vec->size != 0, "Vector has no front (size == 0)");
  return vec->data[0];
}

void              fn_vector_set(struct_vector* vec, size_t idx, VECTOR_DATA_TYPE value) {
  BLOP_ASSERT_PTR(vec);

  BLOP_ASSERT_BOUNDS(idx, vec->size);
  vec->data[idx] = value;
}
VECTOR_DATA_TYPE  fn_vector_get(struct_vector* vec, size_t idx) {
  BLOP_ASSERT_PTR(vec);

  BLOP_ASSERT_BOUNDS(idx, vec->size);
  return vec->data[idx];
}
void              fn_vector_resize(struct_vector* vec, size_t size) {
  BLOP_ASSERT_PTR(vec);

  if (size == vec->size) {
    return;
  }

  if (size == 0) {
    fn_vector_clear(vec);
    return;
  }

  if (size < vec->size) {
    #ifdef VECTOR_DEALLOCATE_DATA
      for (size_t i = size; i < vec->size; i++) {
        VECTOR_DEALLOCATE_DATA(vec->data[i]);
      }
    #endif /* VECTOR_DEALLOCATE_DATA */
  }

  vec->capacity = TERNARY(
    size < VECTOR_INITIAL_SIZE,
    VECTOR_INITIAL_SIZE,
    VECTOR_RESIZE_POLICIE(size)
  );

  VECTOR_DATA_TYPE* data = NULL;
  CALLOC(data, VECTOR_DATA_TYPE, vec->capacity);
  memcpy(data, vec->data, MIN(vec->size, size) * sizeof(VECTOR_DATA_TYPE));

  FREE(vec->data);
  vec->data = data;
  vec->size = size;
}
void              fn_vector_shrink(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);

  if (vec->size < VECTOR_SHRINK_POLICIE(vec->capacity) && vec->size < VECTOR_INITIAL_SIZE) {
    vec->capacity = VECTOR_RESIZE_POLICIE(vec->size);
    VECTOR_DATA_TYPE* data = NULL;
    CALLOC(data, VECTOR_DATA_TYPE, vec->capacity);

    memcpy(data, vec->data, vec->size * sizeof(VECTOR_DATA_TYPE));
    FREE(vec->data);
    vec->data = data;
  }
}

void              fn_vector_clear(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);

  #ifdef VECTOR_DEALLOCATE_DATA
    for (size_t i = 0; i < vec->size; i++) {
      VECTOR_DEALLOCATE_DATA(vec->data[i]);
    }
  #endif /* VECTOR_DEALLOCATE_DATA */

  vec->size = 0;
  vec->capacity = VECTOR_INITIAL_SIZE;
  FREE(vec->data);
  CALLOC(vec->data, VECTOR_DATA_TYPE, vec->capacity);
}
void              fn_vector_erase(struct_vector* vec, size_t idx) {
  BLOP_ASSERT_PTR(vec);

  BLOP_ASSERT_BOUNDS(idx, vec->size);

  #ifdef VECTOR_DEALLOCATE_DATA
    VECTOR_DEALLOCATE_DATA(vec->data[idx]);
  #endif /* VECTOR_DEALLOCATE_DATA */           

  if (idx != vec->size - 1) {
    memmove(&vec->data[idx], &vec->data[idx + 1], (vec->size - idx - 1) * sizeof(VECTOR_DATA_TYPE));
  }
  
  vec->size--;

  fn_vector_shrink(vec);
}
void              fn_vector_pop_back(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);

  if (vec->size == 0) {
    EMPTY_POPPING();
    return;
  }

  fn_vector_erase(vec, vec->size - 1);
}
void              fn_vector_pop_front(struct_vector* vec) {
  BLOP_ASSERT_PTR(vec);

  if (vec->size == 0) {
    EMPTY_POPPING();
    return;
  }

  fn_vector_erase(vec, 0);
}

void              fn_vector_insert(struct_vector* vec, size_t idx, VECTOR_DATA_TYPE value) {
  BLOP_ASSERT_PTR(vec);

  BLOP_ASSERT_BOUNDS(idx, vec->size + 1);

  if (vec->size != vec->capacity) {
    if (idx != vec->size) {
      memmove(&vec->data[idx + 1], &vec->data[idx], (vec->size - idx) * sizeof(VECTOR_DATA_TYPE));
    }
  } else {
    vec->capacity = VECTOR_RESIZE_POLICIE(vec->size);
    VECTOR_DATA_TYPE* data = NULL;
    CALLOC(data, VECTOR_DATA_TYPE, vec->capacity);

    if (idx != 0) {
      memcpy(data, vec->data, idx * sizeof(VECTOR_DATA_TYPE));
    }
    
    if (idx != vec->size) {
      memcpy(&data[idx + 1], &vec->data[idx], (vec->size - idx) * sizeof(VECTOR_DATA_TYPE));
    }

    FREE(vec->data);
    vec->data = data;
  }

  vec->data[idx] = value;
  vec->size++;
}
void              fn_vector_push_back(struct_vector* vec, VECTOR_DATA_TYPE value) {
  BLOP_ASSERT_PTR(vec);

  fn_vector_insert(vec, vec->size, value);
}
void              fn_vector_push_front(struct_vector* vec, VECTOR_DATA_TYPE value) {
  BLOP_ASSERT_PTR(vec);

  fn_vector_insert(vec, 0, value);
}

void              fn_vector_memcpy(struct_vector* vec, size_t idx, const VECTOR_DATA_TYPE* src, size_t count) {
  BLOP_ASSERT_PTR(vec);
  BLOP_ASSERT_PTR(src);

  if (count == 0) { return; }

  BLOP_ASSERT_BOUNDS(idx, vec->size);
  BLOP_ASSERT_BOUNDS(idx + count, vec->size + 1);

  memcpy(&vec->data[idx], src, count * sizeof(VECTOR_DATA_TYPE));
}
void              fn_vector_memset(struct_vector* vec, size_t idx, VECTOR_DATA_TYPE value, size_t count) {
  BLOP_ASSERT_PTR(vec);

  if (count == 0) { return; }

  BLOP_ASSERT_BOUNDS(idx, vec->size);
  BLOP_ASSERT_BOUNDS(idx + count, vec->size + 1);

  for (size_t i = 0; i < count; i++) {
    vec->data[idx + i] = value;
  }
}

#endif /* VECTOR_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#undef VECTOR_NAME
#undef VECTOR_FN_PREFIX

#undef VECTOR_DATA_TYPE
#undef VECTOR_INITIAL_SIZE
#undef VECTOR_SHRINK_POLICIE
#undef VECTOR_RESIZE_POLICIE
#undef VECTOR_DEALLOCATE_DATA

#undef VECTOR_STRUCT
#undef VECTOR_NOT_STRUCT
#undef VECTOR_IMPLEMENTATION
 
#undef struct_vector

#undef fn_vector_create    
#undef fn_vector_destroy

#undef fn_vector_rdlock    
#undef fn_vector_wrlock    
#undef fn_vector_rdunlock  
#undef fn_vector_wrunlock

#undef fn_vector_data      
#undef fn_vector_size      
#undef fn_vector_back      
#undef fn_vector_front

#undef fn_vector_set       
#undef fn_vector_get       
#undef fn_vector_resize    
#undef fn_vector_shrink

#undef fn_vector_clear     
#undef fn_vector_erase     
#undef fn_vector_pop_back  
#undef fn_vector_pop_front

#undef fn_vector_insert    
#undef fn_vector_push_back 
#undef fn_vector_push_front

#undef fn_vector_memcpy    
#undef fn_vector_memset