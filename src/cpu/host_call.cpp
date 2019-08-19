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
    return BX_MEM(0)->getHostMemAddr(BX_CPU(0), addr, BX_RW);
}

HOST_FUN_C host_func_table[] = {
    {nullptr,nullptr,nullptr},
//int puts(const char*)
    DEF_HOST_FUNC(puts,[](Bit64u* args){
        typedef const char* ARG1_T;
        ARG1_T arg1 = (ARG1_T)(getMemAddr(args[0]));
        return puts(arg1);}
                  ),
    
};



void do_call_host_func(Bit32u idx,HOST_CALL_5ARGS& args)
{
    auto fe = host_func_table[idx];
//    static_cast<HFun_i_s>(ptr)(paddr);
//    ((HFun_is)f.ptr)((const char*)paddr);
    fe.pf(args);
}

