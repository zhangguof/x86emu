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

const uint32_t WINAPI_START = 0x1000;
const uint32_t USERAPI_START = 0x20000;
extern uint32_t host_call_used_idx;
#include <unordered_map>
//typedef int (*HFun_is)(const char*);
typedef  uint64_t (*host_fun_t)(uint64_t*);

struct HOST_FUN_C
{
    //    void* ptr;
    const char* name;
    //    std::function<uint64_t(uint64_t*)> pf;
    host_fun_t pf;
    uint32_t idx;
    HOST_FUN_C(const char* n,host_fun_t f,uint32_t i):name(n),pf(f),idx(i){}
    
    //LP64
};
typedef std::unordered_map<uint32_t, std::shared_ptr<HOST_FUN_C>> HostCallTbl_t;
extern HostCallTbl_t* p_user_host_call_tbl;

extern std::unordered_map<uint32_t,HOST_FUN_C*> host_call_hash_tbl;

typedef uint64_t (*wrap_func_ptr_t)(uint64_t*);
extern uint64_t do_host_fun_ptr(WIN32_PTR ptr,uint64_t* args);

extern uint32_t new_wrap_func(wrap_func_ptr_t pf,const char* name,uint16_t  ret_n = 0);

extern void add_export32(const char* name,uint32_t addr);

#define DEF_USER_HOST_CALL(cls,func) \
add_host_func(#func,cls::wrap_##func);

#define DEF_STD_USER_HOST_CALL(cls,func,retn) \
add_host_std_func(#func,cls::wrap_##func,retn);

#include <vector>
class HostCallerBase
{
public:
    static std::vector<HostCallerBase*> *call_tbl;
    static void add_caller(HostCallerBase* base);
    static void add_host_func(const char* name,wrap_func_ptr_t f);
    static void add_host_std_func(const char* name,wrap_func_ptr_t f,uint16_t retn);
    static void init();
//    static void add_export32(const char* name,)
    
    HostCallerBase(){
        add_caller(this);
    }
    virtual void init_funcs();
};


#endif /* wrap_host_call_h */
