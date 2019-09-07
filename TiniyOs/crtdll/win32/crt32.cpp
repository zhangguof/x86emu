#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "crt32.h"




int Double(int x)
{
        return 2 * x;
}

int64_t test_64b(int a,int b,int64_t c)
{
	int64_t r = c + 0x10000;
	r <<= 10;
	return r;
}


int test_dll3()
{
	test_f1(0x100,0x200,"hello in test1\n",
			'Z',0xECECECEC,0x3E3F,0x1FFFFFFF2EEEEEEE);
	return 0x1234;
}


int test_dll2()
{
	const char* name = "hello test dll 2111!\n";
	puts(name);
	// char* s = (char*)malloc(32);
	char* s = strstr(name,"test");
	puts(s);
	int a = 0x123;
	long b = 0x1234587;
	return a+b;
	// printf("test:%d,0x%0lx,%s!!!\n",a,b,name);

	// int len = strlen(name);
	// memcpy(s,name,len);
	// puts(s);

	// free(s);

	// return 0;
}


int test_dll(const char* name,int a1,int a2,int a3,int a4,int a5,double f1)
{
	// printf(name);
	// call_win_host_func(0x1,name);
	int b = a1+a2+a3+a4+a5;
	double ff = f1+1.0;
	return b;
}
