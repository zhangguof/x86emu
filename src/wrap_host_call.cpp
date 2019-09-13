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


uint64_t wrap_unknow_sym(uint64_t* args)
{
    g_engine->cpu_ptr->debug_disasm_instruction(g_engine->cpu_ptr->prev_rip);
//    testtest();

    exit(0);
}

//set_thread_area(struct user_desc* pebdescriptor)




#undef DEF_HOST_FUNC
