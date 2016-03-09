#ifndef _RTREE_HELPERS_H_
#define _RTREE_HELPERS_H_

#include <stdint.h>

struct Point * create_2d_point(float *coords);
struct Rectangle * create_2d_rectangle(float *coords_low, float *coords_high);
struct Node * create_2d_node(uint8_t m, uint8_t M, float alloc_factor, float *coords_low, float *coords_high, uint16_t level);
struct Context * create_2d_context(uint8_t m, uint8_t M, float alloc_factor, float *coords_low, float *coords_high);
struct RTree * create_2d_rtree(uint8_t m, uint8_t M, float alloc_factor, float *coords_low, float *coords_high);

void destroy_rectangle(struct Rectangle *rectangle);
void destroy_context(struct Context *context);
void destroy_entry(struct Entry *entry);

#endif
