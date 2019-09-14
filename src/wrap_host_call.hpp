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

#include <string>
struct unknow_sym_info
{
    std::string dll;
    std::string name;
    unknow_sym_info(const char* d,const char* n):dll(d),name(n){}
    unknow_sym_info(std::string &d,std::string &n):dll(d),name(n){}
};

#include <vector>
extern std::vector<unknow_sym_info> unknow_sym_tbl;

typedef uint32_t WIN32_PTR;
extern void gen_unkown_code(uint8_t* code_buf, uint32_t idx,bx_phy_address unknow_sym_func_addr,bool is_32);


#endif /* wrap_host_call_h */
