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


uint64_t test_dll3(int a,const char* name,uint64_t a64)
{
	// puts(name);
	// test_f1(a,0x200,"hello in test1\n",
	// 		'Z',0xECECECEC,0x3E3F,a64);
	const char* s = "hhhhhttththth\n";
	// char buf[1024];
	// snprintf(buf,1024,"%s,0x%0x\n","int:",0x1234567);
	// puts(buf);
	printf("int dll:test_dll3:%d,%s,0x%0llx\n",a,name,a64);
	return 0x123456789ABCDEF;
}


int test_dll2()
{
	const char* name = "hello test dll 2111!\n";
	// puts(name);
	char* s = (char*)malloc(32);
	s = strstr(name,"test");
	puts(s);
	int a = 0x123;
	long b = 0x1234587;
	return a+b;
}


int test_dll(const char* name,int a1,int a2,int a3,int a4,int a5,double f1)
{

	int b = a1+a2+a3+a4+a5;
	double ff = f1+1.0;
	return b;
}
