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

// I don't implement threads, so critical sections are easy.

STATIC VOID  DeleteCriticalSection(PVOID lpCriticalSection)
{
    return;
}

STATIC VOID  EnterCriticalSection(PVOID lpCriticalSection)
{
    return;
}

STATIC VOID  LeaveCriticalSection(PVOID lpCriticalSection)
{
    return;
}

STATIC BOOL  InitializeCriticalSectionAndSpinCount(PVOID lpCriticalSection, DWORD dwSpinCount)
{
    return TRUE;
}

STATIC BOOL  InitializeCriticalSectionEx(PVOID lpCriticalSection, DWORD dwSpinCount, DWORD Flags)
{
    return TRUE;
}

STATIC VOID  InitializeCriticalSection(PVOID lpCriticalSection)
{
    return;
}
#undef DECLARE_CRT_EXPORT
#define DECLARE_CRT_EXPORT(name,_func) uint64_t wrap_##_func(uint64_t* args)

DECLARE_CRT_EXPORT("DeleteCriticalSection", DeleteCriticalSection)
{
    DeleteCriticalSection(0);
    return 0;
}
DECLARE_CRT_EXPORT("LeaveCriticalSection", LeaveCriticalSection)
{
    LeaveCriticalSection(0);
    return 0;
}
DECLARE_CRT_EXPORT("EnterCriticalSection", EnterCriticalSection)
{
    EnterCriticalSection(0);
    return 0;
}
DECLARE_CRT_EXPORT("InitializeCriticalSectionAndSpinCount", InitializeCriticalSectionAndSpinCount)
{
    return InitializeCriticalSectionAndSpinCount(0,0);
}
DECLARE_CRT_EXPORT("InitializeCriticalSectionEx", InitializeCriticalSectionEx)
{
    return InitializeCriticalSectionEx(0,0,0);
}
DECLARE_CRT_EXPORT("InitializeCriticalSection", InitializeCriticalSection)
{
    InitializeCriticalSection(0);
    return 0;
}
#undef DECLARE_CRT_EXPORT
