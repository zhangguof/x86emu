#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "crt.h"

extern "C"
{
extern uint64_t call_win_host_func(uint32_t,...);
}




int Double(int x)
{
        return 2 * x;
}

int test_dll2()
{
	// call_win_host_func(0x1,"hello test dll2!!!\n");
	const char* name = "hello test dll 2111!\n";
	puts(name);
	// char* s = (char*)malloc(32);
	char* s = strstr(name,"test");
	puts(s);

	// int len = strlen(name);
	// memcpy(s,name,len);
	// puts(s);

	// free(s);

	return 0;
}

int test_dll(const char* name,int a1,int a2,int a3,int a4,int a5,double f1)
{
	// printf(name);
	call_win_host_func(0x1,name);
	int b = a1+a2+a3+a4+a5;
	double ff = f1+1.0;
	return b;
}
