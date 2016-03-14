#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include "TestRTree.h"
#include "Helpers.h"
#include "Rectangle.h"
#include "Node.h"
#include "RTree.h"


void _test_rtree_create() {
	struct RTree *rtree = malloc(sizeof(struct RTree));

	// Create rectangle
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };
	struct Context *context = create_2d_context(4, 12, 4, coords_low, coords_high);

	// Test rtree_create
	rtree_create(rtree, context);
	assert_ptr_equal(rtree->context, context);	
	assert_true(node_is_leaf(rtree->root));
	assert_true(node_is_root(rtree->root));

	destroy_context(context);	
	free(rtree);
}

void _test_rtree_insert() {
	float coords_low[] = { 1, 1 };
	float coords_high[] = { 5, 5 };
	struct RTree *rtree = create_2d_rtree(2, 4, 4, coords_low, coords_high);

	// Create entries
	struct Entry *entries = malloc(5 * sizeof(struct Entry));
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
	
	struct Entry *entry_1 = create_2d_entry(tuples[0], low_1, high_1);
	struct Entry *entry_2 = create_2d_entry(tuples[1], low_2, high_2);
	struct Entry *entry_3 = create_2d_entry(tuples[2], low_3, high_3);
	struct Entry *entry_4 = create_2d_entry(tuples[3], low_4, high_4);
	struct Entry *entry_5 = create_2d_entry(tuples[4], low_5, high_5);
	
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
	assert_ptr_equal(((struct Node *)rtree->root->entries[0])->parent, rtree->root);
	assert_ptr_equal(((struct Node *)rtree->root->entries[1])->parent, rtree->root);
	assert_true(((struct Node *)rtree->root->entries[0])->level == 0);
	assert_true(((struct Node *)rtree->root->entries[1])->level == 0);
	assert_true(rtree->root->level == 1);
}

void _test_rtree_delete() {

}

void _test_rtree_destroy() {

}

void _test_rtree_search() {

}

int test_rtree(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(_test_rtree_create),
		cmocka_unit_test(_test_rtree_insert)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}