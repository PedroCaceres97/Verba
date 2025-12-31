#include <blop/blop.h>

#ifndef LIST_NAME
  #define LIST_NAME List
#endif /* LIST_NAME */

#ifndef NODE_NAME
  #define NODE_NAME Node
#endif /* NODE_NAME */

#ifndef LIST_FN_PREFIX
  #define LIST_FN_PREFIX LIST_NAME
#endif /* LIST_FN_PREFIX */

#ifndef NODE_FN_PREFIX
  #define NODE_FN_PREFIX NODE_NAME
#endif /* NODE_FN_PREFIX */

#ifndef LIST_DATA_TYPE
  #define LIST_DATA_TYPE int
#endif /* LIST_DATA_TYPE */

/** @cond doxygen_ignore */
#define struct_list         LIST_NAME
#define struct_node         NODE_NAME

#define fn_list_create      CONCAT2(LIST_FN_PREFIX, _create)
#define fn_list_destroy     CONCAT2(LIST_FN_PREFIX, _destroy)

#define fn_list_rdlock      CONCAT2(LIST_FN_PREFIX, _rdlock)
#define fn_list_wrlock      CONCAT2(LIST_FN_PREFIX, _wrlock)
#define fn_list_rdunlock    CONCAT2(LIST_FN_PREFIX, _rdunlock)
#define fn_list_wrunlock    CONCAT2(LIST_FN_PREFIX, _wrunlock)

#define fn_list_get         CONCAT2(LIST_FN_PREFIX, _get)
#define fn_list_size        CONCAT2(LIST_FN_PREFIX, _size)
#define fn_list_back        CONCAT2(LIST_FN_PREFIX, _back)
#define fn_list_front       CONCAT2(LIST_FN_PREFIX, _front)

#define fn_list_clear       CONCAT2(LIST_FN_PREFIX, _clear)
#define fn_list_erase       CONCAT2(LIST_FN_PREFIX, _erase)
#define fn_list_pop_back    CONCAT2(LIST_FN_PREFIX, _pop_back)
#define fn_list_pop_front   CONCAT2(LIST_FN_PREFIX, _pop_front)

#define fn_list_push_back   CONCAT2(LIST_FN_PREFIX, _push_back)
#define fn_list_push_front  CONCAT2(LIST_FN_PREFIX, _push_front)
#define fn_list_insert_next CONCAT2(LIST_FN_PREFIX, _insert_next)
#define fn_list_insert_prev CONCAT2(LIST_FN_PREFIX, _insert_prev)

#define fn_node_create      CONCAT2(NODE_FN_PREFIX, _create)
#define fn_node_duplicate   CONCAT2(NODE_FN_PREFIX, _duplicate)
#define fn_node_destroy     CONCAT2(NODE_FN_PREFIX, _destroy)

#define fn_node_set         CONCAT2(NODE_FN_PREFIX, _set)
#define fn_node_get         CONCAT2(NODE_FN_PREFIX, _get)
#define fn_node_next        CONCAT2(NODE_FN_PREFIX, _next)
#define fn_node_prev        CONCAT2(NODE_FN_PREFIX, _prev)
#define fn_node_list        CONCAT2(NODE_FN_PREFIX, _list)

/** @endcond */

#ifdef __cplusplus
extern "C" {
#endif

struct struct_node;
struct struct_list;

typedef struct struct_node struct_node;
typedef struct struct_list struct_list;

struct_list*    fn_list_create      (struct_list* list);
void            fn_list_destroy     (struct_list* list);

void            fn_list_rdlock      (struct_list* list);
void            fn_list_wrlock      (struct_list* list);
void            fn_list_rdunlock    (struct_list* list);
void            fn_list_wrunlock    (struct_list* list);

struct_node*    fn_list_get         (struct_list* list, size_t index);
size_t          fn_list_size        (struct_list* list);
struct_node*    fn_list_back        (struct_list* list);
struct_node*    fn_list_front       (struct_list* list);

void            fn_list_clear       (struct_list* list, int deallocate);
void            fn_list_erase       (struct_list* list, struct_node* node, int deallocate);
void            fn_list_pop_back    (struct_list* list, int deallocate);
void            fn_list_pop_front   (struct_list* list, int deallocate);

void            fn_list_push_back   (struct_list* list, struct_node* node);
void            fn_list_push_front  (struct_list* list, struct_node* node);
void            fn_list_insert_next (struct_list* list, struct_node* pivot, struct_node* node);
void            fn_list_insert_prev (struct_list* list, struct_node* pivot, struct_node* node);

struct_node*    fn_node_create      (struct_node* node);
struct_node*    fn_node_duplicate   (struct_node* src, struct_node* dst);
void            fn_node_destroy     (struct_node* node);

void            fn_node_set         (struct_node* node, LIST_DATA_TYPE value);
LIST_DATA_TYPE  fn_node_get         (struct_node* node);
struct_node*    fn_node_next        (struct_node* node);
struct_node*    fn_node_prev        (struct_node* node);
struct_list*    fn_node_list        (struct_node* node);

#if (defined(LIST_STRUCT) || defined(LIST_IMPLEMENTATION)) && !defined(LIST_NOT_STRUCT)
  struct struct_node {
    LIST_DATA_TYPE  data;
    struct_node*    next;
    struct_node*    prev;
    struct_list*    list;
    int             allocated;
  };

  struct struct_list {
    size_t          size;
    struct_node*    front;
    struct_node*    back;
    int             allocated;
    RWLOCK_TYPE     lock;
  };
#endif /* (defined(LIST_STRUCT) || defined(LIST_IMPLEMENTATION)) && !defined(LIST_NOT_STRUCT) */

#ifdef LIST_IMPLEMENTATION

struct_list*        fn_list_create(struct_list* list) {
  if (!list) {
    CALLOC(list, struct struct_list, 1);
    list->allocated = true;
  } else {
    list->allocated = false;
  }

  list->size  = 0;
  list->front = NULL;
  list->back  = NULL;
  RWLOCK_INIT(list->lock);

  return list;
}
void                fn_list_destroy(struct_list* list) {
  BLOP_ASSERT_PTR(list);

  BLOP_ASSERT(list->size == 0, "Destroying non empty list (HINT: Clear the list)");

  RWLOCK_DESTROY(list->lock);

  if (list->allocated) {
    FREE(list);
  }
}

void                fn_list_rdlock(struct_list* list) {
  BLOP_ASSERT_PTR(list);

  RWLOCK_RDLOCK(list->lock);
}
void                fn_list_wrlock(struct_list* list) {
  BLOP_ASSERT_PTR(list);

  RWLOCK_WRLOCK(list->lock);
}
void                fn_list_rdunlock(struct_list* list) {
  BLOP_ASSERT_PTR(list);

  RWLOCK_RDUNLOCK(list->lock);
}
void                fn_list_wrunlock(struct_list* list) {
  BLOP_ASSERT_PTR(list);

  RWLOCK_WRUNLOCK(list->lock);
}

size_t              fn_list_size(struct_list* list) {
  BLOP_ASSERT_PTR(list);
  return list->size;
}
struct_node*        fn_list_back(struct_list* list) {
  BLOP_ASSERT_PTR(list);
  return list->back;
}
struct_node*        fn_list_front(struct_list* list) {
  BLOP_ASSERT_PTR(list);
  return list->front;
}

struct_node*        fn_list_get(struct_list* list, size_t idx) {
  BLOP_ASSERT_PTR(list);

  BLOP_ASSERT_BOUNDS(idx, list->size);

  struct_node* current = NULL;
  if (idx < list->size / 2) {
    current = list->front;
    for (size_t i = 0; i < idx; i++) {
      current = current->next;
    }
  } else {
    current = list->back;
    for (size_t i = list->size - 1; i > idx; i--) {
      current = current->prev;
    }
  }
  return current;
}

void                fn_list_clear(struct_list* list, int deallocate) {
  BLOP_ASSERT_PTR(list);

  if (list->size == 0) {
    return;
  }

  struct_node* current = list->front;
  struct_node* next = NULL;
  while (current) {
    next = current->next;
    current->list = NULL;
    current->next = NULL;
    current->prev = NULL;
    if (deallocate) {
      fn_node_destroy(current);
    }
    current = next;
  }

  list->size = 0;
  list->front = NULL;
  list->back = NULL;
}
void                fn_list_erase(struct_list* list, struct_node* node, int deallocate) {
  BLOP_ASSERT_PTR(list);
  BLOP_ASSERT_PTR(node);

  BLOP_ASSERT(node->list == list, "Erasing a foreign node");

  if (node->prev) {
    node->prev->next = node->next;
  } else {
    list->front = node->next;
  }

  if (node->next) {
    node->next->prev = node->prev;
  } else {
    list->back = node->prev;
  }

  list->size--;
  node->list = NULL;
  node->next = NULL;
  node->prev = NULL;

  if (deallocate) {
    fn_node_destroy(node);
  }
}
void                fn_list_pop_back(struct_list* list, int deallocate) {
  BLOP_ASSERT_PTR(list);

  if (list->size == 0) {
    EMPTY_POPPING();
    return;
  }

  struct_node* back = list->back;

  if (list->back->prev) {
    list->back->prev->next = NULL;
    list->back = list->back->prev;
  } else {
    list->front = NULL;
    list->back = NULL;
  }

  list->size--;

  back->list = NULL;
  back->next = NULL;
  back->prev = NULL;
  if (deallocate) {
    fn_node_destroy(back);
  }
}
void                fn_list_pop_front(struct_list* list, int deallocate) {
  BLOP_ASSERT_PTR(list);

  if (list->size == 0) {
    EMPTY_POPPING();
    return;
  }

  struct_node* front = list->front;

  if (list->front->next) {
    list->front->next->prev = NULL;
    list->front = list->front->next;
  } else {
    list->front = NULL;
    list->back = NULL;
  }

  list->size--;

  front->list = NULL;
  front->next = NULL;
  front->prev = NULL;
  if (deallocate) {
    fn_node_destroy(front);
  }
}

void                fn_list_push_back(struct_list* list, struct_node* node) {
  BLOP_ASSERT_PTR(list);
  BLOP_ASSERT_PTR(node);

  BLOP_ASSERT(node->list == NULL, "Pushing a foreign node (HINT: Duplicate the node)");

  node->list = list;
  node->next = NULL;
  node->prev = list->back;

  if (list->back) {
    list->back->next = node;
    list->back = node;
  } else {
    list->front = node;
    list->back = node;
  }

  list->size++;
}
void                fn_list_push_front(struct_list* list, struct_node* node) {
  BLOP_ASSERT_PTR(list);
  BLOP_ASSERT_PTR(node);

  BLOP_ASSERT(node->list == NULL, "Pushing a foreign node (HINT: Duplicate the node)");

  node->list = list;
  node->next = list->front;
  node->prev = NULL;

  if (list->front) {
    list->front->prev = node;
    list->front = node;
  } else {
    list->front = node;
    list->back = node;
  }

  list->size++;
}
void                fn_list_insert_next(struct_list* list, struct_node* pivot, struct_node* node) {
  BLOP_ASSERT_PTR(list);
  BLOP_ASSERT_PTR(pivot);
  BLOP_ASSERT_PTR(node);

  BLOP_ASSERT(node->list == NULL, "Inserting a foreign node (HINT: Duplicate the node)");
  BLOP_ASSERT(pivot->list == list, "The pivot is a foreign node");

  if (pivot == list->back) {
    fn_list_push_back(list, node);
    return;
  }

  node->list = list;
  node->prev = pivot;
  node->next = pivot->next;

  pivot->next->prev = node;
  pivot->next = node;

  list->size++;
}
void                fn_list_insert_prev(struct_list* list, struct_node* pivot, struct_node* node) {
  BLOP_ASSERT_PTR(list);
  BLOP_ASSERT_PTR(pivot);
  BLOP_ASSERT_PTR(node);

  BLOP_ASSERT(node->list == NULL, "Inserting a foreign node (HINT: Duplicate the node)");
  BLOP_ASSERT(pivot->list == list, "The pivot is a foreign node");

  if (pivot == list->front) {
    fn_list_push_front(list, node);
    return;
  }

  node->list = list;
  node->next = pivot;
  node->prev = pivot->prev;

  pivot->prev->next = node;
  pivot->prev = node;

  list->size++;
}

struct_node*        fn_node_create(struct_node* node) {
  if (!node) {
    CALLOC(node, struct struct_node, 1);
    node->allocated = true;
  } else {
    node->allocated = false;
  }


  node->data = (LIST_DATA_TYPE)(0);
  node->next = NULL;
  node->prev = NULL;
  node->list = NULL;
  RWLOCK_INIT(node->lock);

  return node;
}
struct_node*        fn_node_duplicate(struct_node* src, struct_node* dst) {
  BLOP_ASSERT_PTR(src);

  dst = fn_node_create(dst);
  dst->data = src->data;

  return dst;
}
void                fn_node_destroy(struct_node* node) {
  BLOP_ASSERT_PTR(node);

  BLOP_ASSERT(node->list == NULL, "Destroying an unattached node (HINT: Set deallocate to true in any list erasing function)");

  #ifdef LIST_DEALLOCATE_DATA
    LIST_DEALLOCATE_DATA(node->data);
  #endif
  RWLOCK_DESTROY(node->lock);

  if (node->allocated) {
    FREE(node);
  }
}

void                fn_node_set(struct_node* node, LIST_DATA_TYPE value) {
  BLOP_ASSERT_PTR(node);
  node->data = value;
}
LIST_DATA_TYPE      fn_node_get(struct_node* node) {
  BLOP_ASSERT_PTR(node);
  return node->data;
}
struct_node*        fn_node_next(struct_node* node) {
  BLOP_ASSERT_PTR(node);
  return node->next;
}
struct_node*        fn_node_prev(struct_node* node) {
  BLOP_ASSERT_PTR(node);
  return node->prev;
}
struct_list*        fn_node_list(struct_node* node) {
  BLOP_ASSERT_PTR(node);
  return node->list;
}

#endif /* LIST_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#undef LIST_NAME
#undef NODE_NAME
#undef LIST_FN_PREFIX
#undef NODE_FN_PREFIX

#undef LIST_DATA_TYPE
#undef LIST_DEALLOCATE_DATA

#undef LIST_STRUCT
#undef LIST_NOT_STRUCT
#undef LIST_IMPLEMENTATION

#undef struct_list        
#undef struct_node             

#undef fn_list_create
#undef fn_list_destroy

#undef fn_list_rdlock
#undef fn_list_wrlock
#undef fn_list_rdunlock
#undef fn_list_wrunlock

#undef fn_list_get
#undef fn_list_size
#undef fn_list_back
#undef fn_list_front

#undef fn_list_clear
#undef fn_list_erase
#undef fn_list_pop_back
#undef fn_list_pop_front

#undef fn_list_push_back
#undef fn_list_push_front
#undef fn_list_insert_next
#undef fn_list_insert_prev

#undef fn_node_create
#undef fn_node_duplicate
#undef fn_node_destroy

#undef fn_node_set
#undef fn_node_get
#undef fn_node_next       
#undef fn_node_prev       
#undef fn_node_list       