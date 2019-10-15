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

class winEvent:public HostCallerBase
{
#undef DECLARE_CRT_EXPORT
#define DECLARE_CRT_EXPORT(name,func)\
static uint64_t wrap_##func(uint64_t* args);
    DECLARE_CRT_EXPORT("CreateEventA", CreateEventA);
    DECLARE_CRT_EXPORT("CreateEventW", CreateEventW);
    DECLARE_CRT_EXPORT("SetEvent", SetEvent);
    DECLARE_CRT_EXPORT("ResetEvent", ResetEvent);
    
#undef DECLARE_CRT_EXPORT
    virtual void init_funcs();
    
};

#define DEF_HOST_FUNC(cls,func) \
uint64_t cls::wrap_##func(uint64_t* args)



static HANDLE CreateEventA(
                    PVOID lpEventAttributes,
                    BOOL                  bManualReset,
                    BOOL                  bInitialState,
                    LPSTR                lpName)
{
    LOG_DEBUG("CreateEventA:%p, %u, %u, %p [%s]", lpEventAttributes, bManualReset, bInitialState, lpName,lpName);
     return (HANDLE) 'EVNT';
}

DEF_HOST_FUNC(winEvent,CreateEventA)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*) w32.next<WIN32_PTR>();
        uint32_t arg2 = w32.next<uint32_t>();
        uint32_t arg3 = w32.next<uint32_t>();
        char* arg4  = (char*)getMemAddr(w32.next<WIN32_PTR>());
        return (uint64_t)CreateEventA(arg1, arg2, arg3, arg4);
    }
    else
    {
        
    }
    return 0;
}

static HANDLE WINAPI CreateEventW(PVOID lpEventAttributes, BOOL bManualReset, BOOL bInitialState, PWCHAR lpName)
{
    char *AnsiName;
#ifndef NDEBUG
    AnsiName = lpName ? CreateAnsiFromWide(lpName) : NULL;
#else
    AnsiName = NULL;
#endif

    LOG_DEBUG("%p, %u, %u, %p [%s]", lpEventAttributes, bManualReset, bInitialState, lpName, AnsiName);

    free(AnsiName);

    return (HANDLE) 'EVNT';
}

DEF_HOST_FUNC(winEvent,CreateEventW)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*) w32.next<WIN32_PTR>();
        uint32_t arg2 = w32.next<uint32_t>();
        uint32_t arg3 = w32.next<uint32_t>();
        uint16_t* arg4  = (uint16_t*)getMemAddr(w32.next<WIN32_PTR>());
        return (uint64_t)CreateEventW(arg1, arg2, arg3, arg4);
    }
    else
    {
        
    }
    return 0;
}


static BOOL WINAPI SetEvent(HANDLE hEvent)
{
    LOG_DEBUG("%p", hEvent);
    return TRUE;
}

DEF_HOST_FUNC(winEvent,SetEvent)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*) w32.next<WIN32_PTR>();
        return (uint64_t)SetEvent(arg1);
    }
    else
    {
        
    }
    return 0;
}

static BOOL WINAPI ResetEvent(HANDLE hEvent)
{
    LOG_DEBUG("%p", hEvent);
    return TRUE;
}

DEF_HOST_FUNC(winEvent,ResetEvent)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*) w32.next<WIN32_PTR>();
        return (uint64_t)ResetEvent(arg1);
    }
    else
    {
        
    }
    return 0;
}

void winEvent::init_funcs()
{
    DEF_STD_USER_HOST_CALL(winEvent, CreateEventA,16);
    DEF_STD_USER_HOST_CALL(winEvent, CreateEventW,16);
	DEF_STD_USER_HOST_CALL(winEvent, SetEvent,4);
	DEF_STD_USER_HOST_CALL(winEvent, ResetEvent,4);
}

void init_caller_winEvent()
{
    static winEvent win_event;
}
