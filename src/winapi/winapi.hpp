//
//  winapi.hpp
//  x86emu
//
//  Created by tony on 2019/9/18.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef winapi_hpp
#define winapi_hpp


#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include "loaddll/winnt_types.h"

    
#define DEF_HOST_STD_FUNC(func,idx,asize) \
uint64_t wrap_##func(uint64_t* args);
    
#include "winapi/wrap_winapi.h"
    
#undef DEF_HOST_STD_FUNC
    
#define DECLARE_CRT_EXPORT(name,_func) uint64_t wrap_##_func(uint64_t* args)
    
#ifdef __cplusplus
}
#endif




#endif /* winapi_hpp */
