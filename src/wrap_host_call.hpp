//
//  wrap_host_call.hpp
//  x86emu
//
//  Created by tony on 2019/9/4.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef wrap_host_call_h
#define wrap_host_call_h

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define DEF_HOST_FUNC(func,idx) \
uint64_t wrap_##func(uint64_t* args);

#include "host_call.hpp"
#include "gen_code/wrap_gen_code.h"

#undef DEF_HOST_FUNC
    
#ifdef __cplusplus
}
#endif

#endif /* wrap_host_call_h */
