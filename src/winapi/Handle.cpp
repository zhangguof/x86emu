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

#undef WINAPI
#define WINAPI

class winHandle:public HostCallerBase
{
#undef DECLARE_CRT_EXPORT
#define DECLARE_CRT_EXPORT(name,func)\
static uint64_t wrap_##func(uint64_t* args);
    DECLARE_CRT_EXPORT("DuplicateHandle", DuplicateHandle);
    DECLARE_CRT_EXPORT("SetHandleCount", SetHandleCount);
    
#undef DECLARE_CRT_EXPORT
    virtual void init_funcs();
    
};

#define DEF_HOST_FUNC(cls,func) \
uint64_t cls::wrap_##func(uint64_t* args)

STATIC BOOL WINAPI DuplicateHandle(uint32_t hSourceProcessHandle, HANDLE hSourceHandle, HANDLE hTargetProcessHandle, uint32_t* lpTargetHandle, DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwOptions)
{
    LOG_DEBUG("%u, %p, %p, %p, %#x, %u, %#x", hSourceProcessHandle, hSourceHandle, hTargetProcessHandle, lpTargetHandle, dwDesiredAccess, bInheritHandle, dwOptions);

    // lol i dunno
    *lpTargetHandle = hSourceProcessHandle;
    return TRUE;
}

DEF_HOST_FUNC(winHandle,DuplicateHandle)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        uint32_t arg1 = (uint32_t)w32.next<uint32_t>();
        void* arg2 = (void*)w32.next<WIN32_PTR>();
        void* arg3 = (void*)w32.next<WIN32_PTR>();
        uint32_t* arg4 = (uint32_t*)getMemAddr(w32.next<WIN32_PTR>());
        uint32_t arg5 = w32.next<uint32_t>();
        uint32_t arg6 = w32.next<uint32_t>();
        uint32_t arg7 = w32.next<uint32_t>();
        return DuplicateHandle(arg1,arg2,arg3,arg4,arg5,arg6,arg7);
        
    }
    else
    {
        
    }
    return 0;
}

STATIC UINT WINAPI SetHandleCount(UINT handleCount)
{
    LOG_DEBUG("%u", handleCount);
    return handleCount;
}

DEF_HOST_FUNC(winHandle,SetHandleCount)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        uint32_t arg1 = w32.next<uint32_t>();
        return SetHandleCount(arg1);
    }
    return 0;
}

void winHandle::init_funcs()
{
    DEF_STD_USER_HOST_CALL(winHandle,DuplicateHandle,28);
    DEF_STD_USER_HOST_CALL(winHandle,SetHandleCount,4);
}

static winHandle win_h;
