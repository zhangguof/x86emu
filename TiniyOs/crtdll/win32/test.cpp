

extern "C" {

// #define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <setjmp.h>

}

#include "test.h"



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

static jmp_buf buf;

int testjmp()
{
	int ret = setjmp(buf);
	if(ret)
	{
		printf("=====in setjmp:%d\n",ret);
		return ret;
	}
	else
	{
		printf("test jmp ok!\n");
		return 0;
	}
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
	// char* s = (char*)malloc(32);
	char *s = nullptr;
	s = strstr(name,"test");
	puts(s);
	int a = 0x123;
	long b = 0x1234587;
	testjmp();
	longjmp(buf,1);

	return a+b;
}

