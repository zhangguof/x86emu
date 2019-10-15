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

class winMem:public HostCallerBase
{
#undef DECLARE_CRT_EXPORT
#define DECLARE_CRT_EXPORT(name,func)\
static uint64_t wrap_##func(uint64_t* args);
    DECLARE_CRT_EXPORT("VirtualAlloc", VirtualAlloc);
    DECLARE_CRT_EXPORT("VirtualProtect", VirtualProtect);
    DECLARE_CRT_EXPORT("VirtualUnlock", VirtualUnlock);
    DECLARE_CRT_EXPORT("VirtualFree", VirtualFree);
    DECLARE_CRT_EXPORT("VirtualQuery",VirtualQuery);
    
#undef DECLARE_CRT_EXPORT
    virtual void init_funcs();
    
};

#define DEF_HOST_FUNC(cls,func) \
uint64_t cls::wrap_##func(uint64_t* args)



#define PAGE_READONLY 0x02
#define PAGE_READWRITE 0x04
#define PAGE_EXECUTE_READWRITE 0x40

#define MEM_COMMIT 0x00001000
#define MEM_RESERVE 0x00002000

#define MEM_RELEASE 0x8000


// Indicates that the memory pages within the region are mapped into the view of an image section.
#define MEM_IMAGE 0x1000000

// Indicates that the memory pages within the region are mapped into the view of a section.
#define MEM_MAPPED 0x40000

// Indicates that the memory pages within the region are private (that is, not shared by other processes).
#define MEM_PRIVATE 0x20000


STATIC PVOID WINAPI VirtualAlloc(PVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    if (flAllocationType & ~(MEM_COMMIT | MEM_RESERVE)) {
        LOG_DEBUG("flAllocationType %#x not implemnted", flAllocationType);
        return NULL;
    }

    // This VirtualAlloc() always returns PAGE_EXECUTE_READWRITE memory.
    if (flProtect == PAGE_READWRITE)
        return host_malloc(dwSize);
    if (flProtect == PAGE_EXECUTE_READWRITE) {
        LOG_DEBUG("JIT PAGE_EXECUTE_READWRITE Allocation Requested");
        return host_malloc(dwSize);
    }

    LOG_DEBUG("flProtect flags %#x not implemented", flProtect);
    return NULL;
}

//STATIC PVOID WINAPI VirtualAlloc(PVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
DEF_HOST_FUNC(winMem, VirtualAlloc)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1  = (void*) w32.next<WIN32_PTR>();
        uint32_t arg2  = w32.next<uint32_t>();
        uint32_t arg3 = w32.next<uint32_t>();
        uint32_t arg4 = w32.next<uint32_t>();
        return (uint64_t)VirtualAlloc(arg1, arg2, arg3, arg4);
    }
    else
    {
        
    }
    return 0;
}

STATIC BOOL WINAPI VirtualProtect(PVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
    if (flNewProtect != PAGE_READONLY) {
        LOG_DEBUG("unimplemented VirtualProtect() request, %#x", flNewProtect);
    }
    return TRUE;
}
DEF_HOST_FUNC(winMem, VirtualProtect)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1  = (void*) w32.next<WIN32_PTR>();
        uint32_t arg2  = w32.next<uint32_t>();
        uint32_t arg3 = w32.next<uint32_t>();
        uint32_t* arg4 = (uint32_t*) getMemAddr(w32.next<uint32_t>());
        return (uint64_t)VirtualProtect(arg1, arg2, arg3, arg4);
    }
    else
    {
        
    }
    return 0;
}

STATIC BOOL WINAPI VirtualUnlock(PVOID lpAddress, SIZE_T dwSize)
{
    return TRUE;
}

DEF_HOST_FUNC(winMem, VirtualUnlock)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1  = (void*) w32.next<WIN32_PTR>();
        uint32_t arg2  = w32.next<uint32_t>();
        return (uint64_t)VirtualUnlock(arg1, arg2);
    }
    else
    {
        
    }
    return 0;
}

STATIC BOOL WINAPI VirtualFree(PVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType)
{
    if (dwFreeType == MEM_RELEASE)
        host_free(lpAddress);
    return TRUE;
}

DEF_HOST_FUNC(winMem, VirtualFree)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1  = (void*) w32.next<WIN32_PTR>();
        uint32_t arg2  = w32.next<uint32_t>();
        uint32_t arg3 = w32.next<uint32_t>();
        return (uint64_t)VirtualFree(arg1, arg2, arg3);
    }
    else
    {
        
    }
    return 0;
}

typedef struct _MEMORY_BASIC_INFORMATION {
    WIN32_PTR  BaseAddress;
    WIN32_PTR  AllocationBase;
    DWORD  AllocationProtect;
    uint32_t RegionSize;
    DWORD  State;
    DWORD  Protect;
    DWORD  Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

static SIZE_T VirtualQuery(
                    void*                   lpAddress,
                    PMEMORY_BASIC_INFORMATION lpBuffer,
                    SIZE_T                    dwLength
                    )
{
    lpBuffer->BaseAddress = (WIN32_PTR)PAGE_BASE_ADDR;
    lpBuffer->AllocationBase = (WIN32_PTR)PAGE_BASE_ADDR;
    lpBuffer->AllocationProtect = PAGE_EXECUTE_READWRITE;
    lpBuffer->RegionSize = memSize - PAGE_BASE_ADDR;
    lpBuffer->State =  MEM_COMMIT;
    lpBuffer->Protect = PAGE_EXECUTE_READWRITE;
    lpBuffer->Type = MEM_IMAGE | MEM_MAPPED;
    
    assert(dwLength == sizeof(MEMORY_BASIC_INFORMATION));
    return dwLength;
}

DEF_HOST_FUNC(winMem, VirtualQuery)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        void* arg1  = (void*) w32.next<WIN32_PTR>();
        auto arg2  = (PMEMORY_BASIC_INFORMATION) getMemAddr(w32.next<WIN32_PTR>());
        uint32_t arg3 = w32.next<uint32_t>();
        return (uint64_t)VirtualQuery(arg1, arg2, arg3);
    }
    else
    {
        
    }
    return 0;
}

void winMem::init_funcs()
{
    DEF_STD_USER_HOST_CALL(winMem, VirtualAlloc,16);
	DEF_STD_USER_HOST_CALL(winMem, VirtualProtect,16);
	DEF_STD_USER_HOST_CALL(winMem, VirtualUnlock,8);
	DEF_STD_USER_HOST_CALL(winMem, VirtualFree,12);
    DEF_STD_USER_HOST_CALL(winMem, VirtualQuery,12);
}
void init_caller_winMem()
{
    static winMem win_mem;

}



