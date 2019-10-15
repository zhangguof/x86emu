//
//  wrap_guest_call.cpp
//  x86emu
//
//  Created by tony on 2019/9/24.
//  Copyright © 2019 tony. All rights reserved.
//

#include "wrap_guest_call.hpp"
#include <vector>

HostMem::HostMem()
{
    host_ptr = nullptr;
    guest_addr = 0;
    size = 0;
}
HostMem::HostMem(size_t sz){
    alloc(sz);
}
void HostMem::alloc(size_t sz)
{
    guest_addr = (uint64_t)host_malloc(sz);
    size = sz;
    host_ptr = (void*)getMemAddr(guest_addr);
}

HostMem::HostMem(void* host_src_ptr,size_t sz){
    alloc(sz);
    memcpy(host_ptr, host_src_ptr, sz);
}
void HostMem::copy_from(void *host_src_ptr, size_t sz)
{
    alloc(sz);
    memcpy(host_ptr, host_src_ptr, sz);
}

HostMem::~HostMem()
{
    host_free((void*)guest_addr);
}

//do nothing
void do_push_args()
{
    
}
