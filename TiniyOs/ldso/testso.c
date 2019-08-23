#include <stdio.h>
int a = 0x10000;
int* pa = &a;
// int xx = 0x2000;
// extern void test_ext(int a);
int test_f1()
{
	return 1;
}

int test_f2(int x)
{
	return x+1;
}
// int test_f3(int a)
// {
// 	return 0;
// }
typedef int(*fun_ptr_t)(int);

fun_ptr_t p_fun = test_f1;

int test_f()
{
	// puts("111111\n");
	// int a = 0x1000;
	// test_ext(111);

	int aa = *pa;

	int b = 0x2000 + a;
	b += test_f2(1);
	// int *pa = &a;
	int c = b + test_f1();
	// int c = a+b+xx+test_f1();
	// c = test_f2(c)+test_f3(1);
	printf("====a+b=%d\n",c);
	return c;

}