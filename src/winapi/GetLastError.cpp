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

STATIC DWORD LastError;

STATIC DWORD  GetLastError(void)
{
    LOG_INFO("GetLastError() => %#x", LastError);

    return LastError;
}

void  SetLastError(DWORD dwErrCode)
{
    //DebugLog("SetLastError(%#x)", dwErrCode);
    LastError = dwErrCode;
}

#undef DECLARE_CRT_EXPORT
#define DECLARE_CRT_EXPORT(name,_func) uint64_t wrap_##_func(uint64_t* args)

DECLARE_CRT_EXPORT("GetLastError", GetLastError)
{
    return GetLastError();
}
DECLARE_CRT_EXPORT("SetLastError", SetLastError)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        uint32_t err = w32_args.next<uint32_t>();
        SetLastError(err);
    }
    else
    {
        uint32_t err = (uint32_t)args[0];
        SetLastError(err);
    }
    return 0;
}

#undef DECLARE_CRT_EXPORT
