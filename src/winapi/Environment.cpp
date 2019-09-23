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

class winEnv:public HostCallerBase
{
#undef DECLARE_CRT_EXPORT
#define DECLARE_CRT_EXPORT(name,func)\
static uint64_t wrap_##func(uint64_t* args);
    DECLARE_CRT_EXPORT("GetEnvironmentStringsW", GetEnvironmentStringsW);
    DECLARE_CRT_EXPORT("FreeEnvironmentStringsW", FreeEnvironmentStringsW);
    DECLARE_CRT_EXPORT("GetEnvironmentVariableW", GetEnvironmentVariableW);
    DECLARE_CRT_EXPORT("ExpandEnvironmentStringsW", ExpandEnvironmentStringsW);
    DECLARE_CRT_EXPORT("GetEnvironmentVariableA", GetEnvironmentVariableA);
    
#undef DECLARE_CRT_EXPORT
    virtual void init_funcs();
    
};

#define DEF_HOST_FUNC(cls,func) \
uint64_t cls::wrap_##func(uint64_t* args)

#define ERROR_ENVVAR_NOT_FOUND 203

extern void WINAPI SetLastError(DWORD dwErrCode);

char EnvironmentStrings[] =
    "ALLUSERSPROFILE=AllUsersProfile\0"
    "ALLUSERSAPPDATA=AllUsersAppdata\0"
;

STATIC PVOID WINAPI GetEnvironmentStringsW(void)
{
    LOG_DEBUG("");

    return EnvironmentStrings;
}

STATIC BOOL WINAPI FreeEnvironmentStringsW(PVOID lpszEnvironmentBlock)
{
    LOG_DEBUG("%p", lpszEnvironmentBlock);

    return TRUE;
}

STATIC DWORD WINAPI GetEnvironmentVariableW(PWCHAR lpName, PVOID lpBuffer, DWORD nSize)
{
    char *AnsiName = CreateAnsiFromWide(lpName);

    LOG_DEBUG("%p [%s], %p, %u", lpName, AnsiName, lpBuffer, nSize);

    memset(lpBuffer, 0, nSize);

    if (strcmp(AnsiName, "MpAsyncWorkMaxThreads") == 0) {
        memcpy(lpBuffer, L"1", sizeof(L"1"));
    } else if (strcmp(AnsiName, "MP_FOLDERSCAN_THREAD_COUNT") == 0) {
        memcpy(lpBuffer, L"1", sizeof(L"1"));
    } else if (strcmp(AnsiName, "MP_PERSISTEDSTORE_DISABLE") == 0) {
        memcpy(lpBuffer, L"1", sizeof(L"1"));
    } else if (strcmp(AnsiName, "MP_METASTORE_DISABLE") == 0) {
        memcpy(lpBuffer, L"1", sizeof(L"1"));
    } else {
        SetLastError(ERROR_ENVVAR_NOT_FOUND);
    }

    free(AnsiName);
    return CountWideChars(lpBuffer);
}

// MPENGINE is very fussy about what ExpandEnvironmentStringsW returns.
STATIC DWORD WINAPI ExpandEnvironmentStringsW(PWCHAR lpSrc, PWCHAR lpDst, DWORD nSize)
{
    PCHAR AnsiString = CreateAnsiFromWide(lpSrc);
    DWORD Result;
    struct {
        PCHAR   Src;
        PWCHAR  Dst;
    } KnownPaths[] = {
        { "%ProgramFiles%", L"C:\\Program Files" },
        { "%AllUsersProfile%", L"C:\\ProgramData" },
        { "%PATH%", L"C:\\Path" },
        { "%windir%", L"C:\\Windows" },
        { "%ProgramFiles(x86)%", L"C:\\Program Files" },
        { "%WINDIR%\\system32\\drivers", L"C:\\WINDOWS\\system32\\drivers" },
        { "%windir%\\temp", L"C:\\WINDOWS\\temp" },
        { "%CommonProgramFiles%", L"C:\\CommonProgramFiles" },
        { NULL },
    };

    LOG_DEBUG("%p [%s], %p, %u", lpSrc, AnsiString, lpDst, nSize);

    for (int i = 0; KnownPaths[i].Src; i++) {
        if (strcmp(AnsiString, KnownPaths[i].Src) == 0) {
            Result = CountWideChars(KnownPaths[i].Dst) + 1;
            if (nSize < Result) {
                goto finish;
            }
            memcpy(lpDst, KnownPaths[i].Dst, Result * 2);
            goto finish;
        }
    }

    free(AnsiString);

    if (nSize < CountWideChars(lpSrc) + 1) {
        return CountWideChars(lpSrc) + 1;
    }

    memcpy(lpDst, lpSrc, (1 + CountWideChars(lpSrc)) * 2);

    return CountWideChars(lpSrc) + 1;

finish:
    free(AnsiString);
    return Result;
}

static DWORD WINAPI GetEnvironmentVariableA(PCHAR lpName, PVOID lpBuffer, DWORD nSize)
{
    LOG_DEBUG("%s, %p, %u", lpName, lpBuffer, nSize);
    return 0;
}

void winEnv::init_funcs()
{
	DEF_STD_USER_HOST_CALL(winEnv, GetEnvironmentStringsW,0);
	DEF_STD_USER_HOST_CALL(winEnv, FreeEnvironmentStringsW,4);
	DEF_STD_USER_HOST_CALL(winEnv, GetEnvironmentVariableW,12);
	DEF_STD_USER_HOST_CALL(winEnv, ExpandEnvironmentStringsW,12);
	DEF_STD_USER_HOST_CALL(winEnv, GetEnvironmentVariableA,12);
}

static winEnv win_env;
