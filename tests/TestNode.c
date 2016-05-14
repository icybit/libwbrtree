#include "TestNode.h"
#include <stdarg.h>
#include <stddef.h>
#ifdef DEBUG
#include <stdio.h>
#endif
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
static size_t serializer(rt_entry_t *entry, uint8_t **buffer);

void _test_context_create() {
	rt_ctx_t *context;
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	uint8_t m = 4, M = 12, dimension = 2;
	float alloc_factor = 4.0f;

	context = context_create(m, M, dimension, serializer, alloc_factor, space_MBR);

	assert_int_equal(context->m, m);
	assert_int_equal(context->M, M);
	assert_true(context->alloc_factor == 4.0f);
	assert_ptr_equal(context->space, space_MBR);

	context_destroy(&context);
}

void _test_entry_create() {	
	rt_rect_t *rectangle = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_entry_t *entry;
	uint8_t tuple = 4;	

	entry = entry_create(&tuple, rectangle);

	assert_ptr_equal(entry->tuple, &tuple);
	assert_ptr_equal(entry->MBR, rectangle);

	entry_destroy(&entry);
}

void _test_node_create() {
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_ctx_t *context;
	rt_node_t *node;
	uint8_t dim = 2, m = 4, M = 12, level = 0;	
	float alloc_factor = 4;

	context = context_create(m, M, dim, serializer, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle, level);
		
	assert_ptr_equal(node->context, context);
	assert_ptr_equal(node->MBR, rectangle);
	assert_int_equal(node->level, level);
	assert_null(node->parent);	
	assert_non_null(node->entries);
	assert_int_equal(node->capacity, MAX(0, NALLOC(context->m, context->M, context->alloc_factor)));

	node_destroy(&node);
	context_destroy(&context);
}

void _test_node_is_leaf() {
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_ctx_t *context;
	rt_node_t *node;
	uint8_t m = 4, M = 12, level = 0, dim = 2;
	float alloc_factor = 4.0f;

	context = context_create(m, M, dim, serializer, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle, level);

	assert_true(node_is_leaf(node));

	node->level = 1;

	assert_false(node_is_leaf(node));

	node_destroy(&node);
	context_destroy(&context);
}

void _test_node_is_root() {	
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_ctx_t *context;
	rt_node_t *node;
	uint8_t m = 4, M = 12, level = 0, dim = 2;
	float alloc_factor = 4.0f;

	context = context_create(m, M, dim, serializer, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle, level);

	assert_true(node_is_root(node));

	node->parent = node;

	assert_false(node_is_root(node));

	node_destroy(&node);
	context_destroy(&context);
}

void _test_node_add_entry() {
	rt_rect_t *space_MBR = create_rectangle_2d(1.0f, 1.0f, 3.0f, 3.0f); 
	rt_rect_t *rectangle_a = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f); 
	rt_rect_t *rectangle_b = create_rectangle_2d(1.0f, 1.0f, 3.0f, 3.0f);
	rt_ctx_t *context;
	rt_node_t *node;
	rt_entry_t *entry;
	uint8_t index, dim = 2, m = 4, M = 12, level = 0;
	uint8_t tuple = 4;
	float alloc_factor = 2.0f;	

	context = context_create(m, M, dim, serializer, alloc_factor, space_MBR);
	entry = entry_create(&tuple, rectangle_b);
	node = node_create(context, NULL, NULL, 0, rectangle_a, level);

	assert_true(node_add_entry(node, entry));
	assert_ptr_equal(node->entries[0], entry);

	for (index = 0; index < dim; index++) {
		assert_true(node->MBR->low[index] == rectangle_a->low[index]);
		assert_true(node->MBR->high[index] == rectangle_b->high[index]);
	}	

	node_destroy(&node);
	context_destroy(&context);
}

void _test_node_delete_entry() 
{
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *MBRs[4];
	rt_ctx_t *context;
	rt_entry_t *entries[4];
	rt_node_t *node;
	uint8_t index, dim = 2, m = 4, M = 12, level = 0, tuples[] = { 1, 2, 3, 4 };
	float alloc_factor = 2.0f;

	for (index = 0; index < 4; index++)
	{
		MBRs[index] = create_rectangle_2d(1.0f, 1.0f, 3.0f, 3.0f);
	}

	context = context_create(m, M, dim, serializer, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle, level);

	for (index = 0; index < 4; index++) {
		entries[index] = entry_create(&tuples[index], MBRs[index]);
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

	node_destroy(&node);	
	context_destroy(&context);

	for (index = 0; index < 4; index++)
	{
		entry_destroy(&(entries[index]));
	}
}

void _test_node_adjust_MBR() {
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle_a = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *rectangle_b = create_rectangle_2d(1.0f, 1.0f, 3.0f, 3.0f);
	rt_ctx_t *context;
	rt_node_t *node;
	rt_entry_t *entry;
	uint8_t index, dim = 2, m = 4, M = 12, level = 0, tuple = 4;
	float alloc_factor = 4.0f;	

	context = context_create(m, M, dim, serializer, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle_a, level);
	entry = entry_create(&tuple, rectangle_b);
		
	node_adjust_MBR(node, entry);
	for (index = 0; index < dim; index++) {
		assert_true(node->MBR->low[index] == rectangle_a->low[index]);
		assert_true(node->MBR->high[index] == rectangle_b->high[index]);
	}

	node_destroy(&node);
	entry_destroy(&entry);
	context_destroy(&context);
}

void _test_node_choose_optimal_entry() {	
	rt_rect_t *space, *entry_MBRs[6], *node_MBRs[3];
	rt_ctx_t *context;
	rt_entry_t *entries[6], **node_entries_1, **node_entries_2, **node_entries_3;

	rt_node_t *node, *node_1, *node_2;
	uint8_t index, dim = 2, m = 4, M = 12, tuples[6], leaf_level = 0, root_level = 1;
	float alloc_factor = 4.0f;

	for (index = 0; index < 6; index++) 
	{
		tuples[index] = index + 1;
	}	

	node_entries_1 = malloc(sizeof(void *) * 2);
	node_entries_2 = malloc(sizeof(void *) * 3);
	node_entries_3 = malloc(sizeof(void *) * 2);

	space = create_rectangle_2d(0.0f, 0.0f, 9.0f, 11.0f);
	context = context_create(m, M, dim, serializer, alloc_factor, space);	

	node_MBRs[0] = create_rectangle_2d(0.0f, 0.0f, 0.0f, 0.0f);
	entry_MBRs[0] = create_rectangle_2d(0.0f, 8.0f, 3.0f, 11.0f);
	entry_MBRs[1] = create_rectangle_2d(6.0f, 8.0f, 9.0f, 11.0f);
	entry_MBRs[2] = create_rectangle_2d(0.0f, 4.0f, 3.0f, 7.0f);
	entry_MBRs[3] = create_rectangle_2d(6.0f, 4.0f, 9.0f, 7.0f);
	entry_MBRs[4] = create_rectangle_2d(0.0f, 0.0f, 3.0f, 3.0f);
	entry_MBRs[5] = create_rectangle_2d(6.0f, 0.0f, 9.0f, 3.0f);

	node_MBRs[1] = create_rectangle_2d(0.0f, 0.0f, 0.0f, 0.0f);
	node_MBRs[2] = create_rectangle_2d(0.0f, 0.0f, 0.0f, 0.0f);

	for (index = 0; index < 6; index++)
	{
		entries[index] = entry_create(&tuples[index], entry_MBRs[index]);
	}	
	
	node_entries_2[0] = entries[0];
	node_entries_2[1] = entries[2];
	node_entries_2[2] = entries[4];

	node_entries_3[0] = entries[1];
	node_entries_3[1] = entries[3];

	node_1 = node_create(context, NULL, (void **)node_entries_2, 3, node_MBRs[1], leaf_level);
	node_2 = node_create(context, NULL, (void **)node_entries_3, 2, node_MBRs[2], leaf_level);

	node_entries_1[0] = (void *)node_1;
	node_entries_1[1] = (void *)node_2;

	node = node_create(context, NULL, (void **)node_entries_1, 2, node_MBRs[0], root_level);		

	for (index = 0; index < dim; index++)
	{
	   assert_true(node->MBR->low[index] == entry_MBRs[4]->low[index]);
	   assert_true(node->MBR->high[index] == entry_MBRs[1]->high[index]);
	}	

	assert_ptr_equal(node_choose_optimal_entry(node, entries[5]), node_2);

	node_destroy(&node);	
	node_destroy(&node_1);	
	node_destroy(&node_2);	
	rtree_entry_destroy(&(entries[5]));
	context_destroy(&context);
}

void _test_entry_compare() {
	rt_rect_t *rectangle_1 = create_rectangle_2d(1.0f, 1.0f, 3.0f, 3.0f);
	rt_rect_t *rectangle_2 = create_rectangle_2d(0.0f, 2.0f, 4.0f, 2.0f);
	rt_entry_t *entry_1, *entry_2;
	uint8_t dim_1 = 0, dim_2 = 1, tuples[] = { 1, 2 };

	entry_1 = entry_create(&tuples[0], rectangle_1);
	entry_2 = entry_create(&tuples[1], rectangle_2);
	
	assert_true(entry_compare(&entry_1, &entry_2, &dim_1) > 0);
	assert_true(entry_compare(&entry_1, &entry_2, &dim_2) < 0);
	assert_true(entry_compare(&entry_2, &entry_1, &dim_1) < 0);
	assert_true(entry_compare(&entry_2, &entry_1, &dim_2) > 0);
	assert_true(entry_compare(&entry_2, &entry_2, &dim_1) == 0);
	assert_true(entry_compare(&entry_2, &entry_2, &dim_2) == 0);

	entry_destroy(&entry_1);
	entry_destroy(&entry_2);
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
	
	context = context_create(m, M, dim, serializer, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, rectangle_0, level);

	entry_1 = entry_create(&tuples[0], rectangle_1);
	entry_2 = entry_create(&tuples[1], rectangle_2);
	entry_3 = entry_create(&tuples[2], rectangle_3);
	
	node_add_entry(node, entry_1);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low[0] == 3);
	assert_true(node->MBR->low[1] == -1);
	assert_true(node->MBR->high[0] == 5);
	assert_true(node->MBR->high[1] == 1);

	node_add_entry(node, entry_2);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low[0] == -1);
	assert_true(node->MBR->low[1] == -1);
	assert_true(node->MBR->high[0] == 5);
	assert_true(node->MBR->high[1] == 1);

	node_add_entry(node, entry_3);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low[0] == -1);
	assert_true(node->MBR->low[1] == -1);
	assert_true(node->MBR->high[0] == 5);
	assert_true(node->MBR->high[1] == 7);

	node_destroy(&node);	
	context_destroy(&context);
}

void _test_node_calculate_MBR_Non_Leaf() {
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_rect_t *node_MBRs[4];
	rt_rect_t *entry_MBRs[6];		
	rt_ctx_t *context;
	rt_entry_t *entries[6], **node_entries_1, **node_entries_2, **node_entries_3;
	rt_node_t *node, *node_1, *node_2, *node_3;
	uint8_t index, dim = 2, m = 2, M = 4, leaf_level = 0, root_level = 1, tuples[6];
	float alloc_factor = 4.0f;	

	for (index = 0; index < 6; index++)
	{
		tuples[index] = index + 1;
	}
	
	for (index = 0; index < 4; index++)
	{
		node_MBRs[index] = create_rectangle_2d(0.0f, 0.0f, 0.0f, 0.0f);
	}

	entry_MBRs[0] = create_rectangle_2d(3.0f, -1.0f, 5.0f, 1.0f);
	entry_MBRs[1] = create_rectangle_2d(-1.0f, -1.0f, 1.0f, 1.0f);
	entry_MBRs[2] = create_rectangle_2d(-1.0f, 5.0f, 2.0f, 7.0f);
	entry_MBRs[3] = create_rectangle_2d(5.0f, 2.0f, 7.0f, 4.0f);
	entry_MBRs[4] = create_rectangle_2d(4.0f, 7.0f, 6.0f, 9.0f);
	entry_MBRs[5] = create_rectangle_2d(-2.0f, 2.0f, 0.0f, 4.0f);

	node_entries_1 = malloc(sizeof(void *) * 2);
	node_entries_2 = malloc(sizeof(void *) * 2);
	node_entries_3 = malloc(sizeof(void *) * 2);

	context = context_create(m, M, dim, serializer, alloc_factor, space_MBR);
	node = node_create(context, NULL, NULL, 0, node_MBRs[0], root_level);

	for (index = 0; index < 6; index++)
	{
		entries[index] = entry_create(&tuples[index], entry_MBRs[index]);
	}
	
	node_entries_1[0] = entries[0];
	node_entries_1[1] = entries[1];
	node_entries_2[0] = entries[2];
	node_entries_2[1] = entries[3];
	node_entries_3[0] = entries[4];
	node_entries_3[1] = entries[5];

	node_1 = node_create(context, node, (void **)node_entries_1, 2, node_MBRs[1], leaf_level);
	node_2 = node_create(context, node, (void **)node_entries_2, 2, node_MBRs[2], leaf_level);
	node_3 = node_create(context, node, (void **)node_entries_3, 2, node_MBRs[3], leaf_level);

	node_add_entry(node, node_1);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low[0] == -1);
	assert_true(node->MBR->low[1] == -1);
	assert_true(node->MBR->high[0] == 5);
	assert_true(node->MBR->high[1] == 1);

	node_add_entry(node, node_2);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low[0] == -1);
	assert_true(node->MBR->low[1] == -1);
	assert_true(node->MBR->high[0] == 7);
	assert_true(node->MBR->high[1] == 7);

	node_add_entry(node, node_3);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low[0] == -2);
	assert_true(node->MBR->low[1] == -1);
	assert_true(node->MBR->high[0] == 7);
	assert_true(node->MBR->high[1] == 9);

	node_destroy(&node);	
	context_destroy(&context);
}

void _test_node_destroy()
{
	rt_rect_t *space_MBR = create_rectangle_2d(1.0f, 1.0f, 3.0f, 3.0f); 
	rt_rect_t *rectangle_a = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f); 
	rt_rect_t *rectangle_b = create_rectangle_2d(1.0f, 1.0f, 3.0f, 3.0f);
	rt_ctx_t *context;
	rt_node_t *node;
	rt_entry_t *entry;
	uint8_t dim = 2, m = 4, M = 12, level = 0;
	uint8_t tuple = 4;
	float alloc_factor = 2.0f;	

	context = context_create(m, M, dim, serializer, alloc_factor, space_MBR);
	entry = entry_create(&tuple, rectangle_b);
	node = node_create(context, NULL, NULL, 0, rectangle_a, level);

	node_add_entry(node, entry);	

	node_destroy(&node);
	assert_null(node);

	context_destroy(&context);
}

void _test_context_destroy() 
{
	rt_ctx_t *context;
	rt_rect_t *space_MBR = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	uint8_t m = 4, M = 12, dimension = 2;
	float alloc_factor = 4.0f;

	context = context_create(m, M, dimension, serializer, alloc_factor, space_MBR);	

	context_destroy(&context);
	assert_null(context);
}

void _test_entry_destroy() {	
	rt_rect_t *rectangle = create_rectangle_2d(0.0f, 0.0f, 2.0f, 2.0f);
	rt_entry_t *entry;
	uint8_t tuple = 4;	

	entry = entry_create(&tuple, rectangle);	

	entry_destroy(&entry);
	assert_null(entry);
}

static rt_rect_t * create_rectangle_2d(float low_x, float low_y, float high_x, float high_y)
{
	float *low = initialize_coordinates(low_x, low_y);
	float *high = initialize_coordinates(high_x, high_y);

	return rectangle_create(low, high, 2);
}

static float * initialize_coordinates(float x, float y) 
{
	float *coords = malloc(2 * sizeof(float));

	coords[0] = x;
	coords[1] = y;

	return coords;
}

static size_t serializer(rt_entry_t *entry, uint8_t **buffer)
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
		cmocka_unit_test(_test_node_add_entry),
		cmocka_unit_test(_test_node_delete_entry),
		cmocka_unit_test(_test_entry_compare),
		cmocka_unit_test(_test_node_calculate_MBR_Leaf),
		cmocka_unit_test(_test_node_calculate_MBR_Non_Leaf),
		cmocka_unit_test(_test_node_choose_optimal_entry),
		cmocka_unit_test(_test_node_destroy),
		cmocka_unit_test(_test_context_destroy),
		cmocka_unit_test(_test_entry_destroy)
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}