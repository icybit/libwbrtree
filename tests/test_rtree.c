#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "Helpers.h"
#include "Rectangle.h"
#include "Node.h"
#include "RTree.h"

void test_rtree_create() {
	struct RTree *rtree = malloc(sizeof(struct RTree));

	// Create rectangle
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };
	struct Rectangle *rectangle = create_2d_rectangle(coords_low, coords_high);

	// Create context
	struct Context *context = malloc(sizeof(struct Context));
	context_create(context, 4, 12, 4, rectangle);

	// Test rtree_create
	rtree_create(rtree, context);
	assert_ptr_equal(rtree->context, context);	
	assert_true(node_is_leaf(rtree->root));
	assert_true(node_is_root(rtree->root));

	destroy_rectangle(rectangle);
	free(context);
	free(rtree);
}

void test_rtree_insert() {

}

void test_rtree_delete() {

}

void test_rtree_destroy() {

}

void test_rtree_search() {

}

int TestRTree(void) {
	const struct CMUnitTest tests[] = {
		
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}