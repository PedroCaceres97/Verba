#include <blop/blop.h>

#ifndef SLAB_NAME
  #define SLAB_NAME slab_int
#endif /* SLAB_NAME */

#ifndef SLAB_FN_PREFIX
  #define SLAB_FN_PREFIX SLAB_NAME
#endif /* SLAB_FN_PREFIX */

#ifndef SLAB_DATA_TYPE
  #define SLAB_DATA_TYPE int
#endif /* SLAB_DATA_TYPE */

#ifndef SLAB_OBJECTS_COUNT
  #define SLAB_OBJECTS_COUNT 1024
#endif

#define struct_slab       SLAB_NAME
#define struct_block      CONCAT2(SLAB_NAME, _block)

#define fn_block_create   CONCAT2(SLAB_FN_PREFIX, _block_create)
#define fn_block_destroy  CONCAT2(SLAB_FN_PREFIX, _block_destroy)
#define fn_block_clear    CONCAT2(SLAB_FN_PREFIX, _block_clear)

#define fn_slab_create    CONCAT2(SLAB_FN_PREFIX, _create)
#define fn_slab_destroy   CONCAT2(SLAB_FN_PREFIX, _destroy)

#define fn_slab_rdlock    CONCAT2(SLAB_FN_PREFIX, _rdlock)
#define fn_slab_wrlock    CONCAT2(SLAB_FN_PREFIX, _wrlock)
#define fn_slab_rdunlock  CONCAT2(SLAB_FN_PREFIX, _rdunlock)
#define fn_slab_wrunlock  CONCAT2(SLAB_FN_PREFIX, _wrunlock)

#define fn_slab_clear     CONCAT2(SLAB_FN_PREFIX, _clear)
#define fn_slab_free      CONCAT2(SLAB_FN_PREFIX, _free)
#define fn_slab_alloc     CONCAT2(SLAB_FN_PREFIX, _alloc)

#define fn_slab_size      CONCAT2(SLAB_FN_PREFIX, _size)
#define fn_slab_print     CONCAT2(SLAB_FN_PREFIX, _print)

#ifdef __cplusplus
extern "C" {
#endif

struct struct_slab;
struct struct_block;
typedef struct struct_slab struct_slab;
typedef struct struct_block struct_block;

struct_block*   fn_block_create (struct_block* block);
void            fn_block_destroy(struct_block* block);

struct_slab*    fn_slab_create  (struct_slab* slab);
void            fn_slab_destroy (struct_slab* slab);

void            fn_slab_rdlock  (struct_slab* slab);
void            fn_slab_wrlock  (struct_slab* slab);
void            fn_slab_rdunlock(struct_slab* slab);
void            fn_slab_wrunlock(struct_slab* slab);

void            fn_slab_clear   (struct_slab* slab);
void            fn_slab_free    (struct_slab* slab, SLAB_DATA_TYPE* ptr);
SLAB_DATA_TYPE* fn_slab_alloc   (struct_slab* slab);

size_t          fn_slab_size    (struct_slab* slab);
void            fn_slab_print   (struct_slab* slab);

#if (defined(SLAB_STRUCT) || defined(SLAB_IMPLEMENTATION)) && !defined(SLAB_NOT_STRUCT)
  struct struct_block {
    int                   allocated;
    SLAB_DATA_TYPE        mem[SLAB_OBJECTS_COUNT];
    SLAB_DATA_TYPE*       free_list[SLAB_OBJECTS_COUNT];
    size_t                free_count;
    char                  allocated_list[SLAB_OBJECTS_COUNT];
    struct struct_block*  next;
  };

  struct struct_slab {
    int                   allocated;
    struct struct_block   block;
    size_t                total;
    RWLOCK_TYPE           lock;
  };
#endif /* (defined(SLAB_STRUCT) || defined(SLAB_IMPLEMENTATION)) && !defined(SLAB_NOT_STRUCT) */

#ifdef SLAB_IMPLEMENTATION

struct_block*   fn_block_create(struct_block* block) {
  if (!block) {
    CALLOC(block, struct_block, 1);
    block->allocated = true;
  } else {
    block->allocated = false;
  }

  block->free_count = SLAB_OBJECTS_COUNT;
  for (size_t i = 0; i < SLAB_OBJECTS_COUNT; i++) {
    block->free_list[i] = &block->mem[SLAB_OBJECTS_COUNT - i - 1];
  }

  return block;
}
void            fn_block_destroy(struct_block* block) {
  if (block->allocated) {
    FREE(block);
  }
}

struct_slab*    fn_slab_create(struct_slab* slab) {
  if (!slab) {
    CALLOC(slab, struct_slab, 1);
    slab->allocated = true;
  } else {
    slab->allocated = false;
  }

  RWLOCK_INIT(slab->lock);

  fn_block_create(&slab->block);

  return slab;
}
void            fn_slab_destroy(struct_slab* slab) {
  BLOP_ASSERT_PTR(slab);

  BLOP_ASSERT_FORCED(slab->total == 0, "Trying to free a non empty slab");

  struct_block* current = &slab->block;
  while (current) {
    struct_block* next = current->next;
    fn_block_destroy(current);
    current = next;
  }
}

void            fn_slab_rdlock(struct_slab* slab) {
  BLOP_ASSERT_PTR(slab);
  RWLOCK_RDLOCK(slab->lock);
}
void            fn_slab_wrlock(struct_slab* slab) {
  BLOP_ASSERT_PTR(slab);
  RWLOCK_WRLOCK(slab->lock);
}
void            fn_slab_rdunlock(struct_slab* slab) {
  BLOP_ASSERT_PTR(slab);
  RWLOCK_RDUNLOCK(slab->lock);
}
void            fn_slab_wrunlock(struct_slab* slab) {
  BLOP_ASSERT_PTR(slab);
  RWLOCK_WRUNLOCK(slab->lock);
}

void            fn_slab_clear(struct_slab* slab) {
  BLOP_ASSERT_PTR(slab);

  struct_block* current = &slab->block;
  while (current) {
    #ifdef SLAB_DEALLOCATE_DATA
      for (size_t i = 0; i < SLAB_OBJECTS_COUNT; i++) {
        if (current->allocated_list[i]) {
          SLAB_DEALLOCATE_DATA(&current->mem[i]);
          current->allocated_list[i] = false;
        }
      }
    #endif /* SLAB_DEALLOCATE_DATA */

    memset(current->allocated_list, 0, SLAB_OBJECTS_COUNT);
    memset(current->mem, 0, SLAB_OBJECTS_COUNT * sizeof(SLAB_DATA_TYPE));
    current->free_count = SLAB_OBJECTS_COUNT;
    for (size_t i = 0; i < SLAB_OBJECTS_COUNT; i++) {
      current->free_list[i] = &current->mem[SLAB_OBJECTS_COUNT - i - 1];
    }

    struct_block* next = current->next;
    fn_block_destroy(current);
    current = next;
  }
}
void            fn_slab_free(struct_slab* slab, SLAB_DATA_TYPE* ptr) {
  BLOP_ASSERT_PTR(slab);
  BLOP_ASSERT_PTR(ptr);

  struct_block* current = &slab->block;
  while (current) {
    if (ptr >= &current->mem[0] && ptr <= &current->mem[SLAB_OBJECTS_COUNT - 1]) {
      BLOP_ASSERT_FORCED(current->allocated_list[(size_t)(ptr - current->mem)], "Ptr belongs to slab but was not allocated");

      #ifdef SLAB_DEALLOCATE_DATA
        SLAB_DEALLOCATE_DATA(ptr);
      #endif /* SLAB_DEALLOCATE_DATA */

      current->allocated_list[(size_t)(ptr - current->mem)] = false;
      current->free_list[current->free_count] = ptr;
      current->free_count++;
      slab->total--;
      return;
    }

    current = current->next;
  }

  BLOP_ASSERT_FORCED(false, "Trying to free a foreign ptr");
}
SLAB_DATA_TYPE* fn_slab_alloc(struct_slab* slab) {
  BLOP_ASSERT_PTR(slab);

  struct_block* current = &slab->block;
  while (current) {
    if (current->free_count > 0) {
      slab->total++;
      current->free_count--;
      SLAB_DATA_TYPE* ptr = current->free_list[current->free_count];
      current->allocated_list[(size_t)(ptr - current->mem)] = true;
      return ptr;
    }

    if (current->next == NULL) {
      current->next = fn_block_create(NULL);
    }
    current = current->next;
  }

  return NULL; /* Never reaching here */
}

size_t          fn_slab_size(struct_slab* slab) {
  BLOP_ASSERT_PTR(slab);
  return slab->total;
}
void            fn_slab_print(struct_slab* slab) {
  BLOP_ASSERT_PTR(slab);
  LOG_STDOUT("Slab Information:\n Allocated pointers: %zu\n Total bytes allocated: %zu\n\n", slab->total, slab->total * sizeof(SLAB_DATA_TYPE));
}

#endif /* SLAB_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#undef SLAB_NAME
#undef SLAB_FN_PREFIX

#undef SLAB_DATA_TYPE
#undef SLAB_OBJECTS_COUNT

#undef SLAB_STRUCT
#undef SLAB_NOT_STRUCT
#undef SLAB_IMPLEMENTATION

#define struct_slab

#define fn_slab_create
#define fn_slab_destroy

#define fn_slab_rdlock
#define fn_slab_wrlock
#define fn_slab_rdunlock
#define fn_slab_wrunlock

#define fn_slab_clear
#define fn_slab_free
#define fn_slab_alloc
#define fn_slab_size