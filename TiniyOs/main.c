#include <stdint.h>

#include "libs.h"
#include <stdio.h>
#include "host_func.h"

extern int test1_f();
extern int test2_f();

// extern int get_val();

void test_malloc_free()
{
	char* s8  = (char*) malloc(8);
 	char* s16 = (char*) malloc(16);
	char* s32 = (char*) malloc(32);
	char* s2048 = (char*) malloc(2048);
	char* s4096 = (char*) malloc(4096);

	const char* test = "abcede";
	const char* test2 = "ABCDEDFDASFDASFDASFDASFA";
	int len = strlen(test);
	printf("len:%d,addr:0x%0lx\n",len,s32);

	strcpy(s32,test);

	uint32_t* a = (uint32_t*)s4096;
	*a=0x12345678;
	a++;
	*a=0xffffffff;

	printf("%s|||%d,%d\n",s32,*(int*)s4096,*(int*)(s4096+4));

	free(s8);
	free(s16);
	free(s32);
	free(s2048);
	free(s4096);

	s32 = (char*) malloc(32);
	strcpy(s32,test2);
	s4096 = malloc(4096);
	a = (uint32_t*)s4096;
	printf("addr:0x%0lx\n",a);
	*a=0x87654321;
	a++;
	*a=0xEFEFEFEF;
	printf("%s|||%d,%d\n",s32,*(int*)s4096,*(int*)(s4096+4));
	free(s4096);
	auto x = malloc(4096);
	auto y = malloc(4000);
	auto z = malloc(8192);
	free(x);
	free(y);
	free(z);

	x = malloc(3000);

}



int _start()
{
	printf("hello world!\n");

	int r = test1_f();
	printf("libtest.so:%d\n",r);
	r = test2_f();
	printf("libtest2.so:%d\n",r);

	test_malloc_free();
	
	// printf("test:0x%0x,printf:0x%0llx,0x%0llx\n",r,printf,test_f);
	// __asm__("hlt");
	return 0;
}

int main()
{
	return 0;
}