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
#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))


#ifndef TLS_OUT_OF_INDEXES
# define TLS_OUT_OF_INDEXES 0xFFFFFFFF
#endif

static int TlsIndex;
uintptr_t LocalStorage[1024];

STATIC DWORD  TlsAlloc(void)
{
    if (TlsIndex >= ARRAY_SIZE(LocalStorage) - 1) {
        LOG_DEBUG("TlsAlloc() => %#x", TlsIndex);
        return TLS_OUT_OF_INDEXES;
    }

    return TlsIndex++;
}

DECLARE_CRT_EXPORT("TlsAlloc", TlsAlloc)
{
    return TlsAlloc();
}

STATIC BOOL  TlsSetValue(DWORD dwTlsIndex, PVOID lpTlsValue)
{
    LOG_DEBUG("TlsSetValue(%u, %p)", dwTlsIndex, lpTlsValue);

    if (dwTlsIndex < ARRAY_SIZE(LocalStorage)) {
        LocalStorage[dwTlsIndex] = (uintptr_t) (lpTlsValue);
        return TRUE;
    }

    LOG_DEBUG("dwTlsIndex higher than current maximum");
    return FALSE;
}

DECLARE_CRT_EXPORT("TlsSetValue", TlsSetValue)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        uint32_t arg1 = w32_args.next<uint32_t>();
        WIN32_PTR arg2 = w32_args.next<WIN32_PTR>();
//        void* pargs2 = (void*)(getMemAddr(arg2));
        return TlsSetValue(arg1,(PVOID)arg2);
    }
    else
    {
        uint32_t arg1 = (uint32_t)args[0];
        uint64_t arg2 = args[1];
       return TlsSetValue(arg1,(PVOID)arg2);
    }
}

STATIC DWORD  TlsGetValue(DWORD dwTlsIndex)
{
    if (dwTlsIndex < ARRAY_SIZE(LocalStorage)) {
        return LocalStorage[dwTlsIndex];
    }

    return 0;
}

DECLARE_CRT_EXPORT("TlsGetValue", TlsGetValue)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        uint32_t arg1 = w32_args.next<uint32_t>();
        return TlsGetValue(arg1);
    }
    else
    {
        uint32_t arg1 = (uint32_t)args[0];
        return TlsGetValue(arg1);
    }
}

STATIC BOOL  TlsFree(DWORD dwTlsIndex)
{
    if (dwTlsIndex < ARRAY_SIZE(LocalStorage)) {
        LocalStorage[dwTlsIndex] = (uintptr_t) NULL;
        return TRUE;
    }

    return FALSE;
}

DECLARE_CRT_EXPORT("TlsFree", TlsFree)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        uint32_t arg1 = w32_args.next<uint32_t>();
        return TlsFree(arg1);
    }
    else
    {
        uint32_t arg1 = (uint32_t)args[0];
        return TlsFree(arg1);
    }
}


DECLARE_CRT_EXPORT("FlsFree", FlsFree)
{
    return wrap_TlsFree(args);
}
DECLARE_CRT_EXPORT("FlsAlloc", FlsAlloc)
{
    return wrap_TlsAlloc(args);
}
DECLARE_CRT_EXPORT("FlsSetValue", FlsSetValue)
{
    return wrap_TlsSetValue(args);
}
DECLARE_CRT_EXPORT("FlsGetValue", FlsGetValue)
{
    return wrap_TlsGetValue(args);
}
