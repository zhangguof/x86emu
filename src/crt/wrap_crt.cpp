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

//#include <stdarg.h>
#include "utils.h"


//#undef va_arg
//#undef va_start
//#undef va_end
//
//struct VA_LIST
//{
//    char* ap;
//};

//typedef char* va_list_t;
//inline void va_start(VA_LIST*  va,void* argn)
//{
//    if(is_cpu_mode32())
//        va->ap = (char*)argn + 4;
//    else
//        va->ap = (char*)argn + 8;
//}
//inline vs_start(VS_LIST ap,)
//#define va_start64(ap,argn) ap=((char*)&argn)+8
//#define va_start32(ap,argn) ap=((char*)&argn)+4
//#define va_arg(ap,type) (ap+=sizeof(type), *(type*)((void*)ap-sizeof(type)))

//
//#define va_end(ap) ((void)0)
//
//template<typename T>
//class va_arg
//{
//public:
//    static T get(VA_LIST* va)
//    {
//        va->ap += sizeof(T);
//        return *(T*)(va->ap - sizeof(T));
//    }
//};

//template<typename T>
//class va_arg<T*>
//{
//public:
//    static T* get(VA_LIST* va)
//    {
////        if(is_cpu_mode32())
//        if(0)
//        {
//            va->ap += sizeof(WIN32_PTR);
//            return (T*)*(WIN32_PTR*)(va->ap - sizeof(WIN32_PTR));
//        }
//        else
//        {
//            va->ap += sizeof(uint64_t);
//            return (T*)*(uint64_t*)(va->ap - sizeof(uint64_t));
//        }
//    }
//};

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

 int print_test(const char* fmt,...)
{
//    VA_LIST va;
//    auto pva = &va;
//    va_start(pva,&fmt);
//    auto arg1 = va_arg<int>::get(pva);
//    auto arg2 = va_arg<uint64_t>::get(pva);
//    auto arg3 = va_arg<const char*>::get(pva);
//    printf("get vars %d,0x%0llx,%s\n",arg1,arg2,arg3);
    char buf[1024];
    va_list va;
    va_start(va,fmt);
    vsprintf(buf, fmt, va);
//    auto arg1 = va_arg(va, int);
//    auto arg2 = va_arg(va, uint64_t);
//    auto arg3 = va_arg(va, const char*);
//    printf("get vars %d,0x%0llx,%s\n",arg1,arg2,arg3);
    puts(buf);
    
    return 0;
    
}

void test_vars()
{
    char buf[1024];
    char* pp = buf;
//    const char* name = "hhhhhh\n";
    const char* name2 = "ttttt";
//    char* p = buf;
//    *(uint64_t*)p = (uint64_t)&name;
//    p+=sizeof(uint64_t);
//    *(int*)p = (int)0x1234;
//    p+=sizeof(int);
//    *(uint64_t*)p =0x123456789ABCDEF;
//    p+=sizeof(uint64_t);
//    *(uint64_t*)p = (uint64_t)name2;
    
    print_test("%0x,0x%0llx,%s",0x1234,0x123456789ABCDEF,name2);
}



