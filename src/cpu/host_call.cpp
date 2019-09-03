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
#include "wrap_host_call.hpp"


//typedef int (*HFun_i_s)(const char*);
//do host call ,call from guess

typedef int (*HFun_is)(const char*);


struct HOST_FUN_C
{
//    void* ptr;
    const char* name;
    std::function<uint64_t(uint64_t*)> pf;
    
//LP64
};

//#define DEF_HOST_FUNC(func,f) \
//{\
//(void*)func,#func,f \
//}





HOST_FUN_C host_func_table[] = {
#define DEF_HOST_FUNC(func) \
{\
#func,wrap_##func \
},
    
#include "host_call.hpp"
    
#undef DEF_HOST_FUNC
//    {nullptr,nullptr,nullptr},
//    DEF_HOST_FUNC(do_ret, [](Bit64u* args){
//        int64_t arg1 = (int64_t)args[0];
//        return do_ret(arg1);
//    }),
//    DEF_HOST_FUNC(puts,[](Bit64u* args){
//        typedef const char* ARG1_T;
//        ARG1_T arg1 = (ARG1_T)(getMemAddr(args[0]));
//        return puts(arg1);}
//                  ),
//    DEF_HOST_FUNC(host_malloc, [](Bit64u* args){
//        Bit64u size = args[0];
//        return  (Bit64u)host_malloc(size);
//    }),
//    DEF_HOST_FUNC(host_free, [](Bit64u* args){
//        void* ptr = (void*)args[0];
//        host_free(ptr);
//        return 0;
//    }),
    
};



Bit64u do_call_host_func(Bit32u idx,HOST_CALL_5ARGS& args)
{
    auto fe = host_func_table[idx];
    return fe.pf(args);
}

