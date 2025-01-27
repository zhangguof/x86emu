//
//  wrap_host_call.cpp
//  x86emu
//
//  Created by tony on 2019/9/4.
//  Copyright © 2019 tony. All rights reserved.
//
#define NEED_CPU_REG_SHORTCUTS 1
#include "cpu/x86.h"
#include "param_names.h"
#include "elf.h"
#include <stdio.h>
#include "mmu.hpp"

#include <string.h>
#include <functional>
#include "engine.hpp"

#include "wrap_host_call.hpp"
#include "utils.h"
#include "logger.hpp"

#include <unordered_map>


uint32_t host_call_used_idx = 0x20000;

//guest return to host

inline uint64_t do_ret(int64_t ret_code)
{
    //    g_engine->last_ret = code;
    //    printf("return from guset code!:%ld\n",ret_code);
    g_engine->cpu_ptr->is_exit = true;
    g_engine->last_ret = ret_code;
    return 0;
}

#define GET_ARGS(id,type) \
(type)args[id]

#define GET_ARGS32(w32_args,type)\
(type)w32_args.next<type>()

#define GET_PTR_ARG(ptr,type) \
(type)(getMemAddr((uint64_t)ptr))


#define DEF_HOST_FUNC(func) \
uint64_t wrap_##func(uint64_t* args)

// __std_call int64_t do_ret(int64_t ret_code);
DEF_HOST_FUNC(do_ret)
{
    int64_t ret_code = 0;
    if(is_cpu_mode32())
    {
        //fix esp ,no ret addr
        args--;
        WIN32_ARGS w32_args = {args};
        ret_code = GET_ARGS32(w32_args, int64_t);
        //for std_call
        ESP += sizeof(int64_t);
    }
    else
    {
        ret_code = (int64_t)args[0];
    }
  
    return do_ret(ret_code);
}

DEF_HOST_FUNC(puts)
{
    typedef const char* T1;
//    typedef uint32_t UPtr32;
    T1 arg1;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        arg1 = GET_PTR_ARG(GET_ARGS32(w32_args, WIN32_PTR), T1);
    }
    else
    {
        arg1 = GET_PTR_ARG(GET_ARGS(0, T1), T1);
    }
    return puts(arg1);
}

DEF_HOST_FUNC(malloc)
{
    uint64_t size = 0;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        typedef uint32_t size_t;
        size = w32_args.next<size_t>();
    }
    else
    {
        size = args[0];
       
    }
     return  (uint64_t)host_malloc(size);

}

//void free(void*);
DEF_HOST_FUNC(free)
{
    void* ptr = nullptr;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        typedef uint32_t size_t;
        ptr = (void*)GET_ARGS32(w32_args, WIN32_PTR);
    }
    else
    {
        ptr = (void*)args[0];

    }
    host_free(ptr);
    return 0;
}

//void *realloc(void *mem_address, unsigned int newsize);
DEF_HOST_FUNC(realloc)
{
    void* ptr = nullptr;
    uint64_t size = 0;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        typedef uint32_t size_t;
        ptr = (void*)(w32_args.next<WIN32_PTR>());
        size = w32_args.next<size_t>();
    }
    else
    {
        ptr = (void*)args[0];
        size = args[1];
        
    }
    return  (uint64_t)host_realloc(ptr,size);
}
//void* calloc（unsigned int num，unsigned int size）；
DEF_HOST_FUNC(calloc)
{
    uint64_t num = 0;
    uint64_t size = 0;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        typedef uint32_t size_t;
        num = w32_args.next<size_t>();
        size = w32_args.next<size_t>();
    }
    else
    {
        num = args[0];
        size = args[1];
    }
    return  (uint64_t)host_calloc(num,size);
}

//time_t time(time_t *);
DEF_HOST_FUNC(time)
{
//    void* time_ptr =
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        typedef uint32_t size_t;
        uint32_t* ptr = (uint32_t*)w32_args.next<WIN32_PTR>();
        if(ptr == 0) return (uint64_t)time(nullptr);
        
        ptr = (uint32_t*)(getMemAddr((bx_phy_address)ptr));
        uint64_t ret = time(nullptr);
        *ptr = (uint32_t)ret;
        return ret;
    }
    else
    {
        time_t* ptr = (time_t*)args[0];
        if(ptr == 0)
        {
            return (uint64_t)time(nullptr);
        }
        else
        {
            ptr = (time_t*)(getMemAddr((bx_phy_address)ptr));
            return (uint64_t)time(ptr);
        }
    }
}

DEF_HOST_FUNC(time64)
{
    return wrap_time(args);
}

DEF_HOST_FUNC(time32)
{
//    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        typedef uint32_t size_t;
        uint32_t* ptr = (uint32_t*)w32_args.next<WIN32_PTR>();
        if(ptr == 0) return (uint64_t)time(nullptr);
        
        ptr = (uint32_t*)(getMemAddr((bx_phy_address)ptr));
        uint64_t ret = time(nullptr);
        *ptr = (uint32_t)ret;
        return ret;
    }
//    return 0;
}




//test_f1(int,unsigned int , const char*a)
void test_f1(int a1,unsigned int a2, const char* s1,char c1,size_t st1,
             uint16_t u1,uint64_t u2)
{
    printf("test fun1\n");
    printf("a1:%d,a2:%u,s1:%s,c1:%c,st1:%u,u1:%u,u2:0x%0lx\n",
           a1,a2,s1,c1,st1,u1,u2);
    printf("size:%u,%u,%u,%u,%u\n",sizeof(a1),sizeof(a2),
           sizeof(s1),sizeof(c1),sizeof(st1));
}
uint64_t wrap_test_f1(uint64_t* args)
{
    WIN32_ARGS w32_args = {(void*)args};
    typedef uint32_t size_t;
    typedef int T1;
    typedef unsigned int T2;
    typedef const char* T3;
    typedef char T4;
    typedef size_t T5;
    typedef uint16_t T6;
    typedef uint64_t T7;
    auto arg1 = (T1)(w32_args.next<T1>());
    auto arg2 = (T2)(w32_args.next<T2>());
    
    auto arg3 = (T3)(getMemAddr(w32_args.next<WIN32_PTR>()));
    
    auto arg4 = (T4)(w32_args.next<T4>());
    auto arg5 = (T5)(w32_args.next<T5>());
    auto arg6 = (T6)(w32_args.next<T6>());
    auto arg7 = (T7)(w32_args.next<T7>());
    test_f1(arg1, arg2, arg3,
            arg4, arg5,arg6,
            arg7);
    
    return 0x12345678FEDCBAFF;
}
//void testtest();
//void testtest()
//{
//
//    printf("test!!!\n");
//}

//static void alias_testtest()__attribute__((weakref("testtest")));

DEF_HOST_FUNC(exit)
{
    uint64_t ret_code = 0;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        ret_code = w32_args.next<uint32_t>();
        
    }
    else
    {
        ret_code = args[0];
    }
    LOG_WARN("exiting....");
    g_engine->print_call_trace_win32();
    exit(ret_code);
}
/*
 push idx
 mov eax, addr
 jmp [eax]
 
 00000000 <.text>:
 0:    68 78 56 34 12           push   0x12345678
 5:    b8 55 66 80 00           mov    eax,0x806655
 a:    ff d0                    call   eax
 */
void gen_unkown_code(uint8_t* code_buf, uint32_t idx,bx_phy_address unknow_sym_func_addr,bool is_32)
{
    if(is_32)
    {
        *(code_buf++) = 0x68;
        uint32_t val = idx;
        *((uint32_t*)code_buf) = idx;
        code_buf += sizeof(uint32_t);
        *(code_buf++) = 0xb8;
        *((uint32_t*)code_buf) = (uint32_t)(unknow_sym_func_addr & 0xFFFFFFFF);
        code_buf += sizeof(uint32_t);
        *(code_buf++) = 0xff;
        *(code_buf++) = 0xd0;
        
    }
    else
    {
        // TODO
    }
}

#include <vector>
std::vector<unknow_sym_info> unknow_sym_tbl;

uint64_t wrap_unknow_sym(uint64_t* args)
{
//    g_engine->cpu_ptr->debug_disasm_instruction(g_engine->cpu_ptr->prev_rip);
    uint64_t idx = 0;
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        idx = w32_args.next<uint32_t>();

    }
    else
    {
        idx = args[0];
    }
    size_t len = unknow_sym_tbl.size();
    if(idx < len)
    {
        auto info = unknow_sym_tbl[idx];
        LOG_ERROR("unknow symbol %s:%s\n",info.dll.c_str(),
               info.name.c_str());
    }
    g_engine->print_call_trace_win32();
    exit(0);
    return 0;
}
//char* getenv (const char* name);
DEF_HOST_FUNC(getenv)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args = {args};
        char* name = (char*) getMemAddr((bx_address)w32_args.next<WIN32_PTR>());
        char* ret = getenv(name);
        if(ret)
        {
            auto len = strlen(ret);
            char* new_str = (char*)host_malloc(len+1);
            host_memcpy(new_str, ret, len+1);
            return (uint64_t)new_str;
        }
        else
        {
            return 0;
        }
        
        
    }
    else
    {
        //TODO
        return 0;
    }
}

extern uint32_t get_func_by_name(const char*);

DEF_HOST_FUNC(get_func_by_name)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        const char* name = (const char*)getMemAddr(w32.next<WIN32_PTR>());
        return get_func_by_name(name);
    }
    return 0;
}



/*
 0:    55                       push   ebp
 1:    89 e5                    mov    ebp,esp
 3:    b8 33 20 00 00           mov    eax,0x2033
 8:    ff 25 fd ff ff 1f        jmp    DWORD PTR ds:0x1ffffffd
 e:    c9                       leave
 f:    c3                       ret
 
 10:    55                       push   ebp
 11:    89 e5                    mov    ebp,esp
 13:    b8 44 20 00 00           mov    eax,0x2044
 18:    ff 25 80 00 80 00        jmp    DWORD PTR ds:0x800080
 1e:    c9                       leave
 1f:    c2 03 00                 ret    0x3
 */
struct CodeBuf
{
    uint8_t* ptr;
    template<typename T>
    void write(T a)
    {
        *((T*)ptr) = a;
        ptr += sizeof(T);
    }
    void write_bytes(char* b,size_t size)
    {
        while(size--) *ptr++ = *b++;
    }
};
//void gen_wrap_func_code(uint8_t* code_buf, uint32_t idx,bool is_32)
void gen_wrap_func_code(CodeBuf* code_buf,uint32_t idx,bool is_32,uint16_t ret_n=0)
{
    if(is_32)
    {
        uint32_t host_ptr_addr = g_engine->HOST_CALL_PTR32_addr;
        assert(host_ptr_addr);

        code_buf->write_bytes("\x55\x89\xe5\xb8", 4);


        code_buf->write(idx); //mov    eax,idx
        
        code_buf->write_bytes("\xff\x25", 2);  // jmp [host_prt_addr]
        code_buf->write((uint32_t)host_ptr_addr);
        
        code_buf->write((uint8_t)0xc9); //leave
        if(ret_n == 0)
        {
            code_buf->write((uint8_t)0xc3);
        }
        else
        {
            code_buf->write((uint8_t)0xc2);
            code_buf->write((uint16_t)ret_n);
        }
        
    }
    else
    {
        // TODO
    }
}



//std::unordered_map<WIN32_PTR, wrap_func_ptr_t> user_wrap_func_tbl;
HostCallTbl_t* p_user_host_call_tbl = nullptr;

uint64_t do_host_fun_ptr(WIN32_PTR ptr,uint64_t* args)
{
    if(p_user_host_call_tbl==nullptr)
    {
        LOG_ERROR("user wrap func table not init before call:0x%0x!",ptr);
        exit(1);
    }
    auto it = p_user_host_call_tbl->find(ptr);
    if(it==p_user_host_call_tbl->end())
    {
        LOG_ERROR("unknow user wrap func:0x%0lx\n",ptr);
        exit(1);
    }
    auto f = it->second->pf;
    return f(args);
}

static std::unordered_map<wrap_func_ptr_t, WIN32_PTR> wrap_func_cache;

static WIN32_PTR check_func_cache(wrap_func_ptr_t pf)
{
    auto it = wrap_func_cache.find(pf);
    if(it==wrap_func_cache.end()) return 0;
    return it->second;
}

WIN32_PTR new_wrap_func(wrap_func_ptr_t pf,const char* name,uint16_t ret_n)
{
    uint32_t buf_addr32 = check_func_cache(pf);
    if(buf_addr32!=0) return buf_addr32;
    
    bx_phy_address buf_addr = (bx_phy_address)host_malloc(32);
    buf_addr32 = (uint32_t)buf_addr;
    uint8_t* buf = getMemAddr(buf_addr);
    CodeBuf code_buf = {buf};
//     uint32_t idx = host_call_used_idx++;
    uint32_t idx = buf_addr32;
    gen_wrap_func_code(&code_buf, idx, true,ret_n);
//    user_wrap_func_tbl[buf_addr32] = pf;
    if(p_user_host_call_tbl == nullptr)
    {
        p_user_host_call_tbl = new HostCallTbl_t;
    }
   
    (*p_user_host_call_tbl)[idx] = std::make_shared<HOST_FUN_C>(name,pf,idx);
    
    wrap_func_cache[pf] = buf_addr32;
    
    return buf_addr32;
    
}



//add_host_call_func(#func,func);


//void HostCallerBase::add_host_call_func(const char* name, wrap_func_ptr_t pf)
//{
//    new_wrap_func(pf, name);
//}

std::vector<HostCallerBase*> *HostCallerBase::call_tbl = nullptr;

void HostCallerBase::add_caller(HostCallerBase* base)
{
    if(call_tbl==nullptr)
        call_tbl = new std::vector<HostCallerBase*>;
    call_tbl->push_back(base);
}

void HostCallerBase::add_host_func(const char* name,wrap_func_ptr_t f)
{
    uint32_t addr = new_wrap_func(f, name);
    add_export32(name, addr);
}
void HostCallerBase::add_host_std_func(const char *name, wrap_func_ptr_t f, uint16_t retn)
{
    uint32_t addr = new_wrap_func(f, name,retn);
    add_export32(name, addr);
}

void HostCallerBase::init()
{
    for(auto it:*call_tbl)
    {
        it->init_funcs();
    }
}

void HostCallerBase::init_funcs()
{
//    DEF_USER_HOST_CALL(HostCallerBase,base_test_func);
}



static int test_func(int a,int b)
{
    return a+b;
}

uint64_t wrap_test_func(uint64_t* args)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32_args =  {args};
        int arg1 = w32_args.next<int>();
        int arg2 = w32_args.next<int>();
        return test_func(arg1, arg2);
    }
    else
    {
        //TODO
        return 0;
    }
    
}






#undef DEF_HOST_FUNC
