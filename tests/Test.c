#include <stdio.h>
#include "TestRectangle.h"
#include "TestNode.h"
#include "TestRTree.h"
#include "Benchmarks.h"

int main(int argc, char *argv[])
{
	rt_bench_params_t params;
	params_initialize(&params, 2, 4, 4.0f, 117000);

	/*puts("Running Rectangle Tests...\n");	
	test_rectangle();
	puts("Running Node Tests...\n");	
	test_node();
	puts("Running RTree Tests...\n");	
	test_rtree();*/
	benchmarks_run(&params);

	return 0;
}