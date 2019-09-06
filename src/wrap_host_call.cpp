//
//  wrap_host_call.cpp
//  x86emu
//
//  Created by tony on 2019/9/4.
//  Copyright © 2019 tony. All rights reserved.
//
#define NEED_CPU_REG_SHORTCUTS 1
#include "cpu/x86.h"
#include "param_names.h"
#include "elf.h"
#include <stdio.h>
#include "mmu.hpp"

#include <string.h>
#include <functional>
#include "engine.hpp"

#include "wrap_host_call.hpp"






inline uint64_t do_ret(int64_t ret_code)
{
    //    g_engine->last_ret = code;
    //    printf("return from guset code!:%ld\n",ret_code);
    g_engine->cpu_ptr->is_exit = true;
    g_engine->last_ret = ret_code;
    return 0;
}

#define GET_ARGS(id,type) \
(type)args[id]

#define DEF_HOST_FUNC(func) \
uint64_t wrap_##func(uint64_t* args)

DEF_HOST_FUNC(do_ret)
{
    int64_t arg1 = (int64_t)args[0];
    return do_ret(arg1);
}

DEF_HOST_FUNC(puts)
{
    typedef const char* ARG1_T;
    uint32_t* pargs = (uint32_t*) args;
    ARG1_T arg1 = (ARG1_T)(getMemAddr(pargs[0]));
    return puts(arg1);
}

DEF_HOST_FUNC(malloc)
{
    uint64_t size = args[0];
    return  (uint64_t)host_malloc(size);
}

DEF_HOST_FUNC(free)
{
    void* ptr = (void*)args[0];
    host_free(ptr);
    return 0;
}
//
//DEF_HOST_FUNC(strlen)
//{
//    char* ptr = (char*)getMemAddr(args[0]);
//    return strlen(ptr);
//}
//DEF_HOST_FUNC(memcpy)
//{
//    void* arg1 = (void*)(getMemAddr(args[0]));
//    void* arg2 = (void*)(getMemAddr(args[1]));
//    size_t arg3 = (size_t)(args[2]);
//    memcpy(arg1, arg2, arg3);
//    return args[0];
//}
//DEF_HOST_FUNC(strcpy)
//{
//    char* arg1 = (char*)getMemAddr(args[0]);
//    char* arg2 = (char*)getMemAddr(args[1]);
//    strcpy(arg1, arg2);
//    return args[0];
//}
//
//DEF_HOST_FUNC(strstr)
//{
////    char    *strstr(const char *__big, const char *__little);
//    typedef const char* arg1_t;
//    typedef arg1_t arg2_t;
//    typedef arg1_t ret_t;
//    auto arg1 = GET_ARGS(0, arg1_t);
//    auto arg2 = GET_ARGS(1, arg2_t);
//    arg1 = (arg1_t)getMemAddr((bx_phy_address)arg1);
//    arg2 = (arg2_t)getMemAddr((bx_phy_address)arg2);
//
//    ret_t ret = strstr(arg1,arg2);
//    ret = (ret_t)getGuestAddr(bx_phy_address(ret));
//
//    return (uint64_t)ret;
//
//}

//DEF_HOST_FUNC(vprintf)
//{
//    char* arg1 = (char*)getMemAddr(args[0]);
//    void* va = (void*)getMemAddr(args[1]);
//    return vprintf(arg1, reinterpret_cast<va_list>(va));
//}

//int vprintf(const char * , va_list)
//uint64_t wrap_vprintf(uint64_t* args)
//{
//    char* fmt = (char*)getMemAddr(args[0]);
//    va_list va = (va_list)(args[1]);
//
//    return 0;
//}


#undef DEF_HOST_FUNC
