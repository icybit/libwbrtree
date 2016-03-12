#include "Helpers.h"
#include "../include/Rectangle.h"
#include "../include/Node.h"
#include "../include/RTree.h"

struct Point * create_2d_point(float *coords) {
	int dim = 2;
	struct Point *point = malloc(sizeof(struct Point));	

	point_create(point, dim, coords);
	
	return point;
}

struct Rectangle * create_2d_rectangle(float *coords_low, float *coords_high) {
	int dim = 2;
	struct Point *low = create_2d_point(coords_low);
	struct Point *high = create_2d_point(coords_high);
	struct Rectangle *rectangle = malloc(sizeof(struct Rectangle));

	rectangle_create(rectangle, low, high);

	return rectangle;
}

struct Node * create_2d_node(uint8_t m, uint8_t M, float alloc_factor, float *coords_low, float *coords_high, uint16_t level) {
	int dim = 2;
	struct Rectangle *rectangle = create_2d_rectangle(coords_low, coords_high);
	struct Context *context = malloc(sizeof(struct Context));
	struct Node *node = malloc(sizeof(struct Node));

	context_create(context, m, M, dim, alloc_factor, rectangle);
	node_create(node, context, NULL, NULL, 0, context->space, level);

	return node;
}

struct Context * create_2d_context(uint8_t m, uint8_t M, float alloc_factor, float *coords_low, float *coords_high) {
	int dim = 2;
	struct Rectangle *rectangle = create_2d_rectangle(coords_low, coords_high);
	struct Context *context = malloc(sizeof(struct Context));
	context_create(context, m, M, dim, alloc_factor, rectangle);

	return context;
}

struct RTree * create_2d_rtree(uint8_t m, uint8_t M, float alloc_factor, float *coords_low, float *coords_high) {
	struct Context *context = create_2d_context(m, M, alloc_factor, coords_low, coords_high);
	struct RTree *rtree = malloc(sizeof(struct RTree));
	rtree_create(rtree, context);

	return rtree;
}

struct Entry * create_2d_entry(void *tuple, float *coords_low, float *coords_high) {
	struct Rectangle *rectangle = create_2d_rectangle(coords_low, coords_high);
	struct Entry *entry = malloc(sizeof(struct Entry));
	entry_create(entry, tuple, rectangle);

	return entry;
}

void destroy_rectangle(struct Rectangle *rectangle) {
	free(rectangle->low);
	free(rectangle->high);
	free(rectangle);
}

void destroy_context(struct Context *context) {
	destroy_rectangle(context->space);
	free(context);
}

void destroy_entry(struct Entry *entry) {
	destroy_rectangle(entry->MBR);
	free(entry);
}