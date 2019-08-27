//
//  host_sys_call.cpp
//  x86emu
//
//  Created by tony on 2019/8/20.
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

#include "host_sys_call.hpp"

#include "logger.hpp"

typedef Bit64u (*sys_call_func)(Bit64u idx,SYS_CALL_ARGS args);

Bit64u default_sys_call(Bit64u idx,SYS_CALL_ARGS args)
{
    LOG_ERROR("unsupport sys call(%0x)\n",idx);
    return -1;
}


struct SYS_CALL_TABLE
{
    Bit64u idx;
    sys_call_func ptr;
};
SYS_CALL_TABLE *sys_call_table = nullptr;

SYS_CALL_TABLE _call_table[] = {
#define DEF_SYSCALL(idx,name) \
    { idx, default_sys_call},
    
    #include "sys_call/linux_sys_call.hpp"

#undef DEF_SYSCALL
};


const Bit32u max_syscall_num = sizeof(_call_table) / sizeof(SYS_CALL_TABLE);

void init_sys_call()
{
    //over write default ptr
    sys_call_table = _call_table;
    
#define IMP_SYS_CALL(idx,name) \
    sys_call_table[idx].ptr = name;
    
#include "sys_call/linux_sys_call_imp.hpp"
    
#undef IMP_SYS_CALL
    

    
//    sys_call_table = call_table;
//    max_syscall_num  = sizeof(call_table)/sizeof(SYS_CALL_TABLE);
    
    
}


//imp linux sys_call
//ref:http://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/
//pass args regs:
//%rdi    %rsi    %rdx    %r10    %r8    %r9
Bit64u host_sys_call()
{
    SYS_CALL_ARGS args = {
      RDI,RSI,RDX,R10,R8,R9
    };
    return do_host_sys_call(args);
}

Bit64u do_host_sys_call(SYS_CALL_ARGS args)
{
    Bit64u sys_call_id = RAX;
    if(sys_call_table == nullptr)
    {
        init_sys_call();
    }
    if(sys_call_id > max_syscall_num)
    {
        LOG_ERROR("try syscall(%0x) to larger!\n",sys_call_id);
        return -1;
    }
    auto fn = sys_call_table[sys_call_id].ptr;
    return fn(sys_call_id,args);
}
