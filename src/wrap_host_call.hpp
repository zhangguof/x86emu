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

#define DEF_HOST_FUNC(func,idx) \
uint64_t wrap_##func(uint64_t* args);

#include "host_call.hpp"

#undef DEF_HOST_FUNC

#endif /* wrap_host_call_h */
