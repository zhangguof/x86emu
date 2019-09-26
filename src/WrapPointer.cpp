//
//  WrapPointer.cpp
//  x86emu
//
//  Created by tony on 2019/9/26.
//  Copyright © 2019 tony. All rights reserved.
//

#include "WrapPointer.hpp"
#include "mmu.hpp"

#include <unordered_map>
//#include <vector>
#include <string>
static std::unordered_map<std::string, uint32_t> g_conststr_map;

class ConstHostBuf
{
public:
    
    uint8_t* host_buf;
    uint32_t guest_addr;
    uint32_t size;
    uint32_t idx;
    ConstHostBuf();
    uint32_t new_str(const char* str);
    
};

//extern ConstHostBuf const_str_buf;


ConstHostBuf::ConstHostBuf()
{
    size = 0;
    idx = 0;
    guest_addr = 0;
    host_buf = nullptr;
//    guest_addr = (uint32_t)(uint64_t)host_malloc(size);
//    host_buf = getMemAddr(guest_addr);
}
uint32_t ConstHostBuf::new_str(const char* str)
{
    auto it = g_conststr_map.find(str);
    if(it!=g_conststr_map.end()) return it->second;
    auto len = strlen(str) + 1 ;
    if(idx + len >= size)
    {
        uint32_t new_size = (size + (idx+len)) * 2;
        if(new_size<1024) new_size = 1024;
//        guest_addr = (uint32_t)(uint64_t)host_realloc((void*)guest_addr, new_size);
        guest_addr = (uint32_t)(uint64_t)host_malloc(new_size);
        host_buf = getMemAddr(guest_addr);
        size = new_size;
        idx = 0;
    }
    memcpy(host_buf+idx, str,len);
    uint32_t ret = guest_addr + idx;
    idx += len;
    g_conststr_map[str] = ret;
    return ret;
}
ConstHostBuf const_str_buf;

uint32_t new_cosnt_buf(const char* str)
{
    return const_str_buf.new_str(str);
}


