//
//  init_host_caller.cpp
//  x86emu
//
//  Created by tony on 2019/10/15.
//  Copyright © 2019 tony. All rights reserved.
//

//#include "init_host_caller.hpp"

#define INIT_CALLER(cls) \
    extern void init_caller_##cls();

#include "host_caller_objs.hpp"
    
#undef INIT_CALLER


typedef void (*func_t)();


static func_t caller_tbl[]={
#define INIT_CALLER(cls) \
    init_caller_##cls,
    
#include "host_caller_objs.hpp"
    
#undef INIT_CALLER
};

void init_caller_objs()
{
    int len = sizeof(caller_tbl) / sizeof(func_t);
    for(int i = 0;i<len;++i)
    {
        caller_tbl[i]();
    }
}
