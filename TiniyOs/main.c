#include <stdint.h>
// #include <string.h>
#include "host_func.h"
#include "libs.h"
#include <stdio.h>

char *str = "hello world!";

extern int add(int,int);
extern int get_val();

// typedef unsigned char uint8_t;



// uint32_t strlen(const char* s)
// {
// 	uint32_t c=0;
// 	while(*s++!='\0')
// 	{
// 		c++;
// 	}
// 	return c;
// }





int _start()
{
	int a=10;
	int b=20;
	int c = add(a,b);
	c += get_val();
	puts("hello world!\n");
	printf("a+b=%d\n",c);
	__asm__("hlt");
	return 0;
}