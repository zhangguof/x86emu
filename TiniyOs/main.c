#include <stdint.h>
// #include <string.h>
#include "host_func.h"
#include "libs.h"
#include <stdio.h>

extern int test1_f();
extern int test2_f();

// extern int get_val();

int _start()
{
	printf("hello world!\n");

	int r = test1_f();
	printf("libtest.so:%d\n",r);
	r = test2_f();
	printf("libtest2.so:%d\n",r);
	
	// printf("test:0x%0x,printf:0x%0llx,0x%0llx\n",r,printf,test_f);
	// __asm__("hlt");
	// return (void*)&call_guest_method;
	return 0;
}

int main()
{
	return 0;
}