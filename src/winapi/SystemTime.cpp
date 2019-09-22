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

typedef struct _SYSTEMTIME {
  WORD wYear;
  WORD wMonth;
  WORD wDayOfWeek;
  WORD wDay;
  WORD wHour;
  WORD wMinute;
  WORD wSecond;
  WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME;

extern void  SetLastError(DWORD dwErrCode);

class SysTime:public HostCallerBase
{
#undef DECLARE_CRT_EXPORT
#define DECLARE_CRT_EXPORT(name,func)\
static uint64_t wrap_##func(uint64_t* args);
    
    DECLARE_CRT_EXPORT("GetSystemTime", GetSystemTime);
    DECLARE_CRT_EXPORT("SystemTimeToFileTime", SystemTimeToFileTime);
    DECLARE_CRT_EXPORT("GetSystemTimePreciseAsFileTime", GetSystemTimePreciseAsFileTime);
    DECLARE_CRT_EXPORT("GetSystemTimeAsFileTime", GetSystemTimeAsFileTime);
    DECLARE_CRT_EXPORT("QueryPerformanceCounter", QueryPerformanceCounter);
    DECLARE_CRT_EXPORT("QueryPerformanceFrequency", QueryPerformanceFrequency);
    DECLARE_CRT_EXPORT("GetTickCount", GetTickCount);
    DECLARE_CRT_EXPORT("GetProcessTimes", GetProcessTimes);
    
#undef DECLARE_CRT_EXPORT
    
    virtual void init_funcs();
};

#define DEF_HOST_FUNC(cls,func) \
uint64_t cls::wrap_##func(uint64_t* args)

// These routines are called to check if signing certificates have expired, so
// should return similar values.

STATIC VOID  GetSystemTime(PSYSTEMTIME lpSystemTime)
{
    memset(lpSystemTime, 0, sizeof(SYSTEMTIME));
    return;
}
DEF_HOST_FUNC(SysTime, GetSystemTime)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        PSYSTEMTIME lp = (PSYSTEMTIME) getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
        GetSystemTime(lp);
    }
    else
    {
        
    }
    return 0;
}



STATIC BOOL  SystemTimeToFileTime(SYSTEMTIME *lpSystemTime, PFILETIME lpFileTime)
{
    memset(lpFileTime, 0, sizeof(FILETIME));
    return TRUE;
}

DEF_HOST_FUNC(SysTime, SystemTimeToFileTime)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        SYSTEMTIME* arg1 = (SYSTEMTIME*) getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
        PFILETIME arg2 = (PFILETIME) getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
       return SystemTimeToFileTime(arg1,arg2);
    }
    else
    {
        
    }
    return TRUE;
}

STATIC VOID  GetSystemTimePreciseAsFileTime(PFILETIME lpSystemTimeAsFileTime)
{
    memset(lpSystemTimeAsFileTime, 0, sizeof(FILETIME));
    return;
}

DEF_HOST_FUNC(SysTime, GetSystemTimePreciseAsFileTime)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        PFILETIME lp = (PFILETIME) getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
        GetSystemTimePreciseAsFileTime(lp);
    }
    else
    {
        
    }
    return 0;
}

STATIC VOID  GetSystemTimeAsFileTime(PVOID lpSystemTimeAsFileTime)
{
    memset(lpSystemTimeAsFileTime, 0, sizeof(FILETIME));
    return;
}

DEF_HOST_FUNC(SysTime, GetSystemTimeAsFileTime)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        PVOID lp = (PVOID) getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
        GetSystemTimeAsFileTime(lp);
    }
    else
    {
        
    }
    return 0;
}

STATIC BOOL  QueryPerformanceCounter(PVOID lpPerformanceCount)
{
    SetLastError(0);
    return FALSE;
}

DEF_HOST_FUNC(SysTime, QueryPerformanceCounter)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        PVOID lp = (PVOID) getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
        return QueryPerformanceCounter(lp);
    }
    else
    {
        
    }
    return FALSE;
}

STATIC DWORD  GetTickCount(VOID)
{
    return 0;
}

DEF_HOST_FUNC(SysTime, GetTickCount)
{
//    if(is_cpu_mode32())
    {
        return GetTickCount();
    }
//    else
    {
        
    }
//    return FALSE;
}


STATIC BOOL  QueryPerformanceFrequency(PVOID lpFrequency)
{
    SetLastError(0);
    return FALSE;
}

DEF_HOST_FUNC(SysTime, QueryPerformanceFrequency)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        PVOID lp = (PVOID) getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
        return QueryPerformanceFrequency(lp);
    }
    else
    {
        
    }
    return FALSE;
}

STATIC BOOL  GetProcessTimes(HANDLE hProcess, PFILETIME lpCreationTime, PFILETIME lpExitTime, PFILETIME lpKernelTime, PFILETIME lpUserTime)
{
    SetLastError(0);
    LOG_DEBUG("GetProcessTimes not imp!\n");
    return FALSE;
}

DEF_HOST_FUNC(SysTime, GetProcessTimes)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        HANDLE arg1 = (HANDLE)w32.next<WIN32_PTR>();
        PFILETIME arg2 = (PFILETIME)getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
        PFILETIME arg3 = (PFILETIME)getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
        PFILETIME arg4 = (PFILETIME)getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
        PFILETIME arg5 = (PFILETIME)getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());

        return GetProcessTimes(arg1,arg2,arg3,arg4,arg5);
    }
    else
    {
        
    }
    return FALSE;
}


//virtual void init_funcs()
    
void SysTime::init_funcs()
{
    DEF_STD_USER_HOST_CALL(SysTime,GetSystemTime,4);
    DEF_STD_USER_HOST_CALL(SysTime,SystemTimeToFileTime,8);
    DEF_STD_USER_HOST_CALL(SysTime,GetSystemTimePreciseAsFileTime,4);
    DEF_STD_USER_HOST_CALL(SysTime,GetSystemTimeAsFileTime,4);
    DEF_STD_USER_HOST_CALL(SysTime,QueryPerformanceCounter,4);
    DEF_STD_USER_HOST_CALL(SysTime,QueryPerformanceFrequency,4);
    DEF_STD_USER_HOST_CALL(SysTime,GetTickCount,0);
    DEF_STD_USER_HOST_CALL(SysTime,GetProcessTimes,20);
}

static SysTime sys_time;
