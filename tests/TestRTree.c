#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../include/Node.h"
#include "../include/Rectangle.h"
#include "../include/RTree.h"
#include "Helpers.h"
#include "TestRTree.h"

void _test_rtree_create() {
	rt_rtree_t rtree;
	rt_ctx_t context;
	rt_rect_t rectangle;
	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 2.0f };

	rectangle_create(&rectangle, low, high, 2);
	context_create(&context, 4, 12, 2, 35, 4, &rectangle);
	rtree_create(&rtree, &context);

	assert_ptr_equal(rtree.context, &context);	
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));
}

/* TODO: The following tests should be refactored to remove superfluous heap allocations and to comply with -Pedantic rules */

/*void _test_rtree_insert() {
	float coords_low[] = { 1, 1 };
	float coords_high[] = { 5, 5 };
	rt_rtree_t *rtree = create_2d_rtree(2, 4, 4, coords_low, coords_high);

	// Create entries
	rt_entry_t *entries = malloc(5 * sizeof(rt_entry_t));
	int tuples[] = { 1, 2, 3, 4, 5 };
	
	float low_1[] = { 6, 3 };
	float high_1[] = { 8, 5 };

	float low_2[] = { 9, 6 };
	float high_2[] = { 11, 8 };

	float low_3[] = { 12, 9 };
	float high_3[] = { 14, 11 };

	float low_4[] = { 15, 12 };
	float high_4[] = { 17, 14 };

	float low_5[] = { 18, 15 };
	float high_5[] = { 20, 17 };
	
	rt_entry_t *entry_1 = create_2d_entry(tuples[0], low_1, high_1);
	rt_entry_t *entry_2 = create_2d_entry(tuples[1], low_2, high_2);
	rt_entry_t *entry_3 = create_2d_entry(tuples[2], low_3, high_3);
	rt_entry_t *entry_4 = create_2d_entry(tuples[3], low_4, high_4);
	rt_entry_t *entry_5 = create_2d_entry(tuples[4], low_5, high_5);
	
	// Test rtree_insert
	rtree_insert(rtree, entry_1);
	assert_ptr_equal(rtree->root->MBR->low->coords, coords_low);
	assert_true(rtree->root->MBR->high->coords[0], high_1[0]);
	assert_true(rtree->root->MBR->high->coords[1], high_1[1]);
	assert_true(node_is_leaf(rtree->root));
	assert_true(node_is_root(rtree->root));

	rtree_insert(rtree, entry_2);
	assert_ptr_equal(rtree->root->MBR->low->coords, coords_low);
	assert_true(rtree->root->MBR->high->coords[0], high_2[0]);
	assert_true(rtree->root->MBR->high->coords[1], high_2[1]);
	assert_true(node_is_leaf(rtree->root));
	assert_true(node_is_root(rtree->root));

	rtree_insert(rtree, entry_3);
	assert_ptr_equal(rtree->root->MBR->low->coords, coords_low);
	assert_true(rtree->root->MBR->high->coords[0], high_3[0]);
	assert_true(rtree->root->MBR->high->coords[1], high_3[1]);
	assert_true(node_is_leaf(rtree->root));
	assert_true(node_is_root(rtree->root));

	rtree_insert(rtree, entry_4);
	assert_ptr_equal(rtree->root->MBR->low->coords, coords_low);
	assert_true(rtree->root->MBR->high->coords[0], high_4[0]);
	assert_true(rtree->root->MBR->high->coords[1], high_4[1]);
	assert_true(node_is_leaf(rtree->root));
	assert_true(node_is_root(rtree->root));

	// Node should split to 2 nodes. New root node should be created.
	puts(entry_1->tuple);
	puts(entry_2->tuple);
	puts(entry_3->tuple);
	puts(entry_4->tuple);
	puts(entry_5->tuple);
	rtree_insert(rtree, entry_5);
	assert_ptr_equal(rtree->root->MBR->low->coords, coords_low);
	assert_true(rtree->root->MBR->high->coords[0], high_5[0]);
	assert_true(rtree->root->MBR->high->coords[1], high_5[1]);
	assert_ptr_equal(((rt_node_t *)rtree->root->entries[0])->parent, rtree->root);
	assert_ptr_equal(((rt_node_t *)rtree->root->entries[1])->parent, rtree->root);
	assert_true(((rt_node_t *)rtree->root->entries[0])->level == 0);
	assert_true(((rt_node_t *)rtree->root->entries[1])->level == 0);
	assert_true(rtree->root->level == 1);
}

void _test_rtree_delete() {

}

void _test_rtree_destroy() {

}

void _test_rtree_search() {

}*/

int test_rtree(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(_test_rtree_create)
		/*cmocka_unit_test(_test_rtree_insert)*/
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}