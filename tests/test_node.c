#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include "Common.h"
#include "Helpers.h"
#include "Rectangle.h"
#include "Node.h"

void test_context_create() {
	int dim = 2;

	// Create Rectangle
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };
	struct Rectangle *rectangle = create_2d_rectangle(coords_low, coords_high);
	
	// Create Context
	struct Context *context = malloc(sizeof(struct Context));
	float alloc_factor = 4;
	int m = 4;
	int M = 12;

	context_create(context, m, M, dim, alloc_factor, rectangle);
	assert_int_equal(context->m, m);
	assert_int_equal(context->M, M);
	assert_true(context->alloc_factor == 4);
	assert_ptr_equal(context->space, rectangle);

	destroy_rectangle(rectangle);
	free(context);
}

void test_entry_create() {	
	// Create Rectangle
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };
	struct Rectangle *rectangle = create_2d_rectangle(coords_low, coords_high);

	// Create Entry
	struct Entry *entry = malloc(sizeof(struct Entry));
	uint8_t tuple = 4;

	entry_create(entry, (void *) tuple, rectangle);
	assert_ptr_equal(entry->tuple, (void *) tuple);
	assert_ptr_equal(entry->MBR, rectangle);

	destroy_rectangle(rectangle);
	free(entry);
}

//TODO: Cover node create with more tests
void test_node_create() {
	int dim = 2;

	// Create Rectangle
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };
	struct Rectangle *rectangle = create_2d_rectangle(coords_low, coords_high);

	// Create Context
	struct Context *context = malloc(sizeof(struct Context));
	float alloc_factor = 4;
	int m = 4;
	int M = 12;

	context_create(context, m, M, dim, alloc_factor, rectangle);
	
	// Create Node
	struct Node *node = malloc(sizeof(struct Node));
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

void test_node_is_leaf() {
	// Create Node
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };
	struct Node *node = create_2d_node(4, 12, 4, coords_low, coords_high, 0);	

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

void test_node_is_root() {	
	// Create Node
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };
	struct Node *node = create_2d_node(4, 12, 4, coords_low, coords_high, 0);

	assert_true(node_is_root(node));

	node->parent = node;

	assert_false(node_is_root(node));

	destroy_context(node->context);
	free(node->entries);	
	free(node);
}

void test_node_add_entry() {
	int dim = 2;

	// Node setup
	struct Rectangle *rectangle_a = malloc(sizeof(struct Rectangle));

	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 0, 0 };
	float coords_high_a[] = { 2, 2 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);

	struct Context *context = malloc(sizeof(struct Context));
	float alloc_factor = 2;
	int m = 4;
	int M = 12;

	context_create(context, m, M, dim, alloc_factor, rectangle_a);

	struct Node *node = malloc(sizeof(struct Node));
	int level = 0;
	node_create(node, context, NULL, NULL, 0, context->space, level);

	// Entry setup
	struct Rectangle *rectangle_b = malloc(sizeof(struct Rectangle));

	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[] = { 1, 1 };
	float coords_high_b[] = { 3, 3 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);
	
	
	// Create Entry
	struct Entry *entry = malloc(sizeof(struct Entry));
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

void test_node_delete_entry() {
	int dim = 2;

	// Node setup
	struct Rectangle *rectangle_a = malloc(sizeof(struct Rectangle));

	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 0, 0 };
	float coords_high_a[] = { 2, 2 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);

	struct Context *context = malloc(sizeof(struct Context));
	float alloc_factor = 2;
	int m = 4;
	int M = 12;

	context_create(context, m, M, dim, alloc_factor, rectangle_a);

	struct Node *node = malloc(sizeof(struct Node));
	int level = 0;
	node_create(node, context, NULL, NULL, 0, context->space, level);

	// Entry setup
	struct Rectangle *rectangle_b = malloc(sizeof(struct Rectangle));

	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[] = { 1, 1 };
	float coords_high_b[] = { 3, 3 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);


	// Create Entries
	struct Entry *entry_1 = malloc(sizeof(struct Entry));
	struct Entry *entry_2 = malloc(sizeof(struct Entry));
	struct Entry *entry_3 = malloc(sizeof(struct Entry));
	struct Entry *entry_4 = malloc(sizeof(struct Entry));
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

void test_node_adjust_MBR() {
	int dim = 2;

	// Node setup
	struct Rectangle *rectangle_a = malloc(sizeof(struct Rectangle));

	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 0, 0 };
	float coords_high_a[] = { 2, 2 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);
		
	struct Context *context = malloc(sizeof(struct Context));
	float alloc_factor = 4;
	int m = 4;
	int M = 12;

	context_create(context, m, M, dim, alloc_factor, rectangle_a);
		
	struct Node *node = malloc(sizeof(struct Node));
	int level = 0;
	node_create(node, context, NULL, NULL, 0, context->space, level);

	// Entry setup
	struct Rectangle *rectangle_b = malloc(sizeof(struct Rectangle));
		
	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[] = { 1, 1 };
	float coords_high_b[] = { 3, 3 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);
	// Create Entry
	struct Entry *entry = malloc(sizeof(struct Entry));
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

void test_node_choose_optimal_entry() {
	int dim = 2;

	// Node setup
	struct Rectangle *rectangle_a = malloc(sizeof(struct Rectangle));

	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 0, 0 };
	float coords_high_a[] = { 4, 2 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);

	struct Context *context = malloc(sizeof(struct Context));
	float alloc_factor = 2;
	int m = 4;
	int M = 12;

	context_create(context, m, M, dim, alloc_factor, rectangle_a);

	struct Node *node = malloc(sizeof(struct Node));
	int level = 0;
	node_create(node, context, NULL, NULL, 0, context->space, level);

	// Add some entries

	// Entry_1 setup
	struct Rectangle *rectangle_b = malloc(sizeof(struct Rectangle));

	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[] = { 0, 0 };
	float coords_high_b[] = { 2, 2 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);
		
	struct Entry *entry_1 = malloc(sizeof(struct Entry));
	void *tuple_1 = 1;

	entry_create(entry_1, tuple_1, rectangle_b);
	node_add_entry(node, entry_1);

	// Entry_2 setup
	struct Rectangle *rectangle_c = malloc(sizeof(struct Rectangle));

	struct Point *low_c = malloc(sizeof(struct Point));
	struct Point *high_c = malloc(sizeof(struct Point));
	float coords_low_c[] = { 2, 0 };
	float coords_high_c[] = { 4, 2 };

	point_create(low_c, dim, coords_low_c);
	point_create(high_c, dim, coords_high_c);

	rectangle_create(rectangle_c, low_c, high_c);
		
	struct Entry *entry_2 = malloc(sizeof(struct Entry));
	void *tuple_2 = 2;

	entry_create(entry_2, tuple_2, rectangle_c);
	node_add_entry(node, entry_2);

	// Entry_3 setup
	struct Rectangle *rectangle_d = malloc(sizeof(struct Rectangle));

	struct Point *low_d = malloc(sizeof(struct Point));
	struct Point *high_d = malloc(sizeof(struct Point));
	float coords_low_d[] = { 1, 2 };
	float coords_high_d[] = { 4, 4 };

	point_create(low_d, dim, coords_low_d);
	point_create(high_d, dim, coords_high_d);

	rectangle_create(rectangle_d, low_d, high_d);

	struct Entry *entry_3 = malloc(sizeof(struct Entry));
	void *tuple_3 = 3;

	entry_create(entry_3, tuple_3, rectangle_d);
	
	// Test node_choose_optimal_entry
	struct Node * optimal_entry = node_choose_optimal_entry(node, entry_3);
	assert_ptr_equal(optimal_entry, entry_2);

	free(low_a);
	free(high_a);
	free(rectangle_a);
	free(context);
	free(node);

	free(low_b);
	free(high_b);
	free(rectangle_b);
	free(entry_1);

	free(low_c);
	free(high_c);
	free(rectangle_c);
	free(entry_2);

	free(low_d);
	free(high_d);
	free(rectangle_d);
	free(entry_3);
}

void test_entry_compare() {
	// Rectangle compare needs to be clarified for this
}

void test_node_calculate_MBR() {

}

void test_node_split_node() {

}


int TestNode(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_context_create),
		cmocka_unit_test(test_entry_create),
		cmocka_unit_test(test_node_create),
		cmocka_unit_test(test_node_is_leaf),
		cmocka_unit_test(test_node_is_root),
		cmocka_unit_test(test_node_adjust_MBR),
		cmocka_unit_test(test_node_add_entry),
		cmocka_unit_test(test_node_delete_entry),
		//cmocka_unit_test(test_node_choose_optimal_entry)
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}