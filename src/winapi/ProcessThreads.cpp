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

class winProcessThreads:public HostCallerBase
{
#undef DECLARE_CRT_EXPORT
#define DECLARE_CRT_EXPORT(name,func)\
static uint64_t wrap_##func(uint64_t* args);
    DECLARE_CRT_EXPORT("RtlNtStatusToDosError", RtlNtStatusToDosError);
    DECLARE_CRT_EXPORT("GetThreadTimes", GetThreadTimes);
    DECLARE_CRT_EXPORT("GetCurrentThread", GetCurrentThread);
    DECLARE_CRT_EXPORT("CreateTimerQueueTimer", CreateTimerQueueTimer);
    DECLARE_CRT_EXPORT("RegisterWaitForSingleObject", RegisterWaitForSingleObject);
    DECLARE_CRT_EXPORT("WaitForSingleObject", WaitForSingleObject);
    DECLARE_CRT_EXPORT("GetCurrentProcess", GetCurrentProcess);
    DECLARE_CRT_EXPORT("LsaNtStatusToWinError", LsaNtStatusToWinError);
    DECLARE_CRT_EXPORT("SetThreadToken", SetThreadToken);
    DECLARE_CRT_EXPORT("InterlockedDecrement", InterlockedDecrement);
    DECLARE_CRT_EXPORT("InterlockedIncrement", InterlockedIncrement);
    DECLARE_CRT_EXPORT("InterlockedCompareExchange", InterlockedCompareExchange);
    DECLARE_CRT_EXPORT("CreateSemaphoreA", CreateSemaphoreA);
    DECLARE_CRT_EXPORT("CreateSemaphoreW", CreateSemaphoreW);
    DECLARE_CRT_EXPORT("AcquireSRWLockExclusive", AcquireSRWLockExclusive);
    DECLARE_CRT_EXPORT("InitializeSRWLock", InitializeSRWLock);
    DECLARE_CRT_EXPORT("ReleaseSRWLockExclusive", ReleaseSRWLockExclusive);
    DECLARE_CRT_EXPORT("CreateThreadpoolTimer", CreateThreadpoolTimer);
    DECLARE_CRT_EXPORT("SetThreadpoolTimer", SetThreadpoolTimer);
    DECLARE_CRT_EXPORT("CloseThreadpoolTimer",CloseThreadpoolTimer);
    DECLARE_CRT_EXPORT("WaitForThreadpoolTimerCallbacks", WaitForThreadpoolTimerCallbacks);
    DECLARE_CRT_EXPORT("GetCurrentThreadId", GetCurrentThreadId);
    DECLARE_CRT_EXPORT("GetCurrentProcessId", GetCurrentProcessId);
    DECLARE_CRT_EXPORT("ProcessIdToSessionId", ProcessIdToSessionId);
    DECLARE_CRT_EXPORT("CreateMutexW",CreateMutexW);
    DECLARE_CRT_EXPORT("CreateMutexA",CreateMutexA);
    
#undef DECLARE_CRT_EXPORT
    virtual void init_funcs();
    
};

#define DEF_HOST_FUNC(cls,func) \
uint64_t cls::wrap_##func(uint64_t* args)

static PVOID WINAPI CreateThreadpoolTimer(PVOID pfnti, PVOID pv, PVOID pcbe)
{
    // LOG_DEBUG("%p, %p, %p", pfnti, pv, pcbe);
    return (PVOID) 'POOL';
}

DEF_HOST_FUNC(winProcessThreads,CreateThreadpoolTimer)
{
    return (uint64_t)CreateThreadpoolTimer(0,0,0);
}

static VOID WINAPI InitializeSRWLock(PVOID SRWLock)
{
    LOG_DEBUG("%p", SRWLock);
}

DEF_HOST_FUNC(winProcessThreads,InitializeSRWLock)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*)w32.next<WIN32_PTR>();
        InitializeSRWLock(arg1);
    }
    else
    {
        
    }
    return 0;
}

static VOID WINAPI SetThreadpoolTimer(PVOID pti, PVOID pftDueTime, DWORD msPeriod, DWORD msWindowLength)
{
    LOG_DEBUG("%p, %p, %u, %u", pti, pftDueTime, msPeriod, msWindowLength);

    assert(pftDueTime == NULL);
}

DEF_HOST_FUNC(winProcessThreads,SetThreadpoolTimer)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*)w32.next<WIN32_PTR>();
        void* arg2 = (void*)w32.next<WIN32_PTR>();
        uint32_t arg3 = w32.next<uint32_t>();
        uint32_t arg4 = w32.next<uint32_t>();
        SetThreadpoolTimer(arg1,arg2,arg3,arg4);
    }
    else
    {
        
    }
    return 0;
}

static VOID WINAPI WaitForThreadpoolTimerCallbacks(PVOID pti, BOOL fCancelPendingCallbacks)
{
    LOG_DEBUG("%p, %u", pti, fCancelPendingCallbacks);
}

DEF_HOST_FUNC(winProcessThreads,WaitForThreadpoolTimerCallbacks)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*)w32.next<WIN32_PTR>();
        uint32_t arg2 = w32.next<uint32_t>();
        WaitForThreadpoolTimerCallbacks(arg1,arg2);
    }
    else
    {
        
    }
    return 0;
}

static VOID WINAPI CloseThreadpoolTimer(PVOID pti)
{
    LOG_DEBUG("%p", pti);
}

DEF_HOST_FUNC(winProcessThreads,CloseThreadpoolTimer)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*)w32.next<WIN32_PTR>();
        CloseThreadpoolTimer(arg1);
    }
    else
    {
        
    }
    return 0;
}


static LONG InterlockedDecrement(PULONG Addend)
{
    LOG_DEBUG("%p", Addend);
    return --*Addend;
}

DEF_HOST_FUNC(winProcessThreads,InterlockedDecrement)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        PULONG arg1 = (PULONG)getMemAddr(w32.next<WIN32_PTR>());
        return InterlockedDecrement(arg1);
    }
    else
    {
        
    }
    return 0;
}

static LONG InterlockedIncrement(PULONG Addend)
{
    LOG_DEBUG("%p", Addend);
    return ++*Addend;
}

DEF_HOST_FUNC(winProcessThreads,InterlockedIncrement)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        PULONG arg1 = (PULONG)getMemAddr(w32.next<WIN32_PTR>());
        return InterlockedIncrement(arg1);
    }
    else
    {
        
    }
    return 0;
}

static LONG InterlockedCompareExchange(PULONG Destination, LONG Exchange, LONG Comparand)
{
    LOG_DEBUG("%p", Destination);
    if (*Destination == Comparand) {
        *Destination = Exchange;
    }
    return *Destination;
}

DEF_HOST_FUNC(winProcessThreads,InterlockedCompareExchange)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        PULONG arg1 = (PULONG)getMemAddr(w32.next<WIN32_PTR>());
        uint32_t arg2 = w32.next<uint32_t>();
        uint32_t arg3 = w32.next<uint32_t>();
        return InterlockedCompareExchange(arg1,arg2,arg3);
    }
    else
    {
        
    }
    return 0;
}

static HANDLE CreateSemaphoreA(
                        PVOID lpSemaphoreAttributes,
                        LONG                  lInitialCount,
                        LONG                  lMaximumCount,
                        LPSTR                lpName
                        )
{
    LOG_DEBUG("%p, %u, %u, %p [%s]", lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName,lpName);
     return (HANDLE) 'SEMA';
}

DEF_HOST_FUNC(winProcessThreads,CreateSemaphoreA)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*)(w32.next<WIN32_PTR>());
        uint32_t arg2 = w32.next<uint32_t>();
        uint32_t arg3 = w32.next<uint32_t>();
        char* arg4 = ( char*)getMemAddr(w32.next<WIN32_PTR>());
        return (uint64_t)CreateSemaphoreA(arg1,arg2,arg3,arg4);
    }
    else
    {
        
    }
    return 0;
}

static HANDLE WINAPI CreateSemaphoreW(PVOID lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, PWCHAR lpName)
{
    char *name;
#ifndef NDEBUG
    name = CreateAnsiFromWide(lpName);
#else
    name = NULL;
#endif
    LOG_DEBUG("%p, %u, %u, %p [%s]", lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, name);
    free(name);
    return (HANDLE) 'SEMA';
}


DEF_HOST_FUNC(winProcessThreads,CreateSemaphoreW)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*)(w32.next<WIN32_PTR>());
        uint32_t arg2 = w32.next<uint32_t>();
        uint32_t arg3 = w32.next<uint32_t>();
        uint16_t* arg4 = ( uint16_t*)getMemAddr(w32.next<WIN32_PTR>());
        return (uint64_t)CreateSemaphoreW(arg1,arg2,arg3,arg4);
    }
    else
    {
        
    }
    return 0;
}

static HANDLE WINAPI GetCurrentProcess(VOID)
{
    return (HANDLE) -1;
}
DEF_HOST_FUNC(winProcessThreads,GetCurrentProcess)
{
    return (uint64_t)GetCurrentProcess();
}

static HANDLE WINAPI GetCurrentThread(VOID)
{
    return (HANDLE) -1;
}
DEF_HOST_FUNC(winProcessThreads,GetCurrentThread)
{
    return (uint64_t)GetCurrentThread();
}

static DWORD WINAPI GetCurrentThreadId(VOID)
{
    return getpid();
}

DEF_HOST_FUNC(winProcessThreads,GetCurrentThreadId)
{
    return (uint64_t)GetCurrentThreadId();
}

static DWORD WINAPI GetCurrentProcessId(VOID)
{
    return getpid();
}
DEF_HOST_FUNC(winProcessThreads,GetCurrentProcessId)
{
    return (uint64_t)GetCurrentProcessId();
}



static BOOL WINAPI RegisterWaitForSingleObject(PHANDLE phNewWaitObject, HANDLE hObject, PVOID Callback, PVOID Context, ULONG dwMilliseconds, ULONG dwFlags)
{
    LOG_DEBUG("");
    return TRUE;
}


DEF_HOST_FUNC(winProcessThreads,RegisterWaitForSingleObject)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void** arg1 = (void**)(w32.next<WIN32_PTR>());
        void* arg2 = (void*)(w32.next<WIN32_PTR>());
        void* arg3 = (void*)(w32.next<WIN32_PTR>());
        void* arg4 = (void*)(w32.next<WIN32_PTR>());
        
        uint32_t arg5 = w32.next<uint32_t>();
        uint32_t arg6 = w32.next<uint32_t>();
        return (uint64_t)RegisterWaitForSingleObject(arg1,arg2,arg3,arg4,arg5,arg6);
    }
    else
    {
        
    }
    return 0;
}


static VOID WINAPI AcquireSRWLockExclusive(PVOID SRWLock)
{
    LOG_DEBUG("%p", SRWLock);
}

DEF_HOST_FUNC(winProcessThreads,AcquireSRWLockExclusive)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*)(w32.next<WIN32_PTR>());
        AcquireSRWLockExclusive(arg1);
    }
    else
    {
        
    }
    return 0;
}

static VOID WINAPI ReleaseSRWLockExclusive(PVOID SRWLock)
{
    LOG_DEBUG("%p", SRWLock);
}

DEF_HOST_FUNC(winProcessThreads,ReleaseSRWLockExclusive)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*)(w32.next<WIN32_PTR>());
        ReleaseSRWLockExclusive(arg1);
    }
    else
    {
        
    }
    return 0;
}

static HANDLE WINAPI CreateMutexA(PVOID lpMutexAttributes, BOOL bInitialOwner, LPSTR lpName)
{
    LOG_DEBUG("%p, %u, %p",lpMutexAttributes,bInitialOwner,lpName,lpName);
    return INVALID_HANDLE_VALUE;
}

DEF_HOST_FUNC(winProcessThreads,CreateMutexA)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*)(w32.next<WIN32_PTR>());
        uint32_t arg2 = w32.next<uint32_t>();
        char* arg3 = (char*)getMemAddr(w32.next<WIN32_PTR>());
        return (uint64_t)CreateMutexA(arg1,arg2,arg3);
    }
    else
    {
        
    }
    return 0;
}

static HANDLE WINAPI CreateMutexW(PVOID lpMutexAttributes, BOOL bInitialOwner, PWCHAR lpName)
{
    LOG_DEBUG("%p, %u, %p",lpMutexAttributes,bInitialOwner,lpName);
    return INVALID_HANDLE_VALUE;
}

DEF_HOST_FUNC(winProcessThreads,CreateMutexW)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*)(w32.next<WIN32_PTR>());
        uint32_t arg2 = w32.next<uint32_t>();
        uint16_t* arg3 = (uint16_t*)getMemAddr(w32.next<WIN32_PTR>());
        return (uint64_t)CreateMutexW(arg1,arg2,arg3);
    }
    else
    {
        
    }
    return 0;
}




static DWORD WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
{
    LOG_DEBUG("%p, %u", hHandle, dwMilliseconds);
    return -1;
}

DEF_HOST_FUNC(winProcessThreads,WaitForSingleObject)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1 = (void*)(w32.next<WIN32_PTR>());
        uint32_t arg2 = w32.next<uint32_t>();
        return WaitForSingleObject(arg1,arg2);
    }
    else
    {
        
    }
    return 0;
}

static ULONG WINAPI LsaNtStatusToWinError(NTSTATUS Status)
{
    LOG_DEBUG("%#x", Status);
    return Status;
}

DEF_HOST_FUNC(winProcessThreads,LsaNtStatusToWinError)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        uint32_t arg1 = (w32.next<uint32_t>());

        return LsaNtStatusToWinError(arg1);
    }
    else
    {
        
    }
    return 0;
}

static BOOL WINAPI CreateTimerQueueTimer(PHANDLE phNewTimer,
                                         HANDLE TimerQueue,
                                         PVOID Callback,
                                         PVOID Parameter,
                                         DWORD DueTime,
                                         DWORD Period,
                                         ULONG Flags)
{
    LOG_DEBUG("");
    return TRUE;
}

DEF_HOST_FUNC(winProcessThreads,CreateTimerQueueTimer)
{
    return TRUE;
}

static BOOL WINAPI GetThreadTimes(HANDLE hThread,
                                  PFILETIME lpCreationTime,
                                  PFILETIME lpExitTime,
                                  PFILETIME lpKernelTime,
                                  PFILETIME lpUserTime)
{
    LOG_DEBUG("");
    return TRUE;
}

DEF_HOST_FUNC(winProcessThreads,GetThreadTimes)
{
    return TRUE;
}

static ULONG WINAPI RtlNtStatusToDosError(NTSTATUS Status)
{
    LOG_DEBUG("%#x", Status);
    return 5;
}

DEF_HOST_FUNC(winProcessThreads,RtlNtStatusToDosError)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        uint32_t arg1 = (w32.next<uint32_t>());
        
        return RtlNtStatusToDosError(arg1);
    }
    else
    {
        
    }
    return 0;
}

static BOOL WINAPI SetThreadToken(PHANDLE Thread, HANDLE Token)
{
    LOG_DEBUG("");
    return FALSE;
}

DEF_HOST_FUNC(winProcessThreads,SetThreadToken)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        PHANDLE arg1 = (PHANDLE)(w32.next<uint32_t>());
        HANDLE arg2 = (HANDLE)(w32.next<uint32_t>());
        
        return SetThreadToken(arg1,arg2);
    }
    else
    {
        
    }
    return 0;
}



static BOOL WINAPI ProcessIdToSessionId(DWORD dwProcessId, DWORD *pSessionId)
{
    LOG_DEBUG("");
    return FALSE;
}

DEF_HOST_FUNC(winProcessThreads,ProcessIdToSessionId)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        uint32_t arg1 = (uint32_t)(w32.next<uint32_t>());
        uint32_t* arg2 = (uint32_t*)(w32.next<uint32_t>());
        
        return ProcessIdToSessionId(arg1,arg2);
    }
    else
    {
        
    }
    return 0;
}

void winProcessThreads::init_funcs()
{
    DEF_STD_USER_HOST_CALL(winProcessThreads, RtlNtStatusToDosError,4);
	DEF_STD_USER_HOST_CALL(winProcessThreads, GetThreadTimes,20);
	DEF_STD_USER_HOST_CALL(winProcessThreads, GetCurrentThread,0);
	DEF_STD_USER_HOST_CALL(winProcessThreads, CreateTimerQueueTimer,28);
	DEF_STD_USER_HOST_CALL(winProcessThreads, RegisterWaitForSingleObject,24);
	DEF_STD_USER_HOST_CALL(winProcessThreads, WaitForSingleObject,8);
	DEF_STD_USER_HOST_CALL(winProcessThreads, GetCurrentProcess,0);
	DEF_STD_USER_HOST_CALL(winProcessThreads, LsaNtStatusToWinError,4);
	DEF_STD_USER_HOST_CALL(winProcessThreads, SetThreadToken,8);
	DEF_STD_USER_HOST_CALL(winProcessThreads, InterlockedDecrement,4);
	DEF_STD_USER_HOST_CALL(winProcessThreads, InterlockedIncrement,4);
	DEF_STD_USER_HOST_CALL(winProcessThreads, InterlockedCompareExchange,12);
    DEF_STD_USER_HOST_CALL(winProcessThreads, CreateSemaphoreA,16);
	DEF_STD_USER_HOST_CALL(winProcessThreads, CreateSemaphoreW,16);
	DEF_STD_USER_HOST_CALL(winProcessThreads, AcquireSRWLockExclusive,4);
	DEF_STD_USER_HOST_CALL(winProcessThreads, InitializeSRWLock,4);
	DEF_STD_USER_HOST_CALL(winProcessThreads, ReleaseSRWLockExclusive,4);
    DEF_STD_USER_HOST_CALL(winProcessThreads, CreateThreadpoolTimer,12);
	DEF_STD_USER_HOST_CALL(winProcessThreads, SetThreadpoolTimer,16);
    DEF_STD_USER_HOST_CALL(winProcessThreads, CloseThreadpoolTimer, 4);
	DEF_STD_USER_HOST_CALL(winProcessThreads, WaitForThreadpoolTimerCallbacks,8);
	DEF_STD_USER_HOST_CALL(winProcessThreads, GetCurrentThreadId,0);
	DEF_STD_USER_HOST_CALL(winProcessThreads, GetCurrentProcessId,0);
	DEF_STD_USER_HOST_CALL(winProcessThreads, ProcessIdToSessionId,8);
    DEF_STD_USER_HOST_CALL(winProcessThreads, CreateMutexW, 12);
    DEF_STD_USER_HOST_CALL(winProcessThreads, CreateMutexA, 12);

}

static winProcessThreads win_pt;



