#ifndef _RTREE_COMMON_H_
#define _RTREE_COMMON_H_

#include <math.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NELEMS(x) (sizeof(x)/sizeof((x)[0]))
#define NALLOC(m, M, f) MIN((int)ceil(m * f), M)

#endif