
#include <stdint.h>
#include "host_func.h"
#include <stdarg.h>
#include <stdio.h>

// extern void call_host_func(uint32_t func_id, );

// #define DEF_HOST_FUNC(fun,idx,ret,args...) \
// 	ret fun(s) { \
// 		call_host_func(idx,args)
// 	}

#define DEF_FUNC(name,idx) \
	const uint32_t H_##name##_id = idx;

//0x0 for do_ret
DEF_FUNC(puts,0x1)
DEF_FUNC(malloc,0x2)
DEF_FUNC(free,0x3)


int puts(const char* s)
{
	call_host_func(H_puts_id,s);
}

void* malloc(uint64_t size)
{
	return (void*)call_host_func(H_malloc_id,size);
}

void free(void* ptr)
{
	call_host_func(H_free_id,ptr);
}




