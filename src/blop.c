#include <verba/blop.h>

#define STRING_NAME         String
#define STRING_FN_PREFIX    string
#define STRING_NOT_STRUCT
#define STRING_IMPLEMENTATION
#include <blop/string.h>

#define VECTOR_NAME         VECstr
#define VECTOR_DATA_TYPE    String
#define VECTOR_FN_PREFIX    vecstr
#define VECTOR_NOT_STRUCT
#define VECTOR_IMPLEMENTATION
#include <blop/vector.h>

#define MEMTRACK_NAME       Tracker
#define MEMTRACK_FN_PREFIX  tracker
#define MEMTRACK_NOT_STRUCT
#define MEMTRACK_IMPLEMENTATION
#include <blop/memtrack.h>

#define SLAB_NAME           SLABstr
#define SLAB_FN_PREFIX      slabstr
#define SLAB_NOT_STRUCT
#define SLAB_IMPLEMENTATION
#include <blop/slab.h>