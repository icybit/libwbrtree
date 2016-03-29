#ifndef _RTREE_HELPERS_H_
#define _RTREE_HELPERS_H_

#include <stdint.h>
#include "../include/Rectangle.h"
#include "../include/Node.h"

rt_node_t create_2d_node(uint8_t m, uint8_t M, float alloc_factor, float *coords_low, float *coords_high, uint16_t level);
rt_entry_t create_2d_entry(void *tuple, float *coords_low, float *coords_high);
#endif
