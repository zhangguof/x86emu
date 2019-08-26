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

// typedef uint64_t(*call_func_t1)(uint64_t);
// uint64_t call_guest_method1(call_func_t1 fun_ptr,uint64_t arg1)
// {
// 	return fun_ptr(arg1);
// }

// struct export_funcs
// {
// 	void* ptr;
// 	char name[64];
// } call_guest_method;

int _start()
{

	printf("hello world!\n");
	
	int r = ldso_test() + ldso_val;
	printf("ldso test:%0x\n",r);

	r = test1_f();
	printf("libtest1.so:%d\n",r);
	r = test2_f();
	printf("libtest2.so:%d\n",r);
	
	// __asm__("hlt");
	return 0;
}
int main()
{
	return 0;
}
