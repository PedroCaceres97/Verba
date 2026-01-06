#ifndef __VERBA_BLOP_H__
#define __VERBA_BLOP_H__

#include <verba/log.h>

#define LOG_STDOUT(format, ...) log_stdout(format, __VA_ARGS__)
#define LOG_STDERR(format, ...) log_stderr(format, __VA_ARGS__)
#include <blop/blop.h>

#define STRING_NAME         String
#define STRING_FN_PREFIX    string
#define STRING_STRUCT
#include <blop/string.h>

#define VECTOR_NAME         VECstr
#define VECTOR_FN_PREFIX    vecstr
#define VECTOR_DATA_TYPE    String*
#define VECTOR_STRUCT
#include <blop/vector.h>

#define MEMTRACK_NAME       Tracker
#define MEMTRACK_FN_PREFIX  tracker
#define MEMTRACK_STRUCT
#include <blop/memtrack.h>

#define SLAB_NAME           SLABstr
#define SLAB_FN_PREFIX      slabstr
#define SLAB_DATA_TYPE      String
#define SLAB_STRUCT
#include <blop/slab.h>

#endif /* __VERBA_BLOP_H__ */
