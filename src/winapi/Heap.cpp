#define NEED_CPU_REG_SHORTCUTS 1
#include "cpu/x86.h"
#include "param_names.h"
#include "elf.h"
#include "mmu.hpp"

#include <functional>
#include "engine.hpp"

//#include "crt/wrap_crt.hpp"
#include "wrap_host_call.hpp"
#include "utils.h"
#include "winapi.hpp"
#include "logger.hpp"
#include "loaddll/load_dll.hpp"

#define HEAP_ZERO_MEMORY 8
#undef WINAPI
#define WINAPI

class winHeap:public HostCallerBase
{
#undef DECLARE_CRT_EXPORT
#define DECLARE_CRT_EXPORT(name,func)\
static uint64_t wrap_##func(uint64_t* args);
    DECLARE_CRT_EXPORT("HeapCreate", HeapCreate);
    DECLARE_CRT_EXPORT("GetProcessHeap", GetProcessHeap);
    DECLARE_CRT_EXPORT("HeapAlloc", HeapAlloc);
    DECLARE_CRT_EXPORT("HeapFree", HeapFree);
    DECLARE_CRT_EXPORT("HeapSize", HeapSize);
    DECLARE_CRT_EXPORT("HeapReAlloc", HeapReAlloc);
    DECLARE_CRT_EXPORT("LocalAlloc", LocalAlloc);
    DECLARE_CRT_EXPORT("LocalFree", LocalFree);
    
#undef DECLARE_CRT_EXPORT
    virtual void init_funcs();
    
};

#define DEF_HOST_FUNC(cls,func) \
uint64_t cls::wrap_##func(uint64_t* args)


STATIC HANDLE WINAPI GetProcessHeap(void)
{
    return (HANDLE) 'HEAP';
}

DEF_HOST_FUNC(winHeap,GetProcessHeap)
{
    return (uint64_t)GetProcessHeap();
}

STATIC HANDLE WINAPI HeapCreate(DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize)
{
    LOG_DEBUG("%#x, %u, %u", flOptions, dwInitialSize, dwMaximumSize);
    return (HANDLE) 'HEAP';
}

DEF_HOST_FUNC(winHeap, HeapCreate)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        uint32_t arg1 = w32.next<uint32_t>();
        uint32_t arg2 = w32.next<uint32_t>();
        uint32_t arg3 = w32.next<uint32_t>();
        return (uint64_t) HeapCreate(arg1, arg2, arg3);
    }
    else
    {
        
    }
    return 0;
}

STATIC PVOID WINAPI HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes)
{
    PVOID Buffer;

    // LOG_DEBUG("%p, %#x, %u", hHeap, dwFlags, dwBytes);

    if (dwFlags & HEAP_ZERO_MEMORY) {
//        Buffer = calloc(dwBytes, 1);
        Buffer = host_calloc(dwBytes, 1);
    } else {
//        Buffer = malloc(dwBytes);
        Buffer = host_malloc(dwBytes);
    }

    return Buffer;
}


DEF_HOST_FUNC(winHeap, HeapAlloc)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*) w32.next<uint32_t>();
        uint32_t arg2 = w32.next<uint32_t>();
        uint32_t arg3 = w32.next<uint32_t>();
        return (uint64_t) HeapAlloc(arg1, arg2, arg3);
    }
    else
    {
        
    }
    return 0;
}

STATIC BOOL WINAPI HeapFree(HANDLE hHeap, DWORD dwFlags, PVOID lpMem)
{
    // LOG_DEBUG("%p, %#x, %p", hHeap, dwFlags, lpMem);

//    free(lpMem);
    host_free(lpMem);

    return TRUE;
}

DEF_HOST_FUNC(winHeap, HeapFree)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*) w32.next<WIN32_PTR>();
        uint32_t arg2 = w32.next<uint32_t>();
        void* arg3 = (void*)w32.next<WIN32_PTR>();
        return (uint64_t) HeapFree(arg1, arg2, arg3);
    }
    else
    {
        
    }
    return 0;
}

STATIC SIZE_T WINAPI HeapSize(HANDLE hHeap, DWORD dwFlags, PVOID lpMem)
{
    return host_malloc_usable_size(lpMem);
}

DEF_HOST_FUNC(winHeap, HeapSize)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*) w32.next<WIN32_PTR>();
        uint32_t arg2 = w32.next<uint32_t>();
        void* arg3 = (void*)w32.next<WIN32_PTR>();
        return (uint64_t) HeapSize(arg1, arg2, arg3);
    }
    else
    {
        
    }
    return 0;
}



STATIC PVOID WINAPI HeapReAlloc(HANDLE hHeap, DWORD dwFlags, PVOID lpMem, SIZE_T dwBytes)
{
//    return realloc(lpMem, dwBytes);
    return host_realloc(lpMem, dwBytes);
}

DEF_HOST_FUNC(winHeap, HeapReAlloc)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*) w32.next<WIN32_PTR>();
        uint32_t arg2 = w32.next<uint32_t>();
        void* arg3 = (void*)w32.next<WIN32_PTR>();
        uint32_t arg4 = w32.next<uint32_t>();
        return (uint64_t) HeapReAlloc(arg1, arg2, arg3,arg4);
    }
    else
    {
        
    }
    return 0;
}

STATIC PVOID WINAPI LocalAlloc(UINT uFlags, SIZE_T uBytes)
{
    PVOID Buffer = host_malloc(uBytes);
    assert(uFlags == 0);

    LOG_DEBUG("%#x, %u => %p", uFlags, uBytes, Buffer);

    return Buffer;
}

DEF_HOST_FUNC(winHeap, LocalAlloc)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        uint32_t arg1 = w32.next<uint32_t>();
        uint32_t arg2 = w32.next<uint32_t>();
        return (uint64_t) LocalAlloc(arg1, arg2);
    }
    else
    {
        
    }
    return 0;
}


STATIC PVOID WINAPI LocalFree(PVOID hMem)
{
    LOG_DEBUG("%p", hMem);
//    free(hMem);
    host_free(hMem);
    return NULL;
}
DEF_HOST_FUNC(winHeap, LocalFree)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*)w32.next<WIN32_PTR>();
        return (uint64_t) LocalFree(arg1);
    }
    else
    {
        
    }
    return 0;
}


void winHeap::init_funcs()
{
    DEF_STD_USER_HOST_CALL(winHeap, HeapCreate, 12);
    DEF_STD_USER_HOST_CALL(winHeap, GetProcessHeap,0);
    DEF_STD_USER_HOST_CALL(winHeap, HeapAlloc, 12);
    DEF_STD_USER_HOST_CALL(winHeap, HeapFree, 12);
    DEF_STD_USER_HOST_CALL(winHeap, HeapSize, 12);
    DEF_STD_USER_HOST_CALL(winHeap, HeapReAlloc,16);
    DEF_STD_USER_HOST_CALL(winHeap, LocalAlloc, 8);
    DEF_STD_USER_HOST_CALL(winHeap, LocalFree,4);
}

void init_caller_winHeap()
{
    static winHeap win_heap;
}

