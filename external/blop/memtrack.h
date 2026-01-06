#ifndef __BLOP_MEMTRACK_H__
#define __BLOP_MEMTRACK_H__ 

#include <blop/blop.h>

#ifndef MEMTRACK_NAME
  #define MEMTRACK_NAME Memtrack
#endif /* MEMTRACK_NAME */

#ifndef MEMTRACK_FN_PREFIX
  #define MEMTRACK_FN_PREFIX MEMTRACK_NAME
#endif /* MEMTRACK_FN_PREFIX */

#define struct_memtrack       MEMTRACK_NAME
#define struct_ptrhdr         CONCAT2(struct_memtrack, _ptrhdr)

#define fn_memtrack_create    CONCAT2(MEMTRACK_FN_PREFIX, _create)
#define fn_memtrack_destroy   CONCAT2(MEMTRACK_FN_PREFIX, _destroy)

#define fn_memtrack_rdlock    CONCAT2(MEMTRACK_FN_PREFIX, _rdlock)
#define fn_memtrack_wrlock    CONCAT2(MEMTRACK_FN_PREFIX, _wrlock)
#define fn_memtrack_rdunlock  CONCAT2(MEMTRACK_FN_PREFIX, _rdunlock)
#define fn_memtrack_wrunlock  CONCAT2(MEMTRACK_FN_PREFIX, _wrunlock)

#define fn_memtrack_clear     CONCAT2(MEMTRACK_FN_PREFIX, _clear)
#define fn_memtrack_free      CONCAT2(MEMTRACK_FN_PREFIX, _free)
#define fn_memtrack_alloc     CONCAT2(MEMTRACK_FN_PREFIX, _alloc)
#define fn_memtrack_realloc   CONCAT2(MEMTRACK_FN_PREFIX, _realloc)
#define fn_memtrack_duplicate CONCAT2(MEMTRACK_FN_PREFIX, _duplicate)

#define fn_memtrack_bytes     CONCAT2(MEMTRACK_FN_PREFIX, _bytes)
#define fn_memtrack_count     CONCAT2(MEMTRACK_FN_PREFIX, _count)
#define fn_memtrack_print_out CONCAT2(MEMTRACK_FN_PREFIX, _print_out)
#define fn_memtrack_print_err CONCAT2(MEMTRACK_FN_PREFIX, _print_err)

#ifdef __cplusplus
extern "C" {
#endif

struct struct_memtrack;
struct struct_ptrhdr;

typedef struct struct_memtrack struct_memtrack;
typedef struct struct_ptrhdr struct_ptrhdr;

struct_memtrack*  fn_memtrack_create    (struct_memtrack* memtrack, Context context);
void              fn_memtrack_destroy   (struct_memtrack* memtrack);

void              fn_memtrack_rdlock    (struct_memtrack* memtrack);
void              fn_memtrack_wrlock    (struct_memtrack* memtrack);
void              fn_memtrack_rdunlock  (struct_memtrack* memtrack);
void              fn_memtrack_wrunlock  (struct_memtrack* memtrack);

void              fn_memtrack_clear     (struct_memtrack* memtrack);
void              fn_memtrack_free      (struct_memtrack* memtrack, void* ptr);
void*             fn_memtrack_alloc     (struct_memtrack* memtrack, Context context, size_t size);
void*             fn_memtrack_realloc   (struct_memtrack* memtrack, Context context, void* ptr, size_t size);
void*             fn_memtrack_duplicate (struct_memtrack* memtrack, Context context, void* ptr, size_t size);

size_t            fn_memtrack_bytes     (struct_memtrack* memtrack);
size_t            fn_memtrack_count     (struct_memtrack* memtrack);
void              fn_memtrack_print_out (struct_memtrack* memtrack);
void              fn_memtrack_print_err (struct_memtrack* memtrack);

#define LIST_NAME           track_list
#define NODE_NAME           track_node
#define LIST_DATA_TYPE      struct_ptrhdr*
#define LIST_STRUCT
#include <blop/list.h>

struct struct_ptrhdr {
  struct_memtrack*  memtrack;
  track_node        node;
  size_t            size;
  Context           context;
};

struct struct_memtrack {
  RWLOCK_TYPE       lock;
  track_list        ptrs;
  size_t            bytes;
  Context           context;
  int               allocated;
};

#ifdef MEMTRACK_IMPLEMENTATION

#define LIST_NAME           track_list
#define NODE_NAME           track_node
#define LIST_DATA_TYPE      struct_ptrhdr*
#define LIST_NOT_STRUCT
#define LIST_IMPLEMENTATION
#include <blop/list.h>

#define MEMTRACK_PTR_TO_HDR(ptr) (struct_ptrhdr*)PTR_SUB(ptr, sizeof(struct struct_ptrhdr))
#define MEMTRACK_HDR_TO_PTR(hdr) (void*)PTR_ADD(hdr, sizeof(struct struct_ptrhdr))

struct_memtrack*  fn_memtrack_create(struct_memtrack* memtrack, Context context) {
  if (!memtrack) {
    CALLOC(memtrack, struct struct_memtrack, 1);
    memtrack->allocated = true;
  } else {
    memtrack->allocated = false;
  }

  RWLOCK_INIT(memtrack->lock);
  track_list_create(&memtrack->ptrs);

  memtrack->bytes   = 0;
  memtrack->context = context;

  return memtrack;
}
void              fn_memtrack_destroy(struct_memtrack* memtrack) {
  BLOP_ASSERT_PTR(memtrack);

  BLOP_ASSERT(memtrack->ptrs.size == 0, "Destroying non empty memtrack (HINT: Call free all)");

  track_list_destroy(&memtrack->ptrs);
  if (memtrack->allocated) {
    FREE(memtrack);
  }
}

void              fn_memtrack_rdlock(struct_memtrack* memtrack) {
  BLOP_ASSERT_PTR(memtrack);
  RWLOCK_RDLOCK(memtrack->lock);
}
void              fn_memtrack_wrlock(struct_memtrack* memtrack) {
  BLOP_ASSERT_PTR(memtrack);
  RWLOCK_WRLOCK(memtrack->lock);
}
void              fn_memtrack_rdunlock(struct_memtrack* memtrack) {
  BLOP_ASSERT_PTR(memtrack);
  RWLOCK_RDUNLOCK(memtrack->lock);
}
void              fn_memtrack_wrunlock(struct_memtrack* memtrack) {
  BLOP_ASSERT_PTR(memtrack);
  RWLOCK_WRUNLOCK(memtrack->lock);
}

void              fn_memtrack_clear(struct_memtrack* memtrack) {
  BLOP_ASSERT_PTR(memtrack);

  track_node* current = memtrack->ptrs.front;
  while (current != NULL) {
    struct_ptrhdr* hdr = current->data;
    current = current->next;
    track_list_pop_front(&memtrack->ptrs, true);
    memtrack->bytes -= hdr->size;
    FREE(hdr);
  }
}
void              fn_memtrack_free(struct_memtrack* memtrack, void* ptr) {
  BLOP_ASSERT_PTR(memtrack);
  BLOP_ASSERT_PTR(ptr);

  struct_ptrhdr* hdr = MEMTRACK_PTR_TO_HDR(ptr);
  BLOP_ASSERT(hdr->memtrack == memtrack, "Freeing a foreign ptr");

  track_list_erase(&memtrack->ptrs, &hdr->node, true);
  memtrack->bytes -= hdr->size;

  FREE(hdr);
}
void*             fn_memtrack_alloc(struct_memtrack* memtrack, Context context, size_t size) {
  BLOP_ASSERT_PTR(memtrack);

  BLOP_ASSERT(size != 0, "Requested allocation size is 0");

  struct_ptrhdr* hdr = NULL;
  MALLOC(hdr, struct struct_ptrhdr, size + sizeof(struct struct_ptrhdr));

  track_node_create(&hdr->node);
  track_list_push_back(&memtrack->ptrs, &hdr->node);
  hdr->node.data   = hdr;
  hdr->memtrack    = memtrack;
  hdr->size        = size;
  hdr->context     = context;

  memtrack->bytes += size;
  void* ptr        = MEMTRACK_HDR_TO_PTR(hdr);
  return ptr;
}
void*             fn_memtrack_realloc(struct_memtrack* memtrack, Context context, void* ptr, size_t size) {
  BLOP_ASSERT_PTR(memtrack);
  BLOP_ASSERT_PTR(ptr);

  BLOP_ASSERT(size != 0, "Requested allocation size is 0");

  struct_ptrhdr* srchdr = MEMTRACK_PTR_TO_HDR(ptr);
  BLOP_ASSERT(srchdr->memtrack == memtrack, "Reallocating a foreign ptr");

  struct_ptrhdr* newhdr = NULL;
  MALLOC(newhdr, struct struct_ptrhdr, size + sizeof(struct struct_ptrhdr));

  track_node_create(&newhdr->node);
  track_list_insert_next(&memtrack->ptrs, &srchdr->node, &newhdr->node);
  track_list_erase(&memtrack->ptrs, &srchdr->node, true);
  newhdr->node.data = newhdr;
  newhdr->memtrack    = memtrack;
  newhdr->size    = size;
  newhdr->context = context;

  memtrack->bytes    -= srchdr->size;
  memtrack->bytes    += size;
  void*  newptr  = MEMTRACK_HDR_TO_PTR(newhdr);
  memcpy(newptr, ptr, MIN(size, srchdr->size));
  FREE(srchdr);

  return newptr;
}
void*             fn_memtrack_duplicate(struct_memtrack* memtrack, Context context, void* ptr, size_t size) {
  BLOP_ASSERT_PTR(memtrack);
  BLOP_ASSERT_PTR(ptr);

  BLOP_ASSERT(size != 0, "Requested allocation size is 0");

  struct_ptrhdr* srchdr = MEMTRACK_PTR_TO_HDR(ptr);
  BLOP_ASSERT(srchdr->memtrack == memtrack, "Duplicating a foreign ptr");

  struct_ptrhdr* newhdr = NULL;
  MALLOC(newhdr, struct struct_ptrhdr, size + sizeof(struct struct_ptrhdr));

  track_node_create(&newhdr->node);
  track_list_insert_next(&memtrack->ptrs, &srchdr->node, &newhdr->node);
  newhdr->node.data = newhdr;
  newhdr->memtrack    = memtrack;
  newhdr->size    = size;
  newhdr->context = context;

  memtrack->bytes    += size;
  void*  newptr   = MEMTRACK_HDR_TO_PTR(newhdr);
  memcpy(newptr, ptr, MIN(size, srchdr->size));
  return newptr;
}

size_t            fn_memtrack_bytes(struct_memtrack* memtrack) {
  BLOP_ASSERT_PTR(memtrack);
  return memtrack->bytes;
}
size_t            fn_memtrack_count(struct_memtrack* memtrack) {
  BLOP_ASSERT_PTR(memtrack);
  return memtrack->ptrs.size;
}
void              fn_memtrack_print_out(struct_memtrack* memtrack) {
  BLOP_ASSERT_PTR(memtrack);

  LOG_STDOUT("%s", "Memtrack Information:\n");
  LOG_STDOUT(" Alias: %s\n", memtrack->context.alias);
  LOG_STDOUT(" Origin context: %s:%u (%s)\n", memtrack->context.file, memtrack->context.line, memtrack->context.func);
  LOG_STDOUT(" Allocated pointers: %zu\n", memtrack->ptrs.size);
  LOG_STDOUT(" Total bytes allocated: %zu\n\n", memtrack->bytes);

  if (memtrack->bytes == 0) {
    return;
  }
  
  track_node* current = memtrack->ptrs.front;
  LOG_STDOUT("%s", "Individual Pointer Information\n");
  size_t index = 0;
  while (current != NULL) {
    struct_ptrhdr* hdr = current->data;
    LOG_STDOUT(" Pointer [%zu]\n", index);
    LOG_STDOUT("  Size: %zu bytes\n", hdr->size);
    LOG_STDOUT("  Alias: %s\n", hdr->context.alias);
    LOG_STDOUT("  Addres: %p\n", MEMTRACK_HDR_TO_PTR(hdr));
    LOG_STDOUT("  Origin context: %s:%u (%s)\n\n", hdr->context.file, hdr->context.line, hdr->context.func);
    current = current->next;
    index++;
  }
}
void              fn_memtrack_print_err(struct_memtrack* memtrack) {
  BLOP_ASSERT_PTR(memtrack);

  LOG_STDERR("%s", "Memtrack Information:\n");
  LOG_STDERR(" Alias: %s\n", memtrack->context.alias);
  LOG_STDERR(" Origin context: %s:%u (%s)\n", memtrack->context.file, memtrack->context.line, memtrack->context.func);
  LOG_STDERR(" Allocated pointers: %zu\n", memtrack->ptrs.size);
  LOG_STDERR(" Total bytes allocated: %zu\n\n", memtrack->bytes);

  if (memtrack->bytes == 0) {
    return;
  }
  
  track_node* current = memtrack->ptrs.front;
  LOG_STDERR("%s", "Individual Pointer Information\n");
  size_t index = 0;
  while (current != NULL) {
    struct_ptrhdr* hdr = current->data;
    LOG_STDERR(" Pointer [%zu]\n", index);
    LOG_STDERR("  Size: %zu bytes\n", hdr->size);
    LOG_STDERR("  Alias: %s\n", hdr->context.alias);
    LOG_STDERR("  Addres: %p\n", MEMTRACK_HDR_TO_PTR(hdr));
    LOG_STDERR("  Origin context: %s:%u (%s)\n\n", hdr->context.file, hdr->context.line, hdr->context.func);
    current = current->next;
    index++;
  }
}

#endif /* MEMTRACK_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* __BLOP_MEMTRACK_H__ */