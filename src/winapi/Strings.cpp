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

class winStrnigs:public HostCallerBase
{
#undef DECLARE_CRT_EXPORT
#define DECLARE_CRT_EXPORT(name,func)\
static uint64_t wrap_##func(uint64_t* args);
    DECLARE_CRT_EXPORT("MultiByteToWideChar", MultiByteToWideChar);
    DECLARE_CRT_EXPORT("WideCharToMultiByte", WideCharToMultiByte);
    DECLARE_CRT_EXPORT("GetStringTypeA", GetStringTypeA);
    DECLARE_CRT_EXPORT("GetStringTypeW", GetStringTypeW);
    DECLARE_CRT_EXPORT("RtlInitUnicodeString", RtlInitUnicodeString);
    DECLARE_CRT_EXPORT("UuidFromStringW", UuidFromStringW);
    DECLARE_CRT_EXPORT("UuidCreate", UuidCreate);

    
#undef DECLARE_CRT_EXPORT
    virtual void init_funcs();
    
};

#define DEF_HOST_FUNC(cls,func) \
uint64_t cls::wrap_##func(uint64_t* args)

void winStrnigs::init_funcs()
{
    DEF_STD_USER_HOST_CALL(winStrnigs, MultiByteToWideChar,24);
	DEF_STD_USER_HOST_CALL(winStrnigs, WideCharToMultiByte,32);
	DEF_STD_USER_HOST_CALL(winStrnigs, GetStringTypeA,20);
	DEF_STD_USER_HOST_CALL(winStrnigs, GetStringTypeW,16);
	DEF_STD_USER_HOST_CALL(winStrnigs, RtlInitUnicodeString,8);
	DEF_STD_USER_HOST_CALL(winStrnigs, UuidFromStringW,8);
	DEF_STD_USER_HOST_CALL(winStrnigs, UuidCreate,4);
}

#define MB_ERR_INVALID_CHARS 8
#define MB_PRECOMPOSED 1

STATIC int WINAPI MultiByteToWideChar(UINT CodePage, DWORD  dwFlags, PCHAR lpMultiByteStr, int cbMultiByte, PUSHORT lpWideCharStr, int cchWideChar)
{
    size_t i;

    LOG_DEBUG("%u, %#x, %p, %u, %p, %u", CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);

    if ((dwFlags & ~(MB_ERR_INVALID_CHARS | MB_PRECOMPOSED)) != 0) {
        LOG_DEBUG("Unsupported Conversion Flags %#x", dwFlags);
    }

    if (CodePage != 0 && CodePage != 65001) {
        LOG_DEBUG("Unsupported CodePage %u", CodePage);
    }

    if (cbMultiByte == 0)
        return 0;

    if (cbMultiByte == -1)
        cbMultiByte = strlen(lpMultiByteStr) + 1;

    if (cchWideChar == 0)
        return cbMultiByte;

    // cbMultibyte is the number of *bytes* to process.
    // cchWideChar is the number of output *chars* expected.
    if (cbMultiByte > cchWideChar) {
        return 0;
    }

    for (i = 0; i < cbMultiByte; i++) {
        lpWideCharStr[i] = (uint8_t) lpMultiByteStr[i];
        if (dwFlags & MB_ERR_INVALID_CHARS) {
            if (!isascii(lpMultiByteStr[i]) || iscntrl(lpMultiByteStr[i])) {
                lpWideCharStr[i] = '?';
            }
        }
    }

    return i;
}
//STATIC int WINAPI MultiByteToWideChar(UINT CodePage, DWORD  dwFlags, PCHAR lpMultiByteStr, int cbMultiByte, PUSHORT lpWideCharStr, int cchWideChar)
DEF_HOST_FUNC(winStrnigs, MultiByteToWideChar)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        typedef uint32_t T1;
        typedef uint32_t T2;
        typedef char* T3;
        typedef int T4;
        typedef uint16_t* T5;
        typedef int T6;
        auto arg1 = (T1) w32.next<T1>();
        auto arg2 = (T2) w32.next<T2>();
        auto arg3 = (T3) getMemAddr(w32.next<WIN32_PTR>());
        auto arg4 = (T4) w32.next<T4>();
        auto arg5 = (T5) getMemAddr(w32.next<WIN32_PTR>());
        auto arg6 = (T6) w32.next<T6>();
        return MultiByteToWideChar(arg1,arg2,arg3,
                                   arg4,arg5,arg6);
        
        
    }
    else
    {
        
    }
    return 0;
}

STATIC int WINAPI WideCharToMultiByte(UINT CodePage, DWORD dwFlags, PVOID lpWideCharStr, int cchWideChar, PVOID lpMultiByteStr, int cbMultiByte, PVOID lpDefaultChar, PVOID lpUsedDefaultChar)
{
    char *ansi = NULL;

    LOG_DEBUG("%u, %#x, %p, %d, %p, %d, %p, %p", CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);

    if (cchWideChar != -1) {
        // Add a nul terminator.
        PVOID tmpStr = calloc(cchWideChar + 1, sizeof(USHORT));
        memcpy(tmpStr, lpWideCharStr, cchWideChar);
        ansi = CreateAnsiFromWide(tmpStr);
        free(tmpStr);
    } else {
        ansi = CreateAnsiFromWide(lpWideCharStr);
    }

    // This really can happen
    if (ansi == NULL) {
        return 0;
    }

    LOG_DEBUG("cchWideChar == %d, Ansi: [%s]", cchWideChar, ansi);

    if (lpMultiByteStr && strlen(ansi) < cbMultiByte) {
        strcpy((char*)lpMultiByteStr, ansi);
        free(ansi);
        return strlen((char*)lpMultiByteStr) + 1;
    } else if (!lpMultiByteStr && cbMultiByte == 0) {
        int len = strlen(ansi) + 1;
        free(ansi);
        return len;
    }

    free(ansi);
    return 0;
}
//STATIC int WINAPI WideCharToMultiByte(UINT CodePage, DWORD dwFlags, PVOID lpWideCharStr, int cchWideChar, PVOID lpMultiByteStr, int cbMultiByte, PVOID lpDefaultChar, PVOID lpUsedDefaultChar)

DEF_HOST_FUNC(winStrnigs, WideCharToMultiByte)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        typedef uint32_t T1;
        typedef uint32_t T2;
        typedef void* T3;
        typedef int T4;
        typedef void* T5;
        typedef int T6;
        typedef void* T7;
        typedef void* T8;
        auto arg1 = (T1) w32.next<T1>();
        auto arg2 = (T2) w32.next<T2>();
        auto arg3 = (T3) getMemAddr(w32.next<WIN32_PTR>());
        auto arg4 = (T4) w32.next<T4>();
        auto arg5 = (T5) getMemAddr(w32.next<WIN32_PTR>());
        auto arg6 = (T6) w32.next<T6>();
        auto arg7 = (T7) w32.next<WIN32_PTR>();
        auto arg8 = (T8) w32.next<WIN32_PTR>();
        return WideCharToMultiByte(arg1,arg2,arg3,
                                   arg4,arg5,arg6,arg7,arg8);
        
        
    }
    else
    {
        
    }
    return 0;
}


STATIC BOOL WINAPI GetStringTypeA(DWORD locale, DWORD dwInfoType, PUSHORT lpSrcStr, int cchSrc, PUSHORT lpCharType)
{
    LOG_DEBUG("%u, %u, %p, %d, %p", locale, dwInfoType, lpSrcStr, cchSrc, lpCharType);

    memset(lpCharType, 1, cchSrc * sizeof(USHORT));

    return FALSE;
}

DEF_HOST_FUNC(winStrnigs, GetStringTypeA)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        typedef uint32_t T1;
        typedef uint32_t T2;
        typedef uint16_t* T3;
        typedef int T4;
        typedef uint16_t* T5;

        auto arg1 = (T1) w32.next<T1>();
        auto arg2 = (T2) w32.next<T2>();
        auto arg3 = (T3) getMemAddr(w32.next<WIN32_PTR>());
        auto arg4 = (T4) w32.next<T4>();
        auto arg5 = (T5) getMemAddr(w32.next<WIN32_PTR>());

        return GetStringTypeA(arg1,arg2,arg3,
                                   arg4,arg5);
        
        
    }
    else
    {
        
    }
    return 0;
}


STATIC BOOL WINAPI GetStringTypeW(DWORD dwInfoType, PUSHORT lpSrcStr, int cchSrc, PUSHORT lpCharType)
{
    LOG_DEBUG("%u, %p, %d, %p", dwInfoType, lpSrcStr, cchSrc, lpCharType);

    memset(lpCharType, 1, cchSrc * sizeof(USHORT));

    return FALSE;
}

DEF_HOST_FUNC(winStrnigs, GetStringTypeW)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        typedef uint32_t T1;
        typedef uint16_t* T2;
        typedef int T3;
        typedef uint16_t* T4;
        
        auto arg1 = (T1) w32.next<T1>();
        auto arg2 = (T2) getMemAddr(w32.next<WIN32_PTR>());
        auto arg3 = (T3) w32.next<T3>();
        auto arg4 = (T4) getMemAddr(w32.next<WIN32_PTR>());
        
        return GetStringTypeW(arg1,arg2,arg3,arg4);
        
    }
    else
    {
        
    }
    return 0;
}

STATIC VOID WINAPI RtlInitUnicodeString(PUNICODE_STRING DestinationString, PWCHAR SourceString,uint32_t guest_src_addr)
{
    DestinationString->Length = CountWideChars(SourceString) * 2;
    DestinationString->MaximumLength = DestinationString->Length;
    DestinationString->Buffer = (PWCHAR)guest_src_addr;
}

DEF_HOST_FUNC(winStrnigs, RtlInitUnicodeString)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        typedef PUNICODE_STRING T1;
        typedef PWCHAR T2;
        auto arg1 = (T1) getMemAddr(w32.next<WIN32_PTR>());
        auto _arg2 = w32.next<WIN32_PTR>();
        auto arg2 = (T2) getMemAddr(_arg2);
        RtlInitUnicodeString(arg1, arg2, _arg2);
    }
    else
    {
        
    }
    return 0;
}

STATIC PVOID WINAPI UuidFromStringW(PUSHORT StringUuid, PBYTE Uuid)
{
    int i;

    LOG_DEBUG("%S, %p", StringUuid, Uuid);

    for (i = 0; i < 16; i++) {
        Uuid[i] = 0x41;
    }

    return 0;
}

DEF_HOST_FUNC(winStrnigs, UuidFromStringW)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        typedef PUSHORT T1;
        typedef PBYTE T2;
        auto arg1 = (T1) getMemAddr(w32.next<WIN32_PTR>());
        auto arg2 = (T2) getMemAddr(w32.next<WIN32_PTR>());
        return (uint64_t)UuidFromStringW(arg1, arg2);
    }
    else
    {
        
    }
    return 0;
}

STATIC INT WINAPI UuidCreate(PBYTE Uuid)
{
    int i;

    LOG_DEBUG("%p", Uuid);

    for (i = 0; i < 16; i++) {
        Uuid[i] = 0x41;
    }

    return 0;
}

DEF_HOST_FUNC(winStrnigs, UuidCreate)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        typedef PBYTE T1;
        auto arg1 = (T1) getMemAddr(w32.next<WIN32_PTR>());
        return (uint64_t)UuidCreate(arg1);
    }
    else
    {
        
    }
    return 0;
}


static winStrnigs win_str;
