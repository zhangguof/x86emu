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
#include "cpu/x86.h"
#include <vector>
#include <stdint.h>




struct export_funcs
{
    void* ptr;
    char name[64];
};

//struct call_trace
//{
//    bx_phy_address addr;
//};


extern const Bit64u PAGE_BASE_ADDR;
extern const Bit64u memSize;

class Engine
{
public:
    XE_CPU_C* cpu_ptr;
    XE_MEM_C* mem_ptr;
    bx_phy_address entry_addr;
    bx_phy_address call_host_ret_addr;
    bx_phy_address call_host_win32_ret_addr;
    bx_phy_address call_win32_unknow_sym_addr;
    uint32_t HOST_CALL_PTR32_addr;
    
    struct export_funcs* call_guess_method;
    uint64_t last_ret;
    
    std::shared_ptr<std::vector<uint64_t>> p_call_trace_win32;
    void push_call(uint64_t addr);
    void pop_call();
    void print_call_trace_win32();
    
    Engine(){
        cpu_ptr = nullptr;
        mem_ptr = nullptr;
        call_win32_unknow_sym_addr = 0;
//        call_trace_win32
        p_call_trace_win32 = std::make_shared<std::vector<uint64_t>>(1);
        p_call_trace_win32->reserve(16);
        HOST_CALL_PTR32_addr = 0;
    }
    void load_elf(const char* path);
    void load_dll32(const char* path,struct pe_image32** pe32 = nullptr,bool call_entry = false);
    
    void setup_os_env();
    void init();
    Bit8u* getHostMemAddr(bx_phy_address addr){
        return mem_ptr->getHostMemAddr(cpu_ptr, addr, BX_RW);
    }
    
    void run();
    void call_guest_method1(const char* method,uint64_t arg1);
    void call_win_guest_method1(const char* method,uint64_t arg1);
    void call_win32_guest_method1(const char* method,uint64_t arg1);
    void call_win32_dll_entry(bx_phy_address addr);
    
    void sw_cpu_mode(uint32_t mode);

};

extern std::shared_ptr<Engine> g_engine;

static inline Bit8u* getMemAddr(Bit64u addr)
{
    Bit8u* ret = BX_MEM(0)->getHostMemAddr(BX_CPU(0), addr, BX_RW);
    return ret;
}

static inline Bit8u* getGuestAddr(Bit64u  addr)
{
    return BX_MEM(0)->getGuestMemAddr(BX_CPU(0), addr);
}

//void load_RAM_from_data(Bit8u* data, Bit32u len, bx_phy_address ramaddress);
inline static void load_ram(Bit8u* data,Bit32u len,bx_phy_address ram_addr)
{
    BX_MEM(0)->load_RAM_from_data(data,len,ram_addr);
}

static inline void host_memcpy(char* guest_dst,char* src,Bit32u len)
{
    load_ram((Bit8u*)src, len, bx_phy_address(guest_dst));
}

inline bool is_cpu_mode32()
{
    return BX_CPU(0)->cpu_mode == BX_MODE_LONG_COMPAT;
}

#endif /* engine_hpp */
