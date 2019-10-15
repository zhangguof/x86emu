//
//  wrap_guest_call.hpp
//  x86emu
//
//  Created by tony on 2019/9/24.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef wrap_guest_call_hpp
#define wrap_guest_call_hpp

#include "engine.hpp"
#include "mmu.hpp"
#include <vector>

//wrap host mem as arg pass to guest.
struct HostMem
{
    void* host_ptr;
    size_t size;
    uint32_t guest_addr;
    void alloc(size_t sz);
    HostMem();
    HostMem(size_t sz);
    HostMem(void* host_src_ptr, size_t sz);
    void copy_from(void* host_src_ptr,size_t sz);
    ~HostMem();
};

//typedef std::vector<std::shared_ptr<HostMem>> HostMemTbl_t;

template<typename T>
std::shared_ptr<HostMem> wrap_win32_ptr(T* host_ptr,size_t sz)
{
    auto ret = std::make_shared<HostMem>((void*)host_ptr,sz);
    return  ret;
}

template<typename T>
void _do_push_args(T val)
{
    size_t size = sizeof(T);
    assert(size<=8);
    
    if(size <= 4)
    {
        g_engine->cpu_ptr->push_32(val);
    }
    else
    {
        g_engine->cpu_ptr->push_64(val);
    }
}

void do_push_args();

template <typename T,typename... ARGS>
void do_push_args(T val,ARGS... args)
{
    do_push_args(args...);
    _do_push_args(val);
    
}

template<typename... ARGS>
uint64_t wrap_win32_guest_call(const char* name,ARGS... args)
{
    g_engine->save_esp();
    do_push_args(args...);
    
    g_engine->call_win32_guest_method(name);
    
    auto ret = g_engine->last_ret;
    g_engine->restore_esp();
    return ret;
    
}

template<typename... ARGS>
uint64_t wrap_win32_guest_call(uint32_t addr,ARGS... args)
{
    g_engine->save_esp();
    do_push_args(args...);
    
    g_engine->call_win32_guest_addr(addr);
    
    auto ret = g_engine->last_ret;
    g_engine->restore_esp();
    return ret;
    
}




#endif /* wrap_guest_call_hpp */
