//
//  winapi.cpp
//  x86emu
//
//  Created by tony on 2019/9/18.
//  Copyright © 2019 tony. All rights reserved.
//
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

#define DEF_HOST_STD_FUNC(func) \
uint64_t wrap_##func(uint64_t* args)

//DWORD GetModuleFileNameA(
//                         HMODULE hModule,
//                         LPSTR   lpFilename,
//                         DWORD   nSize
//                         );
DWORD GetModuleFileNameA(
                         HMODULE hModule,
                         LPSTR   lpFilename,
                         DWORD   nSize
                         )
{
    const char* name = "C:\\x86Emu";
    uint32_t size = strlen(name);
    memcpy(lpFilename, name, size);
    return size;
}
DEF_HOST_STD_FUNC(GetModuleFileNameA)
{
    HMODULE hModule;
    char* lpFilename = nullptr;
    DWORD size = 0;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        hModule = (HMODULE)w32_args.next<WIN32_PTR>();
        lpFilename = (char*)getMemAddr(w32_args.next<WIN32_PTR>());
        size = w32_args.next<DWORD>();
        
    }
    else
    {
        hModule = (HMODULE) args[0];
        lpFilename = (char*)getMemAddr(args[1]);
        size = args[2];
    }
    return GetModuleFileNameA(hModule, lpFilename, size);
    
}
static void RtlGetNtVersionNumbers(DWORD* major,DWORD* minor,DWORD* build)
{
    //6.1.0
    *major = 6;
    *minor = 1;
    *build = 0;
}

DEF_HOST_STD_FUNC(RtlGetNtVersionNumbers)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        DWORD* arg1 = (DWORD*)getMemAddr(w32.next<WIN32_PTR>());
        DWORD* arg2 = (DWORD*)getMemAddr(w32.next<WIN32_PTR>());
        DWORD* arg3 = (DWORD*)getMemAddr(w32.next<WIN32_PTR>());
        RtlGetNtVersionNumbers(arg1, arg2, arg3);
    }
    else
    {
        DWORD* arg1 = (DWORD*)getMemAddr(args[0]);
        DWORD* arg2 = (DWORD*)getMemAddr(args[1]);
        DWORD* arg3 = (DWORD*)getMemAddr(args[2]);
        RtlGetNtVersionNumbers(arg1, arg2, arg3);
    }
    return 0;
}

static void OutputDebugStringA(const char* lpOutputString)
{
    LOG_DEBUG("[OutputDebugStringA]:%s\n",lpOutputString);
}
DEF_HOST_STD_FUNC(OutputDebugStringA)
{
    const char* s = nullptr;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        s = (const char*)getMemAddr(w32.next<WIN32_PTR>());
        
    }
    else
    {
        s = (const char*) getMemAddr((bx_phy_address)args[0]);
    }
    OutputDebugStringA(s);
    return 0;
}

static DWORD GetCurrentProcessId()
{
    return 0x1234;
}
static DWORD GetCurrentThreadId()
{
    return 0x4312;
}
//void __cdecl _lock(int locknum);
static void _lock(int locknum)
{
    LOG_DEBUG("_lock:%d\n",locknum);
}

static void _unlock(int locknum)
{
    LOG_DEBUG("_unlock:%d\n",locknum);
}

#define DEF_HOST_FUNC(func)\
uint64_t wrap_##func(uint64_t* args)


//use host caller cls
class WinApi:public HostCallerBase
{
    virtual void init_funcs();
    static DEF_HOST_FUNC(GetCurrentProcessId)
    {
        return GetCurrentProcessId();
    }
    static DEF_HOST_FUNC(GetCurrentThreadId)
    {
        return GetCurrentThreadId();
    }
    static DEF_HOST_FUNC(_lock)
    {
        if(is_cpu_mode32())
        {
            WIN32_ARGS w32 = {args};
            int n = w32.next<int>();
            _lock(n);
        }
        else
        {
            
        }
        return 0;
    }
    static DEF_HOST_FUNC(_unlock)
    {
        if(is_cpu_mode32())
        {
            WIN32_ARGS w32 = {args};
            int n = w32.next<int>();
            _unlock(n);
        }
        else
        {
            
        }
        return 0;
    }
    
};
//DWORD GetCurrentProcessId();
void WinApi::init_funcs()
{
    DEF_STD_USER_HOST_CALL(WinApi, GetCurrentProcessId,0);
    DEF_STD_USER_HOST_CALL(WinApi, GetCurrentThreadId, 0);
    DEF_STD_USER_HOST_CALL(WinApi, _lock, 0);//__cdecl
    DEF_STD_USER_HOST_CALL(WinApi, _unlock, 0); //__cdecl
}

static WinApi win_api;
