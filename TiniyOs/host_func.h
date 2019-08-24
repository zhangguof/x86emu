#ifndef _HOST_FUNC_H_
#define _HOST_FUNC_H_
#define NULL (0)

extern void call_host_func(uint32_t,...);

struct host_func_t
{
	uint32_t idx;
	const char* name;
} HOST_FUNC_T;


#endif