#define STRING_IMPLEMENTATION
#define MEMTRACK_IMPLEMENTATION
#include <verba/blop.h>
#include <verba/mem.h>

#define VECTOR_NAME                 VECstr
#define VECTOR_FN_PREFIX            vecstr
#define VECTOR_DATA_TYPE            String*
#define VECTOR_DEALLOCATE_DATA(ptr) mem_string_free(ptr)
#define VECTOR_NOT_STRUCT
#define VECTOR_IMPLEMENTATION
#include <blop/vector.h>

#define SLAB_NAME                   SLABstr
#define SLAB_FN_PREFIX              slabstr
#define SLAB_DATA_TYPE              String
#define SLAB_NOT_STRUCT
#define SLAB_IMPLEMENTATION
#include <blop/slab.h>
