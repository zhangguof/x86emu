#include <stdio.h>
#include <stdint.h>
// #include <windown.h>
#include "crt32.h"


// extern int test_abc(int,int,int);

int main()
{
	int a = 100;
	int r = Double(a);
	int r2 = test_dll("hello",1,2,3,4,5,2.0);
	printf("hello world!\n");
	printf("dll re:%d\n",r);
	test_64b(0x10,0x100,0x123456789);

	return 0;
}