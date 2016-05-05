#include "TestNode.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>
#include "../src/Common.h"
#include "../src/Context.h"
#include "../src/Entry.h"
#include "../src/Node.h"
#include "../src/Rectangle.h"

static rt_rect_t * create_rectangle_2d(float low_x, float low_y, float high_x, float high_y);
static float * initialize_coordinates(float x, float y);
static size_t serialize(rt_entry_t *entry, uint8_t **buffer);

void _test_context_create() {
	rt_ctx_t *context;
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	uint8_t m = 4, M = 12, dimension = 2;
	float alloc_factor = 4.0f;

	context = rtree_context_create(m, M, dimension, serialize, alloc_factor, space_MBR);

	assert_int_equal(context->m, m);
	assert_int_equal(context->M, M);
	assert_true(context->alloc_factor == 4.0f);
	assert_ptr_equal(context->space, space_MBR);

	rtree_context_destroy(context);
}

void _test_entry_create() {	
	rt_rect_t *rectangle = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_entry_t *entry;
	uint8_t *tuple = malloc(sizeof(uint8_t));

	*tuple = 4;

	entry = rtree_entry_create(tuple, rectangle);

	assert_ptr_equal(entry->tuple, tuple);
	assert_ptr_equal(entry->MBR, rectangle);

	rtree_entry_destroy(entry);
}

/* TODO: Cover node create with more tests */
void _test_node_create() {
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_ctx_t *context;
	rt_node_t *node;
	uint8_t dim = 2, m = 4, M = 12, level = 0;	
	float alloc_factor = 4;

	context = rtree_context_create(m, M, dim, serialize, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle, level);
		
	assert_ptr_equal(node->context, context);
	assert_ptr_equal(node->MBR, rectangle);
	assert_int_equal(node->level, level);
	assert_null(node->parent);	
	assert_non_null(node->entries);
	assert_int_equal(node->capacity, MAX(0, NALLOC(context->m, context->M, context->alloc_factor)));

	node_destroy(node);
	rtree_context_destroy(context);
}

void _test_node_is_leaf() {
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_ctx_t *context;
	rt_node_t *node;
	uint8_t m = 4, M = 12, level = 0, dim = 2;
	float alloc_factor = 4.0f;

	context = rtree_context_create(m, M, dim, serialize, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle, level);

	assert_true(node_is_leaf(node));

	node->level = 1;

	assert_false(node_is_leaf(node));

	node_destroy(node);
	rtree_context_destroy(context);
}

void _test_node_is_root() {	
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_ctx_t *context;
	rt_node_t *node;
	uint8_t m = 4, M = 12, level = 0, dim = 2;
	float alloc_factor = 4.0f;

	context = rtree_context_create(m, M, dim, serialize, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle, level);

	assert_true(node_is_root(node));

	node->parent = node;

	assert_false(node_is_root(node));

	node_destroy(node);
	rtree_context_destroy(context);
}

void _test_node_add_entry() {
	rt_rect_t *space_MBR = create_rectangle_2d(1.0f, 1.0f, 3.0f, 3.0f); 
	rt_rect_t *rectangle_a = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f); 
	rt_rect_t *rectangle_b = create_rectangle_2d(1.0f, 1.0f, 3.0f, 3.0f);
	rt_ctx_t *context;
	rt_node_t *node;
	rt_entry_t *entry;
	uint8_t index, success = 0, dim = 2, m = 4, M = 12, level = 0;
	uint8_t tuple = 4;
	float alloc_factor = 2.0f;	

	context = rtree_context_create(m, M, dim, serialize, alloc_factor, space_MBR);
	entry = rtree_entry_create(&tuple, rectangle_b);
	node = node_create(context, NULL, NULL, 0, rectangle_a, level);

	success = node_add_entry(node, entry);
	assert_true(success);
	assert_ptr_equal(node->entries[0], entry);

	for (index = 0; index < dim; index++) {
		assert_true(node->MBR->low[index] == rectangle_a->low[index]);
		assert_true(node->MBR->high[index] == rectangle_b->high[index]);
	}

	for (index = 0; index < node->capacity; index++) {
		if (index < M - 1) {
			assert_true(node_add_entry(node, entry));
			assert_ptr_equal(node->entries[index], entry);
		}
		else {
			assert_false(node_add_entry(node, entry));
		}		
	}

	node_destroy(node);
	rtree_context_destroy(context);
}

void _test_node_delete_entry() 
{
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *MBRs[4];
	rt_ctx_t *context;
	rt_entry_t *entries[4];
	rt_node_t *node;
	uint8_t index, dim = 2, m = 4, M = 12, level = 0, *tuples[4];
	float alloc_factor = 2.0f;

	for (index = 0; index < 4; index++)
	{
		tuples[index] = malloc(sizeof(uint8_t));
		*tuples[index] = index + 1;
		MBRs[index] = create_rectangle_2d(1.0f, 1.0f, 3.0f, 3.0f);
	}

	context = rtree_context_create(m, M, dim, serialize, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle, level);

	for (index = 0; index < 4; index++) {
		entries[index] = rtree_entry_create(tuples[index], MBRs[index]);
		node_add_entry(node, entries[index]);
	}

	node_delete_entry(node, entries[1]);
	assert_ptr_equal(node->entries[0], entries[0]);
	assert_ptr_equal(node->entries[1], entries[2]);
	assert_ptr_equal(node->entries[2], entries[3]);
	assert_int_equal(node->count, 3);

	/* Removed entry_1, sequence should be 3, 4 */
	node_delete_entry(node, entries[0]);
	assert_ptr_equal(node->entries[0], entries[2]);
	assert_ptr_equal(node->entries[1], entries[3]);
	assert_int_equal(node->count, 2);

	/* Removed entry_4, sequence should be 3 */
	node_delete_entry(node, entries[3]);
	assert_ptr_equal(node->entries[0], entries[2]);
	assert_int_equal(node->count, 1);

	/* Removed entry_3, there should be 0 entries */
	node_delete_entry(node, entries[2]);
	assert_int_equal(node->count, 0);

	node_destroy(node);	
	rtree_context_destroy(context);
}

void _test_node_adjust_MBR() {
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle_a = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle_b = create_rectangle_2d(1.0f, 1.0f, 3.0f, 3.0f);
	rt_ctx_t *context;
	rt_node_t *node;
	rt_entry_t *entry;
	uint8_t index, dim = 2, m = 4, M = 12, level = 0, *tuple = malloc(sizeof(uint8_t));
	float alloc_factor = 4.0f;

	*tuple = 4;

	context = rtree_context_create(m, M, dim, serialize, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle_a, level);
	entry = rtree_entry_create(tuple, rectangle_b);
		
	node_adjust_MBR(node, entry);
	for (index = 0; index < dim; index++) {
		assert_true(node->MBR->low[index] == rectangle_a->low[index]);
		assert_true(node->MBR->high[index] == rectangle_b->high[index]);
	}

	node_destroy(node);
	rtree_context_destroy(context);
}

/* TODO: Fix test. Memory gets corrupted when creating nodes and/or adding entries to them.
	Corruption identified by investigating the failed assertion on rectangle->dim at varying steps.
*/
void _test_node_choose_optimal_entry() {
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle_0 = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle_1 = create_rectangle_2d(3.0f, -1.0f, 5.0f, 1.0f);
	rt_rect_t *rectangle_2 = create_rectangle_2d(-1.0f, -1.0f, 1.0f, 1.0f);
	rt_rect_t *rectangle_3 = create_rectangle_2d(-1.0f, 5.0f, 2.0f, 7.0f);
	rt_rect_t *rectangle_4 = create_rectangle_2d(-3.0f, 0.0f, -2.0f, 1.0f);
	rt_ctx_t *context;
	rt_entry_t *entry;
	rt_node_t *node, *node_1, *node_2, *node_3;
	uint8_t dim = 2, m = 4, M = 12, *tuple = malloc(sizeof(uint8_t));
	float alloc_factor = 4.0f;

	*tuple = 1;

	context = rtree_context_create(m, M, dim, serialize, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle_0, 2);
	node_1 = node_create(context, NULL, NULL, 0, rectangle_1, 1);
	node_2 = node_create(context, NULL, NULL, 0, rectangle_2, 1);
	node_3 = node_create(context, NULL, NULL, 0, rectangle_3, 1);
	entry = rtree_entry_create(tuple, rectangle_4);

	node_add_entry(node, node_1);
	node_add_entry(node, node_2);
	node_add_entry(node, node_3);

	assert_ptr_equal(node_choose_optimal_entry(node, entry), node_2);

	node_destroy(node);
	node_destroy(node_1);
	node_destroy(node_2);
	node_destroy(node_3);
	rtree_entry_destroy(entry);
	rtree_context_destroy(context);
}

void _test_entry_compare() {
	rt_rect_t *rectangle_1 = create_rectangle_2d(1.0f, 1.0f, 3.0f, 3.0f);
	rt_rect_t *rectangle_2 = create_rectangle_2d(0.0f, 2.0f, 4.0f, 2.0f);
	rt_entry_t *entry_1, *entry_2;
	uint8_t index, dim_1 = 0, dim_2 = 1, *tuples[2];

	for (index = 0; index < 2; index++)
	{
		tuples[index] = malloc(sizeof(uint8_t));
		*tuples[index] = index + 1;
	}

	entry_1 = rtree_entry_create(tuples[0], rectangle_1);
	entry_2 = rtree_entry_create(tuples[1], rectangle_2);
	
	assert_true(entry_compare(&entry_1, &entry_2, &dim_1) > 0);
	assert_true(entry_compare(&entry_1, &entry_2, &dim_2) < 0);
	assert_true(entry_compare(&entry_2, &entry_1, &dim_1) < 0);
	assert_true(entry_compare(&entry_2, &entry_1, &dim_2) > 0);
	assert_true(entry_compare(&entry_2, &entry_2, &dim_1) == 0);
	assert_true(entry_compare(&entry_2, &entry_2, &dim_2) == 0);

	rtree_entry_destroy(entry_1);
	rtree_entry_destroy(entry_2);
}

void _test_node_calculate_MBR_Leaf() {
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle_0 = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle_1 = create_rectangle_2d(3.0f, -1.0f, 5.0f, 1.0f);
	rt_rect_t *rectangle_2 = create_rectangle_2d(-1.0f, -1.0f, 1.0f, 1.0f);
	rt_rect_t *rectangle_3 = create_rectangle_2d(-1.0f, 5.0f, 2.0f, 7.0f);
	rt_ctx_t *context;
	rt_node_t *node;
	rt_entry_t *entry_1, *entry_2, *entry_3;
	uint8_t dim = 2, m = 2, M = 4, level = 0, tuples[] = { 1, 2, 3 };
	float alloc_factor = 4.0f;
	
	context = rtree_context_create(m, M, dim, serialize, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle_0, level);

	entry_1 = rtree_entry_create(&tuples[0], rectangle_1);
	entry_2 = rtree_entry_create(&tuples[1], rectangle_2);
	entry_3 = rtree_entry_create(&tuples[2], rectangle_3);
	
	node_add_entry(node, entry_1);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low[0] == 0);
	assert_true(node->MBR->low[1] == -1);
	assert_true(node->MBR->high[0] == 5);
	assert_true(node->MBR->high[1] == 2);

	node_add_entry(node, entry_2);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low[0] == -1);
	assert_true(node->MBR->low[1] == -1);
	assert_true(node->MBR->high[0] == 5);
	assert_true(node->MBR->high[1] == 2);

	node_add_entry(node, entry_3);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low[0] == -1);
	assert_true(node->MBR->low[1] == -1);
	assert_true(node->MBR->high[0] == 5);
	assert_true(node->MBR->high[1] == 7);

	node_destroy(node);	
	rtree_context_destroy(context);
}

/* TODO: Fix segmentation fault */
void _test_node_calculate_MBR_Non_Leaf() {
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle_0 = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle_1 = create_rectangle_2d(3.0f, -1.0f, 5.0f, 1.0f);
	rt_rect_t *rectangle_2 = create_rectangle_2d(-1.0f, -1.0f, 1.0f, 1.0f);
	rt_rect_t *rectangle_3 = create_rectangle_2d(-1.0f, 5.0f, 2.0f, 7.0f);
	rt_ctx_t *context;
	rt_entry_t *entry_1, *entry_2, *entry_3, **entries_1, **entries_2, **entries_3;
	rt_node_t *node, *node_1, *node_2, *node_3;
	uint8_t index, dim = 2, m = 2, M = 4, leaf_level = 0, root_level = 1, *tuples[3];
	float alloc_factor = 4.0f;

	for (index = 0; index < 3; index++)
	{
		tuples[index] = malloc(sizeof(uint8_t));
		*tuples[index] = index + 1;
	}

	entries_1 = malloc(sizeof(void *));
	entries_2 = malloc(sizeof(void *));
	entries_3 = malloc(sizeof(void *));

	context = rtree_context_create(m, M, dim, serialize, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle_0, root_level);

	entry_1 = rtree_entry_create(tuples[0], rectangle_1);
	entries_1[0] = entry_1;
	node_1 = node_create(context, node, (void **)entries_1, 1, rectangle_1, leaf_level);

	entry_2 = rtree_entry_create(tuples[1], rectangle_2);
	entries_2[0] = entry_2;
	node_2 = node_create(context, node, (void **)entries_2, 1, rectangle_2, leaf_level);

	entry_3 = rtree_entry_create(tuples[2], rectangle_3);
	entries_3[0] = entry_3;
	node_3 = node_create(context, node, (void **)entries_3, 1, rectangle_3, leaf_level);

	node_add_entry(node, node_1);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low[0] == 0);
	assert_true(node->MBR->low[1] == -1);
	assert_true(node->MBR->high[0] == 5);
	assert_true(node->MBR->high[1] == 2);

	node_add_entry(node, node_2);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low[0] == -1);
	assert_true(node->MBR->low[1] == -1);
	assert_true(node->MBR->high[0] == 5);
	assert_true(node->MBR->high[1] == 2);

	node_add_entry(node, node_3);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low[0] == -1);
	assert_true(node->MBR->low[1] == -1);
	assert_true(node->MBR->high[0] == 5);
	assert_true(node->MBR->high[1] == 7);

	node_destroy(node);
	node_destroy(node_1);
	node_destroy(node_2);
	node_destroy(node_3);
	rtree_entry_destroy(entry_1);
	rtree_entry_destroy(entry_2);
	rtree_entry_destroy(entry_3);
	rtree_context_destroy(context);
}

static rt_rect_t * create_rectangle_2d(float low_x, float low_y, float high_x, float high_y)
{
	float *low = initialize_coordinates(low_x, low_y);
	float *high = initialize_coordinates(high_x, high_y);

	return rtree_rectangle_create(low, high, 2);
}

static float * initialize_coordinates(float x, float y) 
{
	float *coords = malloc(2 * sizeof(float));

	coords[0] = x;
	coords[1] = y;

	return coords;
}

static size_t serialize(rt_entry_t *entry, uint8_t **buffer)
{
	size_t index = 0;
	*buffer = malloc(2 * sizeof(float) + sizeof(uint8_t));
	memmove(&(*buffer[index]), entry->MBR->low, 2 * sizeof(float));
	index += 2 * sizeof(float);
	memmove(&(*buffer[index]), &entry->MBR->dim, sizeof(uint8_t));
	index += sizeof(uint8_t);

	return index;
}

int test_node(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(_test_context_create),
		cmocka_unit_test(_test_entry_create),
		cmocka_unit_test(_test_node_create),
		cmocka_unit_test(_test_node_is_leaf),
		cmocka_unit_test(_test_node_is_root),
		cmocka_unit_test(_test_node_adjust_MBR),
		/*cmocka_unit_test(_test_node_add_entry),*/
		cmocka_unit_test(_test_node_delete_entry),
		cmocka_unit_test(_test_entry_compare),
		cmocka_unit_test(_test_node_calculate_MBR_Leaf),
		/*cmocka_unit_test(_test_node_calculate_MBR_Non_Leaf),*/
		cmocka_unit_test(_test_node_choose_optimal_entry)
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}