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

struct WIN32_ARGS{
    void* ptr;
    template<typename T>
    T next()
    {
        T* p = (T*)ptr;
        ptr = (char*)ptr + (sizeof(T)<4?4:sizeof(T));
        return *p;
    }
};
typedef uint32_t WIN32_PTR;


#endif /* wrap_host_call_h */
