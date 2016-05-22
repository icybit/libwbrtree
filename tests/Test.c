#include <stdio.h>
#include "TestRectangle.h"
#include "TestNode.h"
#include "TestRTree.h"
#include "Benchmarks.h"

int main(int argc, char *argv[])
{
	/*puts("Running Rectangle Tests...\n");	
	test_rectangle();
	puts("Running Node Tests...\n");	
	test_node();
	puts("Running RTree Tests...\n");	
	test_rtree();*/
	benchmarks_run();

	return 0;
}