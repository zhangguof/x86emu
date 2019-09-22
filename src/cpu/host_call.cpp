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
#include "winapi/winapi.hpp"
#include "logger.hpp"
#include <unordered_map>



//typedef int (*HFun_i_s)(const char*);
//do host call ,call from guess








HOST_FUN_C host_func_table[] = {
#define DEF_HOST_FUNC(func,idx) \
{\
#func,wrap_##func,idx \
},
    
#define DEF_HOST_STD_FUNC(func,idx,size) DEF_HOST_FUNC(func,idx)
    
#include "host_call.hpp"
#include "gen_code/wrap_gen_code.h"
#include "winapi/wrap_winapi.h"
    
#undef DEF_HOST_FUNC
#undef DEF_HOST_STD_FUNC

};

std::unordered_map<uint32_t,HOST_FUN_C*> host_call_hash_tbl;
//typedef std::unordered_map<uint32_t, HOST_FUN_C*> HostCallTbl_t;
//HostCallTbl_t* p_user_host_call_tbl = nullptr;

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

