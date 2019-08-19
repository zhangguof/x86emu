#include <stdint.h>
// #include <string.h>

char *str = "hello world!";

extern int add(int,int);
extern int get_val();

// typedef unsigned char uint8_t;

// extern void call_host_func(uint32_t func_id, uint8_t* data, uint32_t len);
extern void call_host_func(uint32_t,...);


uint32_t strlen(const char* s)
{
	uint32_t c=0;
	while(*s++!='\0')
	{
		c++;
	}
	return c;
}

inline void push_int32(uint8_t* buf,int32_t val)
{
	*((int32_t*)buf) = val;
}

inline void push_int64(uint8_t* buf, int64_t val)
{
	*((int64_t*)buf) = val;
}

void puts(const char* s)
{
	// uint32_t len = strlen(s);
	// uint32_t size = 0;
	// size += sizeof(s);
	// uint8_t buf[1<<7]; //128b,16*8byte args max!
	// uint8_t* pbuf = buf;


	call_host_func(0x1,s);
}



int _start()
{
	int a=10;
	int b=20;
	int c = add(a,b);
	c += get_val();
	puts("hello world!\n");
	__asm__("hlt");
	return 0;
}
