#ifndef _RTREE_NODE_H_
#define _RTREE_NODE_H_

#if (defined _GNU_SOURCE || defined __gnu_hurd__ || defined __GNU__ || \
       defined __linux__ || defined __MINGW32__ || defined __GLIBC__)
#define _QSORT_LINUX
#elif (defined _WIN32 || defined _WIN64 || defined __WINDOWS__)
#define _QSORT_WINDOWS
#endif

#include <stdint.h>

typedef struct Rectangle rt_rect_t;

typedef struct Context {
	uint8_t m;
	uint8_t M;
	uint8_t dim;
	size_t buffer_size;
	float alloc_factor;
	rt_rect_t *space;
} rt_ctx_t;

typedef struct Entry {
	void *tuple;
	rt_rect_t *MBR;
} rt_entry_t;

typedef struct Node {
	rt_ctx_t *context;
	struct Node *parent;
	void **entries;
	rt_rect_t *MBR;
	uint8_t capacity;
	uint8_t count;
	uint16_t level;
} rt_node_t;

void context_create(rt_ctx_t *dest, uint8_t m, uint8_t M, uint8_t dim, size_t buffer_size, float alloc_factor, rt_rect_t *space_MBR);

#ifdef _QSORT_LINUX
int entry_compare(const void *entry, const void *other, void *dimension);
#endif
#ifdef _QSORT_WINDOWS
int entry_compare(void *dimension, const void *entry, const void *other);
#endif

void entry_create(rt_entry_t *dest, void *tuple, rt_rect_t *MBR);
#ifdef DEBUG
void entry_print(rt_entry_t *entry);
#endif
size_t entry_serialize(rt_entry_t *entry, unsigned char *buffer);

int node_add_entry(rt_node_t *node, void *entry);
void node_adjust_MBR(rt_node_t *node, void *entry);
void node_calculate_MBR(rt_rect_t *MBR, rt_node_t *node);
void _node_calculate_node_MBR(rt_rect_t *MBR, rt_node_t *node);
void _node_calculate_leaf_MBR(rt_rect_t *MBR, rt_node_t *leaf);
rt_node_t * node_choose_optimal_entry(rt_node_t *node, rt_entry_t *entry);
uint8_t _node_choose_split_axis(rt_node_t *node, void ***sorted_entries, rt_rect_t *MBR_one, rt_rect_t *MBR_two);
uint8_t _node_choose_split_index(uint8_t dimension, rt_node_t *node, void ***sorted_entries, rt_rect_t *MBR_one, rt_rect_t *MBR_two);
#ifdef _QSORT_LINUX
int node_compare(const void *entry, const void *other, void *dimension);
#endif
#ifdef _QSORT_WINDOWS
int node_compare(void *dimension, const void *entry, const void *other);
#endif
void node_copy(rt_node_t *dest, const rt_node_t *source);
void node_create(rt_node_t *dest, rt_ctx_t *context, rt_node_t *parent, void **entries, uint8_t entry_count, rt_rect_t *MBR, uint16_t level);
void node_delete_entry(rt_node_t *node, void *entry);
void node_destroy(rt_node_t *node);
double _node_evaluate_distribution(uint8_t k, void ***sorted_entries, uint8_t dimension, rt_node_t *node, rt_rect_t *MBR_one, rt_rect_t *MBR_two, double(*evaluator)(rt_rect_t *MBR_one, rt_rect_t *MBR_two));
rt_rect_t * _node_get_entry_MBR(rt_node_t *node, void *entry);
int node_is_leaf(rt_node_t *node);
int node_is_root(rt_node_t *node);
rt_node_t * node_split(rt_node_t *node, void *entry);

#endif