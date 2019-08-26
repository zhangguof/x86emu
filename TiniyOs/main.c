#include <stdint.h>

#include "libs.h"
#include <stdio.h>
#include "host_func.h"

extern int test1_f();
extern int test2_f();

// extern int get_val();



extern test_malloc_free();

int _start()
{
	printf("hello world!\n");

	int r = test1_f();
	printf("libtest.so:%d\n",r);
	r = test2_f();
	printf("libtest2.so:%d\n",r);

	test_malloc_free();

	return 0;
}

int main()
{
	return 0;
}