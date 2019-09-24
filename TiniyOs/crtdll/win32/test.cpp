

extern "C" {

// #define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <setjmp.h>
#include <math.h>

}

#include "test.h"






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
		exit(1);
		return ret;
	}
	else
	{
		printf("test jmp ok!\n");
		return 0;
	}
}
#define WINAPI __attribute__((__stdcall__))

WINAPI int test_std(int a,int b,int c)
{
	int aa = a+1;
	int bb = b*2;
	int cc = c+3;
	return aa+bb+cc;
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
	test_std(1,23,45);

	return 0x123456789ABCDEF;
}


void test_file();

#include <stdarg.h>
void test_vpritnf(const char* fmt,...)
{
	va_list ap;
	va_start(ap,fmt);
	printf("start vprintf!!!!\n");
	vprintf(fmt,ap);
	va_end(ap);
}

int test_dll2()
{
	const char* name = "hello test dll 2111!\n";
	printf("test stderr:%p,%p,%p\n",stdin,stdout,stderr);
	fprintf(stderr,name);
	// puts(name);
	// char* s = (char*)malloc(32);
	// char *s = nullptr;
	// s = strstr(name,"test");
	// puts(s);
	// int a = 0x123;
	// long b = 0x1234587;
	// testjmp();
	// longjmp(buf,1);
	// test_file();
	test_vpritnf("test vprintf!:%c,%d,%d,%llx,%llx,%lf,%s,%p\n",'X',1234567,7654321,
	0xEFEFEFEF12345678,0x1234567887654321,double(1.2),"hello test!\n",name);

	// return a+b;
	return 1;
}



void test_file()
{
	const char* fname = "/Users/tony/workspace/github/x86emu/TiniyOs/crtdll/win32/test.txt";
	FILE* fs = fopen(fname,"w");
	const char* buf  = "test\ntest2\nddddddd\n";
	auto len = strlen(buf);
	//size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) __THROW;
	// auto ret = fwrite(buf,1,len,fs);
	// fclose(fs);
	// printf("has write!:%d\n",ret);
}


// #include <unordered_map>
// #include <string>
// std::unordered_map<std::string,call_funcs*> call_func_tbl;

void init_call_funcs(call_funcs* tbl)
{
	// call_funcs* p = tbl;
	// for(;p!=nullptr;p++)
	// {
	// 	printf("check call func:%s,%p\n", p->name,p->pf);
	// 	call_func_tbl[p->name] = p->pf
	// }
}



// void __cdecl _lock (int locknum);
// void* calloc（unsigned int num，unsigned int size）；
//int fputs(const char*s,FILE*stream) __attribute__((weak,alias("fputs_unlocked")));
void test_call_ptr(int (*ptr)(int,int))
{
	int ret = ptr(123,321);
	printf("after test_call_ptr:%d\n",ret);
	// cls_test_func();
	// ret  = abs(-123);
	// printf("ret abs:%d\n",ret);
	// printf("stdio:%p,%p,%p\n",stdin,stdout,stderr);
	// fputs("ssssin fputs!!!!",stderr);
	// double f = 3.14;
	// double ff = cos(f);
	// uint64_t dd = 0x12345878988888;
	// uint64_t dd2  = *((uint64_t*)&ff);
	// // f = cos(f);
	// printf("0x%0llx,%f,0x%0llx\n",dd,f,dd2);
	void* tt = calloc(64,1);

	// _lock(11);

}





