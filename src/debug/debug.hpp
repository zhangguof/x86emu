//
//  debug.hpp
//  x86emu
//
//  Created by tony on 2019/9/16.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef debug_hpp
#define debug_hpp

#include <stdio.h>
#include "dwarf.h"
#include "libdwarf.h"
#include <stdint.h>
#include <string>

extern int test_debug();
struct DEBUG_INFO
{
//    const char* name;
    std::string name;
    uint64_t low_pc;
    uint64_t high_pc;
    DEBUG_INFO(std::string &n,uint64_t l,uint64_t h):name(n),low_pc(l),high_pc(h){}
    DEBUG_INFO(const char* n,uint64_t l,uint64_t h):name(n),low_pc(l),high_pc(h){}
};

#include <unordered_map>
extern std::unordered_map<uint64_t, std::shared_ptr<DEBUG_INFO>> global_debug_info;



#endif /* debug_hpp */
