#include "../include/Rectangle.h"
#include "../include/Node.h"
#include "../include/RTree.h"
#include "Helpers.h"

/* TODO: Revise use of Helpers and remove them if not needed */

//struct Point * create_2d_point(float *coords) {
//	int dim = 2;
//	struct Point *point = malloc(sizeof(struct Point));	
//
//	point_create(point, dim, coords);
//	
//	return point;
//}
//
//rt_rect_t * create_2d_rectangle(float *coords_low, float *coords_high) {
//	struct Point *low = create_2d_point(coords_low);
//	struct Point *high = create_2d_point(coords_high);
//	rt_rect_t *rectangle = malloc(sizeof(rt_rect_t));
//
//	rectangle_create(rectangle, low, high);
//
//	return rectangle;
//}
//
rt_node_t create_2d_node(uint8_t m, uint8_t M, float alloc_factor, float *coords_low, float *coords_high, uint16_t level) {
	uint8_t dim = 2;
	size_t buffer_size = 35;
	rt_rect_t rectangle; 
	rt_ctx_t context;
	rt_node_t node;

	rectangle_create(&rectangle, coords_low, coords_high, dim);
	context_create(&context, m, M, dim, alloc_factor, buffer_size, &rectangle);
	node_create(&node, &context, NULL, NULL, 0, context.space, level);

	return node;
}
//
//rt_ctx_t * create_2d_context(uint8_t m, uint8_t M, float alloc_factor, float *coords_low, float *coords_high) {
//	int dim = 2;
//	size_t buffer_size = 35;
//	rt_rect_t *rectangle = create_2d_rectangle(coords_low, coords_high);
//	rt_ctx_t *context = malloc(sizeof(rt_ctx_t));
//	context_create(context, m, M, dim, alloc_factor, buffer_size, rectangle);
//
//	return context;
//}
//
//rt_rtree_t * create_2d_rtree(uint8_t m, uint8_t M, float alloc_factor, float *coords_low, float *coords_high) {
//	rt_ctx_t *context = create_2d_context(m, M, alloc_factor, coords_low, coords_high);
//	rt_rtree_t *rtree = malloc(sizeof(rt_rtree_t));
//	rtree_create(rtree, context);
//
//	return rtree;
//}
//
rt_entry_t create_2d_entry(void *tuple, float *coords_low, float *coords_high) {
	uint8_t dim = 2;
	rt_rect_t rectangle;
	rt_entry_t entry;

	rectangle_create(&rectangle, coords_low, coords_high, dim);
	entry_create(&entry, tuple, &rectangle);

	return entry;
}
//
//void destroy_rectangle(rt_rect_t *rectangle) {
//	free(rectangle->low);
//	free(rectangle->high);
//	free(rectangle);
//}
//
//void destroy_context(rt_ctx_t *context) {
//	destroy_rectangle(context->space);
//	free(context);
//}
//
//void destroy_entry(rt_entry_t *entry) {
//	destroy_rectangle(entry->MBR);
//	free(entry);
//}