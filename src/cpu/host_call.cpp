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
#include "logger.hpp"
#include <unordered_map>



//typedef int (*HFun_i_s)(const char*);
//do host call ,call from guess

//typedef int (*HFun_is)(const char*);
typedef  uint64_t (*host_fun_t)(uint64_t*);

struct HOST_FUN_C
{
//    void* ptr;
    const char* name;
//    std::function<uint64_t(uint64_t*)> pf;
    host_fun_t pf;
    uint32_t idx;
    
//LP64
};

//#define DEF_HOST_FUNC(func,f) \
//{\
//(void*)func,#func,f \
//}





HOST_FUN_C host_func_table[] = {
#define DEF_HOST_FUNC(func,idx) \
{\
#func,wrap_##func,idx \
},
    
#include "host_call.hpp"
#include "gen_code/wrap_gen_code.h"
    
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

std::unordered_map<uint32_t,HOST_FUN_C*> host_call_hash_tbl;

void init_host_call_tbl()
{
    host_call_hash_tbl.clear();
    size_t n_call = sizeof(host_func_table) / sizeof(HOST_FUN_C);
    for(size_t i=0;i<n_call;++i)
    {
        auto pt = host_func_table + i;
        host_call_hash_tbl[pt->idx] = pt;
    }
}


uint64_t do_call_host_func(Bit32u idx, uint64_t* args)
{
    auto it = host_call_hash_tbl.find(idx);
    if(it == host_call_hash_tbl.end())
    {
        LOG_ERROR("can't find host call(0x%0x)\n",idx);
        return -1;
    }
//    auto fe = it->second;
//    return fe.pf(args);
    return it->second->pf(args);
}

