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

void _test_entry_create() {	
	uint8_t tuple = 4, dim = 2;
	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 2.0f };
	rt_rect_t rectangle;
	rt_entry_t entry;	
	
	rectangle_create(&rectangle, low, high, dim);	

	// Test entry_create
	entry_create(&entry, &tuple, &rectangle);

	assert_ptr_equal(entry.tuple, &tuple);
	assert_ptr_equal(entry.MBR, &rectangle);	
}

//TODO: Cover node create with more tests
void _test_node_create() {
	uint8_t tuple = 4, dim = 2, m = 4, M = 12, level = 0;	
	float alloc_factor = 4;
	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 2.0f };
	rt_rect_t rectangle;	
	rt_ctx_t context;	
	rt_node_t node;

	context_create(&context, m, M, dim, 35, alloc_factor, &rectangle);
	node_create(&node, &context, NULL, NULL, 0, context.space, level);
		
	assert_ptr_equal(node.context, &context);
	assert_ptr_equal(node.MBR, context.space);
	assert_int_equal(node.level, level);
	assert_null(node.parent);	
	assert_non_null(node.entries);
	assert_int_equal(node.capacity, MAX(0, NALLOC(context.m, context.M, context.alloc_factor)));
}

void _test_node_is_leaf() {
	uint8_t m = 4, M = 12, level = 0, dim = 2;
	size_t buffer_size = 35;
	float alloc_factor = 4.0f;
	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 2.0f };
	rt_rect_t rectangle;
	rt_ctx_t context;
	rt_node_t node;

	rectangle_create(&rectangle, low, high, dim);
	context_create(&context, m, M, dim, buffer_size, alloc_factor, &rectangle);
	
	node_create(&node, &context, NULL, NULL, 0, context.space, level);

	// node_is_leaf - Should return 1
	assert_true(node_is_leaf(&node));

	node.level = 1;

	// node_is_leaf - Should return 0
	assert_false(node_is_leaf(&node));
}

void _test_node_is_root() {	
	uint8_t m = 4, M = 12, level = 0, dim = 2;
	size_t buffer_size = 35;
	float alloc_factor = 4.0f;
	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 2.0f };
	rt_rect_t rectangle;
	rt_ctx_t context;
	rt_node_t node;

	rectangle_create(&rectangle, low, high, dim);
	context_create(&context, m, M, dim, buffer_size, alloc_factor, &rectangle);

	node_create(&node, &context, NULL, NULL, 0, context.space, level);

	assert_true(node_is_root(&node));

	node.parent = &node;

	assert_false(node_is_root(&node));
}

void _test_node_add_entry() {
	uint8_t index, success = 0, dim = 2, tuple = 4, m = 4, M = 12, level = 0;
	size_t buffer_size = 35;
	float alloc_factor = 2.0f;
	float low_a[] = { 0.0f, 0.0f };
	float high_a[] = { 2.0f, 2.0f };
	float low_b[] = { 1.0f, 1.0f };
	float high_b[] = { 3.0f, 3.0f };
	rt_rect_t rectangle_a, rectangle_b;
	rt_ctx_t context;
	rt_node_t node;
	rt_entry_t entry;

	rectangle_create(&rectangle_a, low_a, high_a, dim);
	rectangle_create(&rectangle_b, low_b, high_b, dim);
	context_create(&context, m, M, dim, buffer_size, alloc_factor, &rectangle_a);
	node_create(&node, &context, NULL, NULL, 0, context.space, level);
	entry_create(&entry, &tuple, &rectangle_b);
		
	// Test node_add_entry
	success = node_add_entry(&node, &entry);
	assert_true(success);
	assert_ptr_equal(node.entries[0], &entry);

	for (index = 0; index < dim; index++) {
		assert_true(node.MBR->low[index] == low_a[index]);
		assert_true(node.MBR->high[index] == high_b[index]);
	}

	// Test memory realocation
	for (index = 0; index < node.capacity; index++) {
		if (index < M - 1) {
			assert_true(node_add_entry(&node, &entry));
			assert_ptr_equal(node.entries[index], &entry);
		}
		else {
			assert_false(node_add_entry(&node, &entry));
		}		
	}
}

void _test_node_delete_entry() {
	uint8_t index, dim = 2, m = 4, M = 12, level = 0;
	size_t buffer_size = 35;
	float alloc_factor = 2.0f;
	float low_a[] = { 0.0f, 0.0f };
	float high_a[] = { 2.0f, 2.0f };
	float low_b[] = { 1.0f, 1.0f };
	float high_b[] = { 3.0f, 3.0f };
	uint8_t tuples[] = { 1, 2, 3, 4 };
	rt_entry_t entries[4];
	rt_rect_t rectangle_a, rectangle_b;
	rt_ctx_t context;
	rt_node_t node;

	rectangle_create(&rectangle_a, low_a, high_a, dim);
	rectangle_create(&rectangle_b, low_b, high_b, dim);
	context_create(&context, m, M, dim, buffer_size, alloc_factor, &rectangle_a);
	node_create(&node, &context, NULL, NULL, 0, context.space, level);
		
	// Entry setup
	for (index = 0; index < 4; index++) {
		entry_create(&entries[index], &tuples[index], &rectangle_b);
		node_add_entry(&node, &entries[index]);
	}	

	// Test node_delete_entry

	// Removed entry_2, sequence should be 1, 3, 4
	node_delete_entry(&node, &entries[1]);
	assert_ptr_equal(node.entries[0], &entries[0]);
	assert_ptr_equal(node.entries[1], &entries[2]);
	assert_ptr_equal(node.entries[2], &entries[3]);
	assert_int_equal(node.count, 3);

	// Removed entry_1, sequence should be 3, 4
	node_delete_entry(&node, &entries[0]);
	assert_ptr_equal(node.entries[0], &entries[2]);
	assert_ptr_equal(node.entries[1], &entries[3]);
	assert_int_equal(node.count, 2);

	// Removed entry_4, sequence should be 3
	node_delete_entry(&node, &entries[3]);
	assert_ptr_equal(node.entries[0], &entries[2]);
	assert_int_equal(node.count, 1);

	// Removed entry_3, there should be 0 entries
	node_delete_entry(&node, &entries[2]);
	assert_int_equal(node.count, 0);
}

void _test_node_adjust_MBR() {
	uint8_t index, dim = 2, tuple = 4, m = 4, M = 12, level = 0;
	size_t buffer_size = 35;
	float alloc_factor = 4.0f;
	float low_a[] = { 0.0f, 0.0f };
	float high_a[] = { 2.0f, 2.0f };
	float low_b[] = { 1.0f, 1.0f };
	float high_b[] = { 3.0f, 3.0f };
	rt_rect_t rectangle_a, rectangle_b;
	rt_ctx_t context;
	rt_node_t node;
	rt_entry_t entry;

	rectangle_create(&rectangle_a, low_a, high_a, dim);
	rectangle_create(&rectangle_b, low_b, high_b, dim);
	context_create(&context, m, M, dim, buffer_size, alloc_factor, &rectangle_a);
	node_create(&node, &context, NULL, NULL, 0, context.space, level);
	entry_create(&entry, &tuple, &rectangle_b);
		
	// Test node_adjust_MBR
	node_adjust_MBR(&node, &entry);
	for (index = 0; index < dim; index++) {
		assert_true(node.MBR->low[index] == low_a[index]);
		assert_true(node.MBR->high[index] == high_b[index]);
	}
}

void _test_node_choose_optimal_entry() {
	uint8_t dim = 2, tuple = 1, m = 4, M = 12, leaf_level = 0, root_level = 1;
	size_t buffer_size = 35;
	float alloc_factor = 4.0f;

	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 2.0f };

	float low_1[] = { 3.0f, -1.0f };
	float high_1[] = { 5.0f, 1.0f };

	float low_2[] = { -1.0f, -1.0f };
	float high_2[] = { 1.0f, 1.0f };

	float low_3[] = { -1.0f, 5.0f };
	float high_3[] = { 2.0f, 7.0f };

	float low_4[] = { -3.0f, 0.0f };
	float high_4[] = { -2.0f, 1.0f };

	rt_ctx_t context;
	rt_entry_t entry;

	rt_node_t node;	
	rt_node_t node_1;	
	rt_node_t node_2;	
	rt_node_t node_3;

	rt_rect_t rectangle;
	rt_rect_t rectangle_1;
	rt_rect_t rectangle_2;
	rt_rect_t rectangle_3;
	rt_rect_t rectangle_4;

	// Create root node
	rectangle_create(&rectangle, low, high, dim);
	context_create(&context, m, M, dim, buffer_size, alloc_factor, &rectangle);
	node_create(&node, &context, NULL, NULL, 0, context.space, root_level);

	// Create leaf nodes
	rectangle_create(&rectangle_1, low_1, high_1, dim);
	node_create(&node_1, &context, NULL, NULL, 0, &rectangle_1, leaf_level);

	rectangle_create(&rectangle_2, low_2, high_2, dim);
	node_create(&node_2, &context, NULL, NULL, 0, &rectangle_2, leaf_level);

	rectangle_create(&rectangle_3, low_3, high_1, dim);
	node_create(&node_3, &context, NULL, NULL, 0, &rectangle_3, leaf_level);

	// Create entry
	rectangle_create(&rectangle_4, low_4, high_4, dim);
	entry_create(&entry, &tuple, &rectangle_4);

	// Add leaf nodes to root
	node_add_entry(&node, &node_1);
	node_add_entry(&node, &node_2);
	node_add_entry(&node, &node_3);

	// Test node_choose_optimal_entry
	assert_ptr_equal(node_choose_optimal_entry(&node, &entry), &node_2);
}

void _test_entry_compare() {	
	uint8_t dim = 2, dim_1 = 0, dim_2 = 1;
	float low_1[] = { 1.0f, 1.0f };
	float high_1[] = { 3.0f, 3.0f };	

	float low_2[] = { 0.0f, 2.0f };
	float high_2[] = { 4.0f, 2.0f };
	
	uint8_t tuples[] = { 1, 2 };

	rt_entry_t entry_1;
	rt_entry_t entry_2;
	rt_entry_t *entry_ptr_1 = &entry_1;
	rt_entry_t *entry_ptr_2 = &entry_2;

	rt_rect_t rectangle_1, rectangle_2;

	rectangle_create(&rectangle_1, low_1, high_1, dim);
	rectangle_create(&rectangle_2, low_2, high_2, dim);

	entry_create(&entry_1, &tuples[0], &rectangle_1);
	entry_create(&entry_2, &tuples[1], &rectangle_2);	
	
	assert_true(entry_compare(&dim_1, &entry_ptr_1, &entry_ptr_2) > 0);
	assert_true(entry_compare(&dim_2, &entry_ptr_1, &entry_ptr_2) < 0);
	assert_true(entry_compare(&dim_1, &entry_ptr_2, &entry_ptr_1) < 0);
	assert_true(entry_compare(&dim_2, &entry_ptr_2, &entry_ptr_1) > 0);
	assert_true(entry_compare(&dim_1, &entry_ptr_2, &entry_ptr_2) == 0);
	assert_true(entry_compare(&dim_2, &entry_ptr_2, &entry_ptr_2) == 0);
}

// For leaf node
void _test_node_calculate_MBR_1() {
	uint8_t dim = 2, tuple = 1, m = 2, M = 4, level = 0;
	size_t buffer_size = 35;
	float alloc_factor = 4.0f;

	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 2.0f };

	float low_1[] = { 3.0f, -1.0f };
	float high_1[] = { 5.0f, 1.0f };

	float low_2[] = { -1.0f, -1.0f };
	float high_2[] = { 1.0f, 1.0f };

	float low_3[] = { -1.0f, 5.0f };
	float high_3[] = { 2.0f, 7.0f };

	rt_rect_t rectangle;
	rt_rect_t rectangle_1;
	rt_rect_t rectangle_2;
	rt_rect_t rectangle_3;
	
	rt_ctx_t context;
	rt_node_t node;
	rt_entry_t entry_1;
	rt_entry_t entry_2;
	rt_entry_t entry_3;
	
	// Create root node
	rectangle_create(&rectangle, low, high, dim);
	context_create(&context, m, M, dim, buffer_size, alloc_factor, &rectangle);
	node_create(&node, &context, NULL, NULL, 0, context.space, level);

	// Create entries
	rectangle_create(&rectangle_1, low_1, high_1, dim);
	entry_create(&entry_1, &tuple, &rectangle_1);

	rectangle_create(&rectangle_2, low_2, high_2, dim);
	entry_create(&entry_2, &tuple, &rectangle_2);

	rectangle_create(&rectangle_3, low_3, high_3, dim);
	entry_create(&entry_3, &tuple, &rectangle_3);
	
	// Test node_calculate_MBR
	node_add_entry(&node, &entry_1);
	node_calculate_MBR(node.MBR, &node);
	assert_true(node.MBR->low[0] == 0);
	assert_true(node.MBR->low[1] == -1);
	assert_true(node.MBR->high[0] == 5);
	assert_true(node.MBR->high[1] == 2);

	node_add_entry(&node, &entry_2);
	node_calculate_MBR(node.MBR, &node);
	assert_true(node.MBR->low[0] == -1);
	assert_true(node.MBR->low[1] == -1);
	assert_true(node.MBR->high[0] == 5);
	assert_true(node.MBR->high[1] == 2);

	node_add_entry(&node, &entry_3);
	node_calculate_MBR(node.MBR, &node);
	assert_true(node.MBR->low[0] == -1);
	assert_true(node.MBR->low[1] == -1);
	assert_true(node.MBR->high[0] == 5);
	assert_true(node.MBR->high[1] == 7);
}

// For non-leaf node
void _test_node_calculate_MBR_2() {
	uint8_t dim = 2, tuple = 1, m = 1, M = 2, leaf_level = 0, root_level = 1;
	size_t buffer_size = 35;
	float alloc_factor = 4.0f;

	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 2.0f };

	float low_1[] = { 3.0f, -1.0f };
	float high_1[] = { 5.0f, 1.0f };

	float low_2[] = { -1.0f, -1.0f };
	float high_2[] = { 1.0f, 1.0f };

	float low_3[] = { -1.0f, 5.0f };
	float high_3[] = { 2.0f, 7.0f };

	rt_rect_t rectangle;
	rt_rect_t rectangle_1;
	rt_rect_t rectangle_2;
	rt_rect_t rectangle_3;

	rt_ctx_t context;
	rt_node_t node;
	rt_node_t node_1;
	rt_node_t node_2;
	rt_node_t node_3;

	rt_entry_t entry_1;
	rt_entry_t entry_2;
	rt_entry_t entry_3;

	rt_entry_t *entry_ptr_1 = &entry_1;
	rt_entry_t *entry_ptr_2 = &entry_2;
	rt_entry_t *entry_ptr_3 = &entry_3;

	// Create root node
	rectangle_create(&rectangle, low, high, dim);
	context_create(&context, m, M, dim, buffer_size, alloc_factor, &rectangle);
	node_create(&node, &context, NULL, NULL, 0, context.space, root_level);

	// Create entry nodes
	rectangle_create(&rectangle_1, low_1, high_1, dim);
	rectangle_create(&rectangle_2, low_2, high_2, dim);
	rectangle_create(&rectangle_3, low_3, high_3, dim);
	entry_create(entry_ptr_1, &tuple, &rectangle_1);
	entry_create(entry_ptr_2, &tuple, &rectangle_2);
	entry_create(entry_ptr_3, &tuple, &rectangle_3);
		
	node_create(&node_1, &context, &node, &entry_ptr_1, 1, &rectangle_1, leaf_level);
	node_create(&node_2, &context, &node, &entry_ptr_2, 1, &rectangle_2, leaf_level);
	node_create(&node_3, &context, &node, &entry_ptr_3, 1, &rectangle_3, leaf_level);

	// Test node_calculate_MBR
	node_add_entry(&node, &node_1);
	node_calculate_MBR(node.MBR, &node);
	assert_true(node.MBR->low[0] == 0);
	assert_true(node.MBR->low[1] == -1);
	assert_true(node.MBR->high[0] == 5);
	assert_true(node.MBR->high[1] == 2);

	node_add_entry(&node, &node_2);
	node_calculate_MBR(node.MBR, &node);
	assert_true(node.MBR->low[0] == -1);
	assert_true(node.MBR->low[1] == -1);
	assert_true(node.MBR->high[0] == 5);
	assert_true(node.MBR->high[1] == 2);

	node_add_entry(&node, &node_3);
	node_calculate_MBR(node.MBR, &node);
	assert_true(node.MBR->low[0] == -1);
	assert_true(node.MBR->low[1] == -1);
	assert_true(node.MBR->high[0] == 5);
	assert_true(node.MBR->high[1] == 7);
}
//
//void _test_node_split_node() {
//
//}

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
		cmocka_unit_test(_test_node_calculate_MBR_1),
		cmocka_unit_test(_test_node_calculate_MBR_2),
		cmocka_unit_test(_test_node_choose_optimal_entry)
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}