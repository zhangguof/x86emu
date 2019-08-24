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


class Engine
{
public:
    XE_CPU_C* cpu_ptr;
    XE_MEM_C* mem_ptr;
    Engine(){
        cpu_ptr = nullptr;
        mem_ptr = nullptr;
    }
    void load_elf(const char* path);
    void setup_os_env();
    void init();
    
    void run();
};

#endif /* engine_hpp */
