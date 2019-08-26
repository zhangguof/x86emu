#ifndef _HOST_FUNC_H_
#define _HOST_FUNC_H_
// #define NULL (0)

extern uint64_t call_host_func(uint32_t,...);


int puts(const char* s);

void* malloc(uint64_t size);

void free(void* ptr);


#endif