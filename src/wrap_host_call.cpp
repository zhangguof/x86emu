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

inline Bit8u* getMemAddr(Bit64u addr)
{
    Bit8u* ret = BX_MEM(0)->getHostMemAddr(BX_CPU(0), addr, BX_RW);
    return ret;
}




inline uint64_t do_ret(int64_t ret_code)
{
    //    g_engine->last_ret = code;
    //    printf("return from guset code!:%ld\n",ret_code);
    g_engine->cpu_ptr->is_exit = true;
    g_engine->last_ret = ret_code;
    return 0;
}

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
    ARG1_T arg1 = (ARG1_T)(getMemAddr(args[0]));
    return puts(arg1);
}

DEF_HOST_FUNC(host_malloc)
{
    uint64_t size = args[0];
    return  (uint64_t)host_malloc(size);
}

DEF_HOST_FUNC(host_free)
{
    void* ptr = (void*)args[0];
    host_free(ptr);
    return 0;
}


#undef DEF_HOST_FUNC
