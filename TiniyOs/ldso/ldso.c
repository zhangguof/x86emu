#include <stdio.h>

#include "elf.h"

int aaa=0x2000;

// extern int test_f();
int test()
{
	return 0x1000;
}
void test2()
{
	aaa++;
}


int _start()
{
	// printf("hello world!\n");
	
	int r2 = test() + aaa;
	int r = test_f();
	
	// printf("test:0x%0x,printf:0x%0llx,0x%0llx\n",r,printf,test_f);
	__asm__("hlt");
	return 0;
}
