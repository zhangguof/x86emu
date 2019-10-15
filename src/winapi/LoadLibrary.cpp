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

extern "C"{
    extern char * CreateAnsiFromWide(void *wcharbuf);
}


static HANDLE  LoadLibraryExW(PVOID lpFileName, HANDLE hFile, DWORD dwFlags)
{
    char *name = CreateAnsiFromWide(lpFileName);

    LOG_DEBUG("%p [%s], %p, %#x", lpFileName, name, hFile, dwFlags);

    free(name);

    return (HANDLE) 'LOAD';
}

static HANDLE  LoadLibraryW(PVOID lpFileName)
{
    LOG_DEBUG("%p", lpFileName);

    return (HANDLE) 'LOAD';
}

static PVOID  GetProcAddress(HANDLE hModule, PCHAR lpProcName)
{
    void* ret = nullptr;
    get_export32(lpProcName, &ret);
//    if(ret)
    
    LOG_DEBUG("GetProcAddress:%p,%s:%p\n",hModule,lpProcName,ret);
    

    return ret;
}

static HANDLE  GetModuleHandleW(PVOID lpModuleName)
{
    char *name = CreateAnsiFromWide(lpModuleName);

    LOG_DEBUG("%p [%s]", lpModuleName, name);

    free(name);

    if (lpModuleName && memcmp(lpModuleName, L"mpengine.dll", sizeof(L"mpengine.dll")) == 0)
        return (HANDLE) 'MPEN';

    if (lpModuleName && memcmp(lpModuleName, L"bcrypt.dll", sizeof(L"bcrypt.dll")) == 0)
        return (HANDLE) 'LOAD';

    if (lpModuleName && memcmp(lpModuleName, L"KERNEL32.DLL", sizeof(L"KERNEL32.DLL")) == 0)
        return (HANDLE) 'KERN';

    if (lpModuleName && memcmp(lpModuleName, L"kernel32.dll", sizeof(L"kernel32.dll")) == 0)
        return (HANDLE) 'KERN';

    if (lpModuleName && memcmp(lpModuleName, L"version.dll", sizeof(L"version.dll")) == 0)
        return (HANDLE) 'VERS';
    return (HANDLE) NULL;
}

static DWORD  GetModuleFileNameA(HANDLE hModule, PCHAR lpFilename, DWORD nSize)
{
    LOG_DEBUG("%p, %p, %u", hModule, lpFilename, nSize);

    strncpy(lpFilename, "fakename.exe", nSize);

    return strlen(lpFilename);
}

static DWORD  GetModuleFileNameW(HANDLE hModule, PWCHAR lpFilename, DWORD nSize)
{
    LOG_DEBUG("%p, %p, %u", hModule, lpFilename, nSize);

    if (nSize > strlen("fakename.exe")) {
        memcpy(lpFilename, L"fakename.exe", sizeof(L"fakename.exe"));
    }

    return strlen("fakename.exe");
}

static HANDLE  GetModuleHandleA(PCHAR lpModuleName)
{
    LOG_DEBUG("%p [%s]", lpModuleName, lpModuleName);

    return (HANDLE) NULL;
}

static VOID  FreeLibrary(PVOID hLibModule)
{
    LOG_DEBUG("FreeLibrary(%p)", hLibModule);
}


#define DEF_HOST_FUNC(func) \
uint64_t wrap_##func(uint64_t* args)

class LoadLib:public HostCallerBase
{
    static DEF_HOST_FUNC(FreeLibrary)
    {
        if(is_cpu_mode32())
        {
            WIN32_ARGS w32 = {args};
            void* p = (void*)(w32.next<WIN32_PTR>());
            FreeLibrary(p);
        }
        else
        {
            //TODO
        }
        return 0;
    }
    //static HANDLE  LoadLibraryExW(PVOID lpFileName, HANDLE hFile, DWORD dwFlags)
    static DEF_HOST_FUNC(LoadLibraryExW)
    {
        if(is_cpu_mode32())
        {
            WIN32_ARGS  w32 = {args};
            const char* name = (char*)getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
            WIN32_PTR arg2 = w32.next<WIN32_PTR>();
            uint32_t arg3 = w32.next<uint32_t>();
            
            return (uint64_t)LoadLibraryExW((void*)name,(void*)arg2,arg3);
        }
        else{
            
        }
        return 0;
    }
    
    //static HANDLE  LoadLibraryW(PVOID lpFileName)
    static DEF_HOST_FUNC(LoadLibraryW)
    {
        if(is_cpu_mode32())
        {
            WIN32_ARGS  w32 = {args};
            const char* name = (char*)getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
            return (uint64_t)LoadLibraryW((void*)name);
        }
        else{
            
        }
        return 0;
    }
    
    //static HANDLE  GetModuleHandleW(PVOID lpModuleName)
    static DEF_HOST_FUNC(GetModuleHandleW)
    {
        if(is_cpu_mode32())
        {
            WIN32_ARGS  w32 = {args};
            const char* name = (char*)getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
            return (uint64_t)GetModuleHandleW((void*)name);
        }
        else{
            
        }
        return 0;
    }
    //static PVOID  GetProcAddress(HANDLE hModule, PCHAR lpProcName)
    static DEF_HOST_FUNC(GetProcAddress)
    {
        if(is_cpu_mode32())
        {
            WIN32_ARGS  w32 = {args};
            WIN32_PTR hm = w32.next<WIN32_PTR>();
            
            char* name = (char*)getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
            return (uint64_t)GetProcAddress((void*)hm,name);
        }
        else{
            
        }
        return 0;
    }
    //static DWORD  GetModuleFileNameA(HANDLE hModule, PCHAR lpFilename, DWORD nSize)
    static DEF_HOST_FUNC(GetModuleFileNameA)
    {
        if(is_cpu_mode32())
        {
            WIN32_ARGS  w32 = {args};
            WIN32_PTR hm = w32.next<WIN32_PTR>();
            char* name = (char*)getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
            uint32_t size = w32.next<uint32_t>();
            
            return (uint64_t)GetModuleFileNameA((void*)hm,name,size);
        }
        else{
            
        }
        return 0;
    }
    //static DWORD  GetModuleFileNameW(HANDLE hModule, PWCHAR lpFilename, DWORD nSize)
    static DEF_HOST_FUNC(GetModuleFileNameW)
    {
        if(is_cpu_mode32())
        {
            WIN32_ARGS  w32 = {args};
            WIN32_PTR hm = w32.next<WIN32_PTR>();
            uint16_t* name = (uint16_t*)getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
            uint32_t size = w32.next<uint32_t>();
            
            return (uint64_t)GetModuleFileNameW((void*)hm,name,size);
        }
        else{
            
        }
        return 0;
    }
    //static HANDLE  GetModuleHandleA(PCHAR lpModuleName)
    static DEF_HOST_FUNC(GetModuleHandleA)
    {
        if(is_cpu_mode32())
        {
            WIN32_ARGS  w32 = {args};

            char* name = (char*)getMemAddr((bx_phy_address)w32.next<WIN32_PTR>());
            
            return (uint64_t)GetModuleHandleA(name);
        }
        else{
            
        }
        return 0;
    }
    

    virtual void init_funcs()
    {
        DEF_STD_USER_HOST_CALL(LoadLib, FreeLibrary,4);
        DEF_STD_USER_HOST_CALL(LoadLib, LoadLibraryExW,12);
		DEF_STD_USER_HOST_CALL(LoadLib, LoadLibraryW,4);
		DEF_STD_USER_HOST_CALL(LoadLib, GetProcAddress,8);
		DEF_STD_USER_HOST_CALL(LoadLib, GetModuleHandleW,4);
		DEF_STD_USER_HOST_CALL(LoadLib, GetModuleHandleA,4);
		DEF_STD_USER_HOST_CALL(LoadLib, GetModuleFileNameA,12);
		DEF_STD_USER_HOST_CALL(LoadLib, GetModuleFileNameW,12);
    }
};
void init_caller_LoadLib()
{
    static LoadLib load_lib;
}

