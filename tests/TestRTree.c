#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/Context.h"
#include "../src/Entry.h"
#include "../src/Node.h"
#include "../src/Rectangle.h"
#include "../include/wbdrtree/wbdrtreeapi.h"
#include "TestRTree.h"

void _test_rtree_create() {
	rt_rtree_t rtree;
	rt_ctx_t context;
	rt_rect_t rectangle;
	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 2.0f };

	rtree_rectangle_create(&rectangle, low, high, 2);
	rtree_context_create(&context, 4, 12, 2, 35, 4, &rectangle);
	rtree_create(&rtree, &context);

	assert_ptr_equal(rtree.context, &context);	
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));
}

void _test_rtree_insert() {
	rt_rtree_t rtree;
	rt_ctx_t context;
	rt_rect_t space, MBR_1, MBR_2, MBR_3, MBR_4, MBR_5;
	rt_entry_t entry_1, entry_2, entry_3, entry_4, entry_5;
	uint8_t dimension = 2;
	int index, tuples[] = { 1, 2, 3, 4, 5 };
	float low[] = { 1, 1 }, high[] = { 25, 25 }, low_1[] = { 6, 3 }, high_1[] = { 8, 5 },
		low_2[] = { 9, 6 }, high_2[] = { 11, 8 }, low_3[] = { 12, 9 }, high_3[] = { 14, 11 },
		low_4[] = { 15, 12 }, high_4[] = { 17, 14 }, low_5[] = { 18, 15 }, high_5[] = { 20, 17 };

	rtree_rectangle_create(&space, low, high, dimension);
	rtree_context_create(&context, 2, 4, dimension, 35, 4, &space);
	rtree_create(&rtree, &context);
	
	rtree_rectangle_create(&MBR_1, low_1, high_1, dimension);
	rtree_entry_create(&entry_1, &tuples[0], &MBR_1);
	
	rtree_insert(&rtree, &entry_1);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.root->MBR->low[index] == low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_1[index]);
	}
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));

	rtree_rectangle_create(&MBR_2, low_2, high_2, dimension);
	rtree_entry_create(&entry_2, &tuples[1], &MBR_2);

	rtree_insert(&rtree, &entry_2);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.root->MBR->low[index] == low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_2[index]);
	}
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));

	rtree_rectangle_create(&MBR_3, low_3, high_3, dimension);
	rtree_entry_create(&entry_3, &tuples[2], &MBR_3);

	rtree_insert(&rtree, &entry_3);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.root->MBR->low[index] == low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_3[index]);
	}
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));

	rtree_rectangle_create(&MBR_4, low_4, high_4, dimension);
	rtree_entry_create(&entry_4, &tuples[3], &MBR_4);

	rtree_insert(&rtree, &entry_4);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.root->MBR->low[index] == low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_4[index]);
	}
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));

	rtree_rectangle_create(&MBR_5, low_5, high_5, dimension);
	rtree_entry_create(&entry_5, &tuples[4], &MBR_5);

	rtree_insert(&rtree, &entry_5);

	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.context->space->low[index] == low[index]);
		assert_true(rtree.context->space->high[index] == high[index]);
		assert_true(rtree.root->MBR->low[index] = low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_5[index]);
	}
	assert_ptr_equal(((rt_node_t *)rtree.root->entries[0])->parent, rtree.root);
	assert_ptr_equal(((rt_node_t *)rtree.root->entries[1])->parent, rtree.root);
	assert_true(((rt_node_t *)rtree.root->entries[0])->level == 0);
	assert_true(((rt_node_t *)rtree.root->entries[1])->level == 0);
	assert_true(rtree.root->level == 1);
}

/* TODO: The following tests should be refactored to remove superfluous heap allocations and to comply with -Pedantic rules */

/*void _test_rtree_delete() {

}

void _test_rtree_destroy() {

}

void _test_rtree_search() {

}*/

int test_rtree(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(_test_rtree_create),
		cmocka_unit_test(_test_rtree_insert)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}