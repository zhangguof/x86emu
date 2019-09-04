#include <stdio.h>
// #include <windown.h>
#include "crt.h"




int main()
{
	int a = 100;
	int r = Double(a);
	int r2 = test_dll("hello",1,2,3,4,5,2.0);
	printf("hello world!\n");
	printf("dll re:%d\n",r);

	return 0;
}