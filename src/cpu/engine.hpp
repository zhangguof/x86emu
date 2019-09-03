//
//  engine.hpp
//  x86emu
//
//  Created by tony on 2019/8/19.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef engine_hpp
#define engine_hpp

#include <stdio.h>
#include "buffer.hpp"

struct export_funcs
{
    void* ptr;
    char name[64];
};




class Engine
{
public:
    XE_CPU_C* cpu_ptr;
    XE_MEM_C* mem_ptr;
    bx_phy_address entry_addr;
    bx_phy_address call_host_ret_addr;
    struct export_funcs* call_guess_method;
    uint64_t last_ret;
    Engine(){
        cpu_ptr = nullptr;
        mem_ptr = nullptr;
    }
    void load_elf(const char* path);
    void setup_os_env();
    void init();
    Bit8u* getHostMemAddr(bx_phy_address addr){
        return mem_ptr->getHostMemAddr(cpu_ptr, addr, BX_RW);
    }
    
    void run();
    void call_guest_method1(const char* method,uint64_t arg1);
    void call_win_guest_method1(const char* method,uint64_t arg1);
};

extern std::shared_ptr<Engine> g_engine;

#endif /* engine_hpp */
