#include <stdio.h>

#include "elf.h"

int ldso_val=0x2000;

extern int test1_f();
extern int test2_f();
int ldso_test()
{
	ldso_val++;
	return 0x1000;
}



int _start()
{
	printf("hello world!\n");
	
	int r = ldso_test() + ldso_val;
	printf("ldso test:%0x\n",r);

	r = test1_f();
	printf("libtest1.so:%d\n",r);
	r = test2_f();
	printf("libtest2.so:%d\n",r);
	
	// printf("test:0x%0x,printf:0x%0llx,0x%0llx\n",r,printf,test_f);
	__asm__("hlt");
	return 0;
}
