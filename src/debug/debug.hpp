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

extern int load_debug(const char *filepath );
extern void load_debug_info(const char* dll_path,uint64_t dll_base,uint64_t load_addr);

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
typedef std::unordered_map<uint64_t, std::shared_ptr<DEBUG_INFO>> Debug_info_tbl_t;
extern Debug_info_tbl_t global_debug_info;



#endif /* debug_hpp */
