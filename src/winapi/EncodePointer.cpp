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

class winEncodePointer:public HostCallerBase
{
#undef DECLARE_CRT_EXPORT
#define DECLARE_CRT_EXPORT(name,func)\
static uint64_t wrap_##func(uint64_t* args);
    DECLARE_CRT_EXPORT("EncodePointer", EncodePointer);
    DECLARE_CRT_EXPORT("DecodePointer", DecodePointer);
    
#undef DECLARE_CRT_EXPORT
    virtual void init_funcs();
    
};

#define DEF_HOST_FUNC(cls,func) \
uint64_t cls::wrap_##func(uint64_t* args)

void winEncodePointer::init_funcs()
{
    DEF_STD_USER_HOST_CALL(winEncodePointer, EncodePointer,4);
    DEF_STD_USER_HOST_CALL(winEncodePointer, DecodePointer,4);
}



STATIC uint32_t WINAPI EncodePointer(uint32_t Ptr)
{
    LOG_DEBUG("%p", Ptr);

    // Super secret high-security encryption algorithm.
    return (uint32_t)((DWORD)(Ptr) ^ ~0);
}

DEF_HOST_FUNC(winEncodePointer, EncodePointer)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        auto arg1 = w32.next<uint32_t>();
        return (uint64_t)EncodePointer(arg1);
    }
    else
    {
        
    }
    return 0;
}

STATIC uint32_t WINAPI DecodePointer(uint32_t Ptr)
{
    LOG_DEBUG("%p", Ptr);

    return (uint32_t)((DWORD)(Ptr) ^ ~0);
}

DEF_HOST_FUNC(winEncodePointer, DecodePointer)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        auto arg1 = w32.next<uint32_t>();
        return (uint64_t)DecodePointer(arg1);
    }
    else
    {
        
    }
    return 0;
}

void init_caller_winEncodePointer()
{
    static winEncodePointer win_ep;
}



