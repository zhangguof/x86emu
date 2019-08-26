//
//  host_call.cpp
//  x86emu
//
//  Created by tony on 2019/8/19.
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


//typedef int (*HFun_i_s)(const char*);
//do host call ,call from guess

typedef int (*HFun_is)(const char*);


struct HOST_FUN_C
{
    void* ptr;
    const char* name;
    std::function<Bit64u(Bit64u*)> pf;
    
//LP64
};

#define DEF_HOST_FUNC(func,f) \
{\
(void*)func,#func,f \
}


inline Bit8u* getMemAddr(Bit64u addr)
{
    Bit8u* ret = BX_MEM(0)->getHostMemAddr(BX_CPU(0), addr, BX_RW);
    return ret;
}

Bit64u do_ret(int64_t ret_code)
{
//    g_engine->last_ret = code;
    printf("return from guset code!:%ld\n",ret_code);
    g_engine->cpu_ptr->is_exit = true;
    g_engine->last_ret = ret_code;
    return 0;
}

HOST_FUN_C host_func_table[] = {
//    {nullptr,nullptr,nullptr},
    DEF_HOST_FUNC(do_ret, [](Bit64u* args){
        int64_t arg1 = (int64_t)args[0];
        return do_ret(arg1);
    }),
//int puts(const char*)
    DEF_HOST_FUNC(puts,[](Bit64u* args){
        typedef const char* ARG1_T;
        ARG1_T arg1 = (ARG1_T)(getMemAddr(args[0]));
        return puts(arg1);}
                  ),
    
};



Bit64u do_call_host_func(Bit32u idx,HOST_CALL_5ARGS& args)
{
    auto fe = host_func_table[idx];
//    static_cast<HFun_i_s>(ptr)(paddr);
//    ((HFun_is)f.ptr)((const char*)paddr);
    return fe.pf(args);
}

