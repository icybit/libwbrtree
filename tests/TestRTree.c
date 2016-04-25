#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../include/Context.h"
#include "../include/Entry.h"
#include "../include/Node.h"
#include "../include/Rectangle.h"
#include "../include/RTree.h"
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

	rectangle_create(&space, low, high, dimension);
	context_create(&context, 2, 4, dimension, 35, 4, &space);
	rtree_create(&rtree, &context);
	
	rectangle_create(&MBR_1, low_1, high_1, dimension);
	entry_create(&entry_1, &tuples[0], &MBR_1);
	
	rtree_insert(&rtree, &entry_1);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.root->MBR->low[index] == low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_1[index]);
	}
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));

	rectangle_create(&MBR_2, low_2, high_2, dimension);
	entry_create(&entry_2, &tuples[1], &MBR_2);

	rtree_insert(&rtree, &entry_2);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.root->MBR->low[index] == low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_2[index]);
	}
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));

	rectangle_create(&MBR_3, low_3, high_3, dimension);
	entry_create(&entry_3, &tuples[2], &MBR_3);

	rtree_insert(&rtree, &entry_3);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.root->MBR->low[index] == low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_3[index]);
	}
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));

	rectangle_create(&MBR_4, low_4, high_4, dimension);
	entry_create(&entry_4, &tuples[3], &MBR_4);

	rtree_insert(&rtree, &entry_4);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.root->MBR->low[index] == low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_4[index]);
	}
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));

	rectangle_create(&MBR_5, low_5, high_5, dimension);
	entry_create(&entry_5, &tuples[4], &MBR_5);

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

void _test_rtree_try_insert() {
	rt_rtree_t rtree;
	rt_ctx_t context;
	rt_node_t * node_1, *node_2;
	rt_rect_t space, MBR_1, MBR_2, MBR_3, MBR_4, MBR_5, MBR_6, MBR_7;
	rt_entry_t entry_1, entry_2, entry_3, entry_4, entry_5, entry_6, entry_7;
	uint8_t dimension = 2;
	int index, tuples[] = { 1, 2, 3, 4, 5, 6 };
	float low[] = { 0, 0 }, high[] = { 27, 19 }, 
		  low_1[] = { 0, 0 }, high_1[] = { 14, 18 },
		  low_2[] = { 15, 1 }, high_2[] = { 27, 19 },
		  low_3[] = { 0, 0 }, high_3[] = { 10, 8 },
		  low_4[] = { 8, 11 }, high_4[] = { 14, 18 }, 
		  low_5[] = { 15, 14 }, high_5[] = { 22, 19 },
		  low_6[] = { 15, 1 }, high_6[] = { 27, 9 },
		  low_7[] = { 22, 3 }, high_7[] = { 26, 8 };

	rectangle_create(&space, low, high, dimension);
	context_create(&context, 2, 4, dimension, 35, 4, &space);
	rtree_create(&rtree, &context);

	rectangle_create(&MBR_1, low_1, high_1, dimension);
	entry_create(&entry_1, &tuples[0], &MBR_1);
	rtree_insert(&rtree, &entry_1);
	
	rectangle_create(&MBR_2, low_2, high_2, dimension);
	entry_create(&entry_2, &tuples[1], &MBR_2);
	rtree_insert(&rtree, &entry_2);
	
	rectangle_create(&MBR_3, low_3, high_3, dimension);
	entry_create(&entry_3, &tuples[2], &MBR_3);
	rtree_insert(&rtree, &entry_3);

	rectangle_create(&MBR_4, low_4, high_4, dimension);
	entry_create(&entry_4, &tuples[3], &MBR_4);
	rtree_insert(&rtree, &entry_4);

	rectangle_create(&MBR_5, low_5, high_5, dimension);
	entry_create(&entry_5, &tuples[4], &MBR_5);
	rtree_insert(&rtree, &entry_5);

	rectangle_create(&MBR_6, low_6, high_6, dimension);
	entry_create(&entry_6, &tuples[5], &MBR_6);
	node_1 = rtree_try_insert(&rtree, &entry_6);

	for (index = 0; index < dimension; index++)
	{
		assert_true(node_1->MBR->low[index] == entry_2.MBR->low[index]);
		assert_true(node_1->MBR->high[index] == entry_2.MBR->high[index]);
	}

	rtree_insert(&rtree, &entry_6);

	rectangle_create(&MBR_7, low_7, high_7, dimension);
	entry_create(&entry_7, &tuples[6], &MBR_7);
	node_2 = rtree_try_insert(&rtree, &entry_7);

	for (index = 0; index < dimension; index++)
	{
		assert_true(node_2->MBR->low[index] == entry_6.MBR->low[index]);
		assert_true(node_2->MBR->high[index] == entry_6.MBR->high[index]);
	}
}

void _test_rtree_try_insert_2() {
	rt_rtree_t rtree;
	/*hashset_st *results_1, *results_2, *results_3;*/
	rt_ctx_t context;
	/*rt_node_t * node_1;*/
	rt_rect_t space, MBR_1, MBR_2, MBR_3, MBR_4, MBR_5, MBR_6, MBR_7, MBR_8, MBR_9 /*MBR_10*//* MBR_11*/;
	rt_entry_t entry_1, entry_2, entry_3, entry_4, entry_5, entry_6, entry_7, entry_8, entry_9 /*entry_10*//* entry_11*/;
	uint8_t dimension = 2;
	int /*index,*/ tuples[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9/*, 10*/ /*11*/ };
	float low[] = { 0, 0 }, high[] = { 0, 0 }, 
		  low_1[] = { 0, 8 }, high_1[] = { 3, 11 },
		  low_2[] = { 6, 8 }, high_2[] = { 9, 11 },
		  low_3[] = { 0, 4 }, high_3[] = { 3, 7 },
		  low_4[] = { 6, 4 }, high_4[] = { 9, 7 }, 
		  low_5[] = { 0, 0 }, high_5[] = { 3, 3 },
		  low_6[] = { 6, 0 }, high_6[] = { 9, 3 },		  
		  low_7[] = { 11, 8 }, high_7[] = { 14, 11 },		  
		  low_8[] = { 11, 4 }, high_8[] = { 14, 7 },		  
		  low_9[] = { 11, 0 }, high_9[] = { 14, 3 };	  
		  /*low_10[] = { 17, 0 }, high_10[] = { 20, 3 };	 */ 
		  /*low_11[] = { 17, 3 }, high_11[] = { 20, 7 };*/	  

	rectangle_create(&space, low, high, dimension);
	context_create(&context, 2, 4, dimension, 35, 4, &space);
	rtree_create(&rtree, &context);

	rectangle_create(&MBR_1, low_1, high_1, dimension);
	entry_create(&entry_1, &tuples[0], &MBR_1);
	rtree_insert(&rtree, &entry_1);
	
	rectangle_create(&MBR_2, low_2, high_2, dimension);
	entry_create(&entry_2, &tuples[1], &MBR_2);
	rtree_insert(&rtree, &entry_2);
	
	rectangle_create(&MBR_3, low_3, high_3, dimension);
	entry_create(&entry_3, &tuples[2], &MBR_3);
	rtree_insert(&rtree, &entry_3);

	rectangle_create(&MBR_4, low_4, high_4, dimension);
	entry_create(&entry_4, &tuples[3], &MBR_4);
	rtree_insert(&rtree, &entry_4);

	rectangle_create(&MBR_5, low_5, high_5, dimension);
	entry_create(&entry_5, &tuples[4], &MBR_5);
	rtree_insert(&rtree, &entry_5);

	rectangle_create(&MBR_6, low_6, high_6, dimension);
	entry_create(&entry_6, &tuples[5], &MBR_6);
	rtree_insert(&rtree, &entry_6);
	
	rectangle_create(&MBR_7, low_7, high_7, dimension);
	entry_create(&entry_7, &tuples[6], &MBR_7);
	rtree_insert(&rtree, &entry_7);

	rectangle_create(&MBR_8, low_8, high_8, dimension);
	entry_create(&entry_8, &tuples[7], &MBR_8);
	rtree_insert(&rtree, &entry_8);

	rectangle_create(&MBR_9, low_9, high_9, dimension);
	entry_create(&entry_9, &tuples[8], &MBR_9);
	rtree_insert(&rtree, &entry_9);

	/*rectangle_create(&MBR_10, low_10, high_10, dimension);
	entry_create(&entry_10, &tuples[9], &MBR_10);
	rtree_insert(&rtree, &entry_10);*/

	rtree_visualize(&rtree);	
/*
	

	rectangle_create(&MBR_11, low_11, high_11, dimension);
	entry_create(&entry_11, &tuples[10], &MBR_11);
	node_1 = rtree_try_insert(&rtree, &entry_11);

	for (index = 0; index < dimension; index++)
	{
		assert_true(node_1->MBR->low[index] == entry_9.MBR->low[index]);
		assert_true(node_1->MBR->high[index] == entry_10.MBR->high[index]);
	}	*/
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
		cmocka_unit_test(_test_rtree_insert),
		/*cmocka_unit_test(_test_rtree_try_insert),*/
		cmocka_unit_test(_test_rtree_try_insert_2)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}