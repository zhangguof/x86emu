#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>



int a = 0x10000;
int* pa = &a;
// int xx = 0x2000;
// extern void test_ext(int a);

// void call_host_func(uint32_t,...);

int test_pow2(int x)
{
	return x*x;
}
// int test_f3(int a)
// {
// 	return 0;
// }
typedef int(*fun_ptr_t)(int);

// fun_ptr_t p_fun = test_f1;

int test1_f()
{
	printf("in libtest.so:test1_f\n");
	// int aa = *pa;
	// printf("=======:%d,%0lx\n",a,&a);
	int c = a;
	c += a + test_pow2(1);
	printf("in test1_f:%0x\n",c);
	return c;
}

