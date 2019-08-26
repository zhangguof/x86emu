
#include <stdint.h>
#include "host_func.h"
#include <stdarg.h>
#include <stdio.h>

// extern void call_host_func(uint32_t func_id, );

// inline void push_int32(uint8_t* buf,int32_t val)
// {
// 	*((int32_t*)buf) = val;
// }

// inline void push_int64(uint8_t* buf, int64_t val)
// {
// 	*((int64_t*)buf) = val;
// }

// #define DEF_HOST_FUNC(fun,idx,ret,args...) \
// 	ret fun(s) { \
// 		call_host_func(idx,args)
// 	}
#define DEF_FUNC(name,idx) \
	const uint32_t H_##name##_id = idx;

DEF_FUNC(puts,0x1)
DEF_FUNC(printf,0x2)

int puts(const char* s)
{
	call_host_func(H_puts_id,s);
}

// void printf(const char* fmt,...)
// {
// 	// va_list args;
// 	// va_start(args,fmt);
// 	// vcall_host_func(Fun_printf_id,fmt,args);
// 	// va_end(args);
// 	char dest_str[128];
// 	int ret = vsnprintf()

// }


