#ifndef _RTREE_TEST_NODE_H_
#define _RTREE_TEST_NODE_H_

void _test_context_create();

void _test_entry_create();

void _test_node_create();

void _test_node_is_leaf();

void _test_node_is_root();

void _test_node_add_entry();

void _test_node_delete_entry();

void _test_node_adjust_MBR();

void _test_node_choose_optimal_entry();

void _test_entry_compare();

void _test_node_calculate_MBR_Leaf();

void _test_node_calculate_MBR_Non_Leaf();

void _test_node_split_node();

int test_node(void);
#endif
