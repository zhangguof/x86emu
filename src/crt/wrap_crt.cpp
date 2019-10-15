//
//  wrap_crt.cpp
//  x86emu
//
//  Created by tony on 2019/9/4.
//  Copyright © 2019 tony. All rights reserved.
//
#define NEED_CPU_REG_SHORTCUTS 1
#include "cpu/x86.h"
#include "param_names.h"
#include "elf.h"
#include "mmu.hpp"

#include <functional>
#include "engine.hpp"

#include "crt/wrap_crt.hpp"
#include "wrap_host_call.hpp"
#include "logger.hpp"
//#include <stdarg.h>
#include "utils.h"
#include <locale.h>

#define DEF_HOST_FUNC(func) \
uint64_t wrap_##func(uint64_t* args)


//int win_vsnprintf(char* str, size_t size, const char *format,struct va_args* arg_ptr)
//int snprintf(char* str,size_t size,const char* fmt,...);
uint64_t wrap_snprintf(uint64_t* args)
{
    char* str;
    size_t size;
    const char* fmt;
    if(is_cpu_mode32())
    {
        WIN32_ARGS win32_args = {args};
        str = (char*)getMemAddr(win32_args.next<WIN32_PTR>());
        size = (win32_args.next<uint32_t>());
        fmt = (char*)getMemAddr(win32_args.next<WIN32_PTR>());
        struct va_args va = {(uint8_t*)win32_args.ptr};
        return win_vsnprintf(str, size, fmt, &va);
    }
    else
    {
        str = (char*)getMemAddr(args[0]);
        size = args[1];
        fmt = (char*)getMemAddr(args[2]);
        struct va_args va = {(uint8_t*)(args[3])};
        return win_vsnprintf(str, size, fmt, &va);
    }
}

//int win_vsprintf(char *dest,const char *format, va_list arg_ptr)
//int sprintf(char* str,const char* fmt,...);
uint64_t wrap_sprintf(uint64_t* args)
{
    char* dest;
    const char* fmt;
    if(is_cpu_mode32())
    {
        WIN32_ARGS win32_args = {args};
        dest = (char*)getMemAddr(win32_args.next<WIN32_PTR>());
        fmt = (char*)getMemAddr(win32_args.next<WIN32_PTR>());
        struct va_args va = {(uint8_t*)win32_args.ptr};
        return win_vsprintf(dest,fmt,&va);
    }
    else
    {
        dest = (char*)getMemAddr(args[0]);
        fmt = (char*)getMemAddr(args[1]);
        struct va_args va = {(uint8_t*)(args[2])};
        return win_vsprintf(dest,fmt, &va);
    }
}
const int BUFF_SIZE = 2048;
//int printf(const char* fmt,arg1,arg2,)
uint64_t wrap_printf(uint64_t* args)
{
    const char* fmt;
//    char buf[BUFF_SIZE];
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        fmt = (char*)getMemAddr(w32_args.next<WIN32_PTR>());
        va_args ap = {(uint8_t*)w32_args.ptr};
        return win_vprintf(fmt, &ap);
    }
    else
    {
        fmt = (char*)getMemAddr(args[0]);
        va_args ap = {(uint8_t*)(&args[1])};
        return win_vprintf(fmt, &ap);
    }
}

//FILE* win_stdin;
//FILE* win_stdout;
//FILE* win_stderr;

#include <unordered_map>
std::unordered_map<uint32_t,FILE*> fstreams;



uint64_t wrap_init_stdio(uint64_t* args)
{
    
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        WIN32_PTR *s = ( WIN32_PTR *)getMemAddr(w32_args.next<WIN32_PTR>());
        fstreams[s[0]] = stdin;
        fstreams[s[1]] = stdout;
        fstreams[s[2]] = stderr;
    }
    else
    {
        bx_phy_address *s;
        s = (bx_phy_address*)getMemAddr(args[0]);
        fstreams[s[0]] = stdin;
        fstreams[s[1]] = stdout;
        fstreams[s[2]] = stderr;
    }
    return 0;
}
//std::FILE* fopen( const char* filename, const char* mode );
uint32_t win_fopen(const char* path,const char* mode)
{
    static uint32_t file_handle_used = 3;
    FILE* s = fopen(path, mode);
    if(s!=nullptr)
    {
        fstreams[file_handle_used] = s;
        return file_handle_used++;
    }
    return -1;
}

DEF_HOST_FUNC(fopen)
{
    const char* path;
    const char* mode;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        path = (const char*) getMemAddr((bx_phy_address)w32_args.next<WIN32_PTR>());
        mode = (const char*) getMemAddr((bx_phy_address)w32_args.next<WIN32_PTR>());

    }
    else
    {
        path  = (const char*) getMemAddr((bx_phy_address)args[0]);
        mode  = (const char*) getMemAddr((bx_phy_address)args[1]);

    }
    return  win_fopen(path, mode);
}

FILE* get_file_stream(uint32_t handle)
{
    auto it = fstreams.find(handle);
    if(it==fstreams.end())
    {
        LOG_ERROR("unknow FILE*:0x%0x",handle);
        return nullptr;
    }
    return it->second;
}

//int fclose( std::FILE* stream );
uint32_t win_fclose(uint32_t handle)
{
    auto it = fstreams.find(handle);
    if(it==fstreams.end()){
        LOG_ERROR("fclose:unknow FILE*:0x%0x",handle);
        return -1;
    }
    FILE* s = it->second;
    auto ret = fclose(s);
    fstreams.erase(it);
    return ret;
}

DEF_HOST_FUNC(fclose)
{
    uint32_t handle;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        handle = w32_args.next<uint32_t>();
    }
    else
    {
        handle = args[0];
    }
    return win_fclose(handle);
}

int win_vfprintf(uint32_t handle,const char* format,va_args* ap)
{
    char buf[BUFF_SIZE];
    FILE* s = get_file_stream(handle);
    if(!s) return 0;
    int size  = win_vsprintf(buf, format, ap);
    int ret = fwrite(buf, sizeof(char), size, s);
    return ret;
}

//int fprintf( std::FILE* stream, const char* format, ... );
DEF_HOST_FUNC(fprintf)
{
    const char* fmt = nullptr;
//    FILE* s = nullptr;
    uint32_t handle = -1;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
//        handle = (uint32_t) getMemAddr(w32_args.next<WIN32_PTR>());
        handle = w32_args.next<uint32_t>();
        fmt = (char*)getMemAddr(w32_args.next<WIN32_PTR>());
        va_args ap = {(uint8_t*)w32_args.ptr};
        return win_vfprintf(handle,fmt, &ap);
        
    }
    else
    {
//        s = (FILE*)getMemAddr(args[0]);
        handle = (uint32_t) args[0];
        fmt = (const char*)getMemAddr(args[1]);
        va_args ap = {(uint8_t*)(&args[2])};
        return win_vfprintf(handle, fmt, &ap);
    }
}

int win_fflush(uint32_t handle)
{
    auto s = get_file_stream(handle);
    if(!s) return -1;
    
    return fflush(s);
}

//int fflush( FILE *stream );
DEF_HOST_FUNC(fflush)
{
    uint32_t handle;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        handle = w32_args.next<uint32_t>();
    }
    else
    {
        handle = args[0];
    }
    return win_fflush(handle);
}
//void abort(void);
DEF_HOST_FUNC(abort)
{
    abort();
    return 0;
}

//size_t fwrite( const void *buffer, size_t size, size_t count,
//FILE *stream );
size_t win_fwrite( const void *buffer, size_t size, size_t count,
              uint32_t handle )
{
    auto s = get_file_stream(handle);
    if(!s) return 0;
    return fwrite(buffer, size, count, s);
}
//
//size_t fread( void          *buffer, size_t size, size_t count,
//             FILE          *stream );

size_t win_fread( void *buffer, size_t size, size_t count, uint32_t handle)
{
    auto s = get_file_stream(handle);
    if(!s) return 0;
    return fread(buffer, size, count, s);
}


DEF_HOST_FUNC(fwrite)
{
    void *buffer;
    size_t size;
    size_t count;
    uint32_t handle = -1;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        buffer = (void*)getMemAddr((bx_phy_address)w32_args.next<WIN32_PTR>());
        size = w32_args.next<uint32_t>();
        count = w32_args.next<uint32_t>();
        handle = w32_args.next<uint32_t>();
    }
    else
    {
        buffer = (void*)getMemAddr((bx_phy_address)args[0]);
        size = args[1];
        count = args[2];
        handle = args[3];
    }
    return win_fwrite(buffer, size, count, handle);
}

DEF_HOST_FUNC(fread)
{
    void *buffer;
    size_t size;
    size_t count;
    uint32_t handle = -1;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        buffer = (void*)getMemAddr((bx_phy_address)w32_args.next<WIN32_PTR>());
        size = w32_args.next<uint32_t>();
        count = w32_args.next<uint32_t>();
        handle = w32_args.next<uint32_t>();
    }
    else
    {
        buffer = (void*)getMemAddr((bx_phy_address)args[0]);
        size = args[1];
        count = args[2];
        handle = args[3];
    }
    return win_fread(buffer, size, count, handle);
}

//int     fileno(FILE *);
static int win_fileno(uint32_t handle)
{
    auto s = get_file_stream(handle);
    if(!s) return 0;
    return fileno(s);
}

DEF_HOST_FUNC(fileno)
{
    uint32_t handle;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        handle = w32.next<uint32_t>();
    }
    else
    {
        handle = (uint32_t)args[0];
    }
    return win_fileno(handle);
}
//__attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) __acrt_iob_func(unsigned index);

uint64_t win___acrt_iob_func(uint32_t idx)
{
    printf("win___acrt_iob_func:%d\n",idx);
    return idx;
}

DEF_HOST_FUNC(__acrt_iob_func)
{
    uint32_t idx;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        idx  = w32.next<uint32_t>();
        
    }
    else
    {
        idx = (uint32_t)args[0];
    }
    return win___acrt_iob_func(idx);
}



// using cls host call func!!
#undef DEF_HOST_FUNC

#define DEF_HOST_FUNC(cls,func) \
uint64_t cls::wrap_##func(uint64_t* args)

#define DEC_HOST_FUNC(func) \
uint64_t wrap_##func(uint64_t* args);



class WrapCRT:public HostCallerBase
{
    virtual void init_funcs();
    static DEC_HOST_FUNC(cls_test_func);
    static DEC_HOST_FUNC(fputc);
    static DEC_HOST_FUNC(_invalid_parameter);
    static DEC_HOST_FUNC(setlocale);
    static DEC_HOST_FUNC(putc);
    static DEC_HOST_FUNC(fputs);
    static DEC_HOST_FUNC(vprintf);
    static DEC_HOST_FUNC(vfprintf);

};

void WrapCRT::init_funcs()
{
    DEF_USER_HOST_CALL(WrapCRT,cls_test_func);
    DEF_USER_HOST_CALL(WrapCRT, fputc);
    DEF_USER_HOST_CALL(WrapCRT, _invalid_parameter);
    DEF_USER_HOST_CALL(WrapCRT, setlocale);
    DEF_USER_HOST_CALL(WrapCRT, putc);
    DEF_USER_HOST_CALL(WrapCRT, fputs);
    DEF_USER_HOST_CALL(WrapCRT, vprintf);
    DEF_USER_HOST_CALL(WrapCRT, vfprintf);
}
//int     fputs(const char * __restrict, FILE * __restrict) __DARWIN_ALIAS(fputs);
static int win_fputs(const char* str, uint32_t handle)
{
    auto fs = get_file_stream(handle);
    if(!fs) return EOF;
    return fputs(str, fs);
}
DEF_HOST_FUNC(WrapCRT, fputs)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        typedef const char* T1;
        typedef uint32_t T2;
        auto arg1 = (T1) getMemAddr(w32.next<WIN32_PTR>());
        auto arg2 = (T2) w32.next<T2>();
        return win_fputs(arg1, arg2);
    }
    return 0;
}

DEF_HOST_FUNC(WrapCRT, vprintf)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        typedef char* T1;
        typedef uint8_t* T2;
        auto arg1 = (T1) getMemAddr(w32.next<WIN32_PTR>());
        auto arg2 = (T2) getMemAddr(w32.next<WIN32_PTR>());
//        return w32_vprintf(arg1, arg2);
        struct va_args arg_ptr = {arg2};
        return win_vprintf(arg1, &arg_ptr);
        
    }
    return 0;
}

DEF_HOST_FUNC(WrapCRT, vfprintf)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        typedef uint32_t T1;
        typedef const char* T2;
        typedef uint8_t* T3;
        auto arg1 = (T1) w32.next<T1>();
        auto arg2 = (T2) getMemAddr(w32.next<WIN32_PTR>());
        auto arg3 = (T3) getMemAddr(w32.next<WIN32_PTR>());
        struct va_args arg_ptr = {arg3};
        return win_vfprintf(arg1,arg2, &arg_ptr);
    }
    return 0;
    
}

DEF_HOST_FUNC(WrapCRT, putc)
{
    return wrap_fputc(args);
}


DEF_HOST_FUNC(WrapCRT,cls_test_func)
{
    printf("WrapCRT cls_test_func!\n");
    return 0;
}

static int win_fputc( int ch, uint32_t handle )
{
    auto s = get_file_stream(handle);
    if(!s) return EOF;
    return fputc(ch, s);
}

DEF_HOST_FUNC(WrapCRT,fputc)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        int arg1 = w32.next<int>();
        uint32_t arg2 = w32.next<uint32_t>();
        return win_fputc(arg1, arg2);
    }
    else
    {
        
    }
    return 0;
}

void win__invalid_parameter(
                            uint16_t const* const expression,
                            uint16_t const* const function_name,
                            uint16_t const* const file_name,
                            unsigned int   const line_number,
                            uintptr_t      const reserved)
{
    ;
}

DEF_HOST_FUNC(WrapCRT,_invalid_parameter)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        typedef uint16_t* T1;
        typedef uint16_t* T2;
        typedef uint16_t* T3;
        typedef uint32_t T4;
        typedef uint32_t T5;
        auto arg1 = (T1)getMemAddr(w32.next<WIN32_PTR>());
        auto arg2 = (T2)getMemAddr(w32.next<WIN32_PTR>());
        auto arg3 = (T3)getMemAddr(w32.next<WIN32_PTR>());
        auto arg4 = (T4)w32.next<T4>();
        auto arg5 = (T5)w32.next<T5>();
        win__invalid_parameter(arg1, arg2, arg3, arg4, arg5);
        
    }
    return 0;
}

static char* win_setlocale (int category, const char* locale)
{
//    return 0;
    auto ret = setlocale(category,locale);
    size_t len = strlen(ret);
    auto g_ret = (char*)host_malloc(len);
    host_memcpy(g_ret, ret, len);
    return (char*)g_ret;
//    return ret;
}

DEF_HOST_FUNC(WrapCRT,setlocale)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        typedef int T1;
        typedef char* T2;

        auto arg1 = w32.next<T1>();
        auto arg2 = (T2)getMemAddr(w32.next<WIN32_PTR>());
        
        return (uint64_t)win_setlocale(arg1, arg2);
    }
    return 0;
}

void init_caller_WrapCRT()
{
    static WrapCRT wrap_crt;
}



