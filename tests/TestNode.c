#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../include/Common.h"
#include "../include/Rectangle.h"
#include "../include/Node.h"
#include "Helpers.h"
#include "TestNode.h"

void _test_context_create() {
	rt_ctx_t context;
	rt_rect_t rectangle;
	uint8_t m = 4, M = 12, dimension = 2;
	size_t buffer_size = 35;
	float alloc_factor = 4.0f;
	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 2.0f };

	rectangle_create(&rectangle, low, high, dimension);
	context_create(&context, m, M, dimension, buffer_size, alloc_factor, &rectangle);

	assert_int_equal(context.m, m);
	assert_int_equal(context.M, M);
	assert_true(context.alloc_factor == 4.0f);
	assert_ptr_equal(context.space, &rectangle);
}

/* TODO: The following tests should be refactored to remove superfluous heap allocations and to comply with -Pedantic rules */

/*void _test_entry_create() {	
	// Create Rectangle
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };
	rt_rect_t *rectangle = create_2d_rectangle(coords_low, coords_high);

	// Create Entry
	rt_entry_t *entry = malloc(sizeof(rt_entry_t));
	uint8_t tuple = 4;

	entry_create(entry, tuple, rectangle);
	assert_true(entry->tuple == tuple);
	assert_ptr_equal(entry->MBR, rectangle);

	destroy_rectangle(rectangle);
	free(entry);
}

//TODO: Cover node create with more tests
void _test_node_create() {
	int dim = 2;

	// Create Rectangle
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };
	rt_rect_t *rectangle = create_2d_rectangle(coords_low, coords_high);

	// Create Context
	rt_ctx_t *context = malloc(sizeof(rt_ctx_t));
	float alloc_factor = 4;
	int m = 4;
	int M = 12;

	context_create(context, m, M, dim, 35, alloc_factor, rectangle);
	
	// Create Node
	rt_node_t *node = malloc(sizeof(rt_node_t));
	int level = 0;
	node_create(node, context, NULL, NULL, 0, context->space, level);
	assert_ptr_equal(node->context, context);
	assert_ptr_equal(node->MBR, context->space);
	assert_int_equal(node->level, level);
	assert_null(node->parent);	
	assert_non_null(node->entries);
	assert_int_equal(node->capacity, MAX(0, NALLOC(context->m, context->M, context->alloc_factor)));

	destroy_rectangle(rectangle);
	free(context);
	free(node);
}

void _test_node_is_leaf() {
	// Create Node
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };
	rt_node_t *node = create_2d_node(4, 12, 4, coords_low, coords_high, 0);	

	// node_is_leaf - Should return 1
	assert_true(node_is_leaf(node));

	node->level = 1;

	// node_is_leaf - Should return 0
	assert_false(node_is_leaf(node));
	
	destroy_context(node->context);
	free(node->entries);
	free(node->parent);
	free(node);
}

void _test_node_is_root() {	
	// Create Node
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };
	rt_node_t *node = create_2d_node(4, 12, 4, coords_low, coords_high, 0);

	assert_true(node_is_root(node));

	node->parent = node;

	assert_false(node_is_root(node));

	destroy_context(node->context);
	free(node->entries);	
	free(node);
}

void _test_node_add_entry() {
	int dim = 2;

	// Node setup
	rt_rect_t *rectangle_a = malloc(sizeof(rt_rect_t));

	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 0, 0 };
	float coords_high_a[] = { 2, 2 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);

	rt_ctx_t *context = malloc(sizeof(rt_ctx_t));
	float alloc_factor = 2;
	int m = 4;
	int M = 12;

	context_create(context, m, M, dim, 35, alloc_factor, rectangle_a);

	rt_node_t *node = malloc(sizeof(rt_node_t));
	int level = 0;
	node_create(node, context, NULL, NULL, 0, context->space, level);

	// Entry setup
	rt_rect_t *rectangle_b = malloc(sizeof(rt_rect_t));

	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[] = { 1, 1 };
	float coords_high_b[] = { 3, 3 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);
	
	
	// Create Entry
	rt_entry_t *entry = malloc(sizeof(rt_entry_t));
	void *tuple = 4;

	entry_create(entry, tuple, rectangle_b);

	// Test node_add_entry
	int success = node_add_entry(node, entry);
	assert_true(success);
	assert_ptr_equal(node->entries[0], entry);

	for (int i = 0; i < dim; i++) {
		assert_ptr_equal(node->MBR->low->coords[i], rectangle_a->low->coords[i]);
		assert_ptr_equal(node->MBR->high->coords[i], rectangle_b->high->coords[i]);
	}

	// Test memory realocation
	for (int i = 0; i < node->capacity; i++) {
		if (i < M - 1) {
			assert_true(node_add_entry(node, entry));
			assert_ptr_equal(node->entries[i], entry);
		}
		else {
			assert_false(node_add_entry(node, entry));
		}		
	}

	free(low_a);
	free(high_a);
	free(rectangle_a);

	free(low_b);
	free(high_b);
	free(rectangle_b);

	free(entry);
	free(context);
}

void _test_node_delete_entry() {
	int dim = 2;

	// Node setup
	rt_rect_t *rectangle_a = malloc(sizeof(rt_rect_t));

	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 0, 0 };
	float coords_high_a[] = { 2, 2 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);

	rt_ctx_t *context = malloc(sizeof(rt_ctx_t));
	float alloc_factor = 2;
	int m = 4;
	int M = 12;

	context_create(context, m, M, dim, 35, alloc_factor, rectangle_a);

	rt_node_t *node = malloc(sizeof(rt_node_t));
	int level = 0;
	node_create(node, context, NULL, NULL, 0, context->space, level);

	// Entry setup
	rt_rect_t *rectangle_b = malloc(sizeof(rt_rect_t));

	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[] = { 1, 1 };
	float coords_high_b[] = { 3, 3 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);


	// Create Entries
	rt_entry_t *entry_1 = malloc(sizeof(rt_entry_t));
	rt_entry_t *entry_2 = malloc(sizeof(rt_entry_t));
	rt_entry_t *entry_3 = malloc(sizeof(rt_entry_t));
	rt_entry_t *entry_4 = malloc(sizeof(rt_entry_t));
	void *tuple_1 = 1;
	void *tuple_2 = 2;
	void *tuple_3 = 3;
	void *tuple_4 = 4;

	entry_create(entry_1, tuple_1, rectangle_b);
	entry_create(entry_2, tuple_2, rectangle_b);
	entry_create(entry_3, tuple_3, rectangle_b);
	entry_create(entry_4, tuple_4, rectangle_b);

	node_add_entry(node, entry_1);
	node_add_entry(node, entry_2);
	node_add_entry(node, entry_3);
	node_add_entry(node, entry_4);

	// Test node_delete_entry

	// Removed entry_2, sequence should be 1, 3, 4
	node_delete_entry(node, entry_2);
	assert_ptr_equal(node->entries[0], entry_1);
	assert_ptr_equal(node->entries[1], entry_3);
	assert_ptr_equal(node->entries[2], entry_4);
	assert_int_equal(node->count, 3);

	// Removed entry_1, sequence should be 3, 4
	node_delete_entry(node, entry_1);
	assert_ptr_equal(node->entries[0], entry_3);
	assert_ptr_equal(node->entries[1], entry_4);
	assert_int_equal(node->count, 2);

	// Removed entry_4, sequence should be 3
	node_delete_entry(node, entry_4);
	assert_ptr_equal(node->entries[0], entry_3);
	assert_int_equal(node->count, 1);

	// Removed entry_3, there should be 0 entries
	node_delete_entry(node, entry_3);	
	assert_int_equal(node->count, 0);

	free(low_a);
	free(high_a);
	free(rectangle_a);

	free(low_b);
	free(high_b);
	free(rectangle_b);

	free(entry_1);
	free(entry_2);
	free(entry_3);
	free(entry_4);
	free(context);
}

void _test_node_adjust_MBR() {
	int dim = 2;

	// Node setup
	rt_rect_t *rectangle_a = malloc(sizeof(rt_rect_t));

	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 0, 0 };
	float coords_high_a[] = { 2, 2 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);
		
	rt_ctx_t *context = malloc(sizeof(rt_ctx_t));
	float alloc_factor = 4;
	int m = 4;
	int M = 12;

	context_create(context, m, M, dim, 35, alloc_factor, rectangle_a);
		
	rt_node_t *node = malloc(sizeof(rt_node_t));
	int level = 0;
	node_create(node, context, NULL, NULL, 0, context->space, level);

	// Entry setup
	rt_rect_t *rectangle_b = malloc(sizeof(rt_rect_t));
		
	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[] = { 1, 1 };
	float coords_high_b[] = { 3, 3 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);
	// Create Entry
	rt_entry_t *entry = malloc(sizeof(rt_entry_t));
	void *tuple = 4;

	entry_create(entry, tuple, rectangle_b);

	// Test node_adjust_MBR
	node_adjust_MBR(node, entry);
	for (int i = 0; i < dim; i++) {
		assert_ptr_equal(node->MBR->low->coords[i], rectangle_a->low->coords[i]);
		assert_ptr_equal(node->MBR->high->coords[i], rectangle_b->high->coords[i]);
	}	

	free(low_a);
	free(high_a);
	free(rectangle_a);

	free(low_b);
	free(high_b);
	free(rectangle_b);
	
	free(context);
	free(node);
	free(entry);
}

void _test_node_choose_optimal_entry() {
	float low[] = { 0, 0 };
	float high[] = { 2, 2 };
	rt_node_t *node = create_2d_node(2, 4, 4, low, high, 1);

	float low_1[] = { 3, -1 };
	float high_1[] = { 5, 1 };
	rt_node_t *node_1 = create_2d_node(2, 4, 4, low_1, high_1, 0);

	float low_2[] = { -1, -1 };
	float high_2[] = { 1, 1 };
	rt_node_t *node_2 = create_2d_node(2, 4, 4, low_2, high_2, 0);


	float low_3[] = { -1, 5 };
	float high_3[] = { 2, 7 };
	rt_node_t *node_3 = create_2d_node(2, 4, 4, low_3, high_3, 0);

	float low_4[] = { -3, 0 };
	float high_4[] = { -2, 1 };
	int tuple = 1;
	rt_entry_t *entry = create_2d_entry(&tuple, low_4, high_4);


	node_add_entry(node, node_1);
	node_add_entry(node, node_2);
	node_add_entry(node, node_3);
	assert_ptr_equal(node_choose_optimal_entry(node, entry), node_2);

	node_destroy(node);
}

void _test_entry_compare() {
	float low_1[] = { 1, 1 };
	float high_1[] = { 3, 3 };
	int tuple_1 = 1;
	rt_entry_t *entry_1 = create_2d_entry(tuple_1, low_1, high_1);

	float low_2[] = { 0, 2 };
	float high_2[] = { 4, 2 };
	int tuple_2 = 2;
	rt_entry_t *entry_2 = create_2d_entry(tuple_2, low_2, high_2);

	uint8_t dim_1 = 0;
	uint8_t dim_2 = 1;

	assert_true(entry_1->tuple == 1);
	assert_true(entry_2->tuple == 2);

	assert_true(entry_compare(&dim_1, entry_1, entry_2) > 0);
	assert_true(entry_compare(&dim_2, entry_1, entry_2) < 0);
	assert_true(entry_compare(&dim_1, entry_2, entry_1) < 0);
	assert_true(entry_compare(&dim_2, entry_2, entry_1) > 0);
	assert_true(entry_compare(&dim_1, entry_2, entry_2) == 0);
	assert_true(entry_compare(&dim_2, entry_2, entry_2) == 0);

	destroy_entry(entry_1);
	destroy_entry(entry_2);
}

// For leaf node
void _test_node_calculate_MBR_1() {
	float low[] = { 0, 0 };
	float high[] = { 2, 2 };
	rt_node_t *node = create_2d_node(2, 4, 4, low, high, 0);

	float low_1[] = { 3, -1 };
	float high_1[] = { 5, 1 };
	int tuple_1 = 1;
	rt_entry_t *entry_1 = create_2d_entry(&tuple_1, low_1, high_1);

	float low_2[] = { -1, -1 };
	float high_2[] = { 1, 1 };
	int tuple_2 = 2;
	rt_entry_t *entry_2 = create_2d_entry(&tuple_2, low_2, high_2);

	float low_3[] = { -1, 5 };
	float high_3[] = { 2, 7 };
	int tuple_3 = 3;
	rt_entry_t *entry_3 = create_2d_entry(&tuple_3, low_3, high_3);
	
	node_add_entry(node, entry_1);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low->coords[0] == 0);
	assert_true(node->MBR->low->coords[1] == -1);
	assert_true(node->MBR->high->coords[0] == 5);
	assert_true(node->MBR->high->coords[1] == 2);

	node_add_entry(node, entry_2);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low->coords[0] == -1);
	assert_true(node->MBR->low->coords[1] == -1);
	assert_true(node->MBR->high->coords[0] == 5);
	assert_true(node->MBR->high->coords[1] == 2);

	node_add_entry(node, entry_3);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low->coords[0] == -1);
	assert_true(node->MBR->low->coords[1] == -1);
	assert_true(node->MBR->high->coords[0] == 5);
	assert_true(node->MBR->high->coords[1] == 7);

	node_destroy(node);
}

// For non-leaf node
void _test_node_calculate_MBR_2() {
	float low[] = { 0, 0 };
	float high[] = { 2, 2 };
	rt_node_t *node = create_2d_node(2, 4, 4, low, high, 1);

	float low_1[] = { 3, -1 };
	float high_1[] = { 5, 1 };
	rt_node_t *node_1 = create_2d_node(2, 4, 4, low_1, high_1, 0);

	float low_2[] = { -1, -1 };
	float high_2[] = { 1, 1 };
	rt_node_t *node_2 = create_2d_node(2, 4, 4, low_2, high_2, 0);
	

	float low_3[] = { -1, 5 };
	float high_3[] = { 2, 7 };
	rt_node_t *node_3 = create_2d_node(2, 4, 4, low_3, high_3, 0);
	

	node_add_entry(node, node_1);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low->coords[0] == 0);
	assert_true(node->MBR->low->coords[1] == -1);
	assert_true(node->MBR->high->coords[0] == 5);
	assert_true(node->MBR->high->coords[1] == 2);

	node_add_entry(node, node_2);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low->coords[0] == -1);
	assert_true(node->MBR->low->coords[1] == -1);
	assert_true(node->MBR->high->coords[0] == 5);
	assert_true(node->MBR->high->coords[1] == 2);

	node_add_entry(node, node_3);
	node_calculate_MBR(node->MBR, node);
	assert_true(node->MBR->low->coords[0] == -1);
	assert_true(node->MBR->low->coords[1] == -1);
	assert_true(node->MBR->high->coords[0] == 5);
	assert_true(node->MBR->high->coords[1] == 7);

	node_destroy(node);
}

void _test_node_split_node() {

}*/

int test_node(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(_test_context_create),
		/*cmocka_unit_test(_test_entry_create),
		cmocka_unit_test(_test_node_create),
		cmocka_unit_test(_test_node_is_leaf),
		cmocka_unit_test(_test_node_is_root),
		cmocka_unit_test(_test_node_adjust_MBR),
		cmocka_unit_test(_test_node_add_entry),
		cmocka_unit_test(_test_node_delete_entry),
		//cmocka_unit_test(_test_entry_compare),
		cmocka_unit_test(_test_node_calculate_MBR_1),
		cmocka_unit_test(_test_node_calculate_MBR_2),
		cmocka_unit_test(_test_node_choose_optimal_entry)*/
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}