//
//  test_guest_call.cpp
//  x86emu
//
//  Created by tony on 2019/9/24.
//  Copyright © 2019 tony. All rights reserved.
//

#define NEED_CPU_REG_SHORTCUTS 1

#include "cpu/x86.h"

#include "mmu.hpp"

#include "engine.hpp"
#include <algorithm>
#include <string>


// #define ELFTEST
#include "elf.h"
#include "elf-ext.hpp"
#include "logger.hpp"
#include "loaddll/load_dll.hpp"
#include "wrap_host_call.hpp"
#include "debug.hpp"

#include "wrap_guest_call.hpp"

static uint64_t wrap_test_ptr(uint64_t* args)
{
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        int a = w32.next<int>();
        int b = w32.next<int>();
        return a*b;
    }
    return 0;
}


void call_test_dll3()
{
     //uint64_t test_dll3(int a,const char* name,uint64_t a64)
    int arg1 = 11223344;
    const char* arg2 = "hello host ptr!";
    uint64_t arg3 = 0xabcedf123456789;
    auto parg2 = wrap_win32_ptr((void*)arg2, strlen(arg2));
    uint32_t arg4 = new_wrap_func(wrap_test_ptr, "test_ptr");
    
    auto ret = wrap_win32_guest_call("test_dll3", arg1,parg2->guest_addr,arg3,arg4);

    printf("call_test_dll3:ret:0x%0llx\n",ret);
    
}

void test_dll_func()
{
    
    printf("======test dll func =======\n");
    
    printf("=====test.dll======\n");
    
    //    load test.dll
    g_engine->load_dll32("test.dll",nullptr,true);
    
    call_test_dll3();
    
    

    
//        g_engine->call_win32_guest_method("test_dll2");
    //
    //
    //    auto f_ptr = new_wr]ap_func(wrap_test_func,"wrap_fun_ptr");
    //    auto pre_esp = ESP;
    //    g_engine->cpu_ptr->push_32(f_ptr);
    //    g_engine->call_win32_guest_method("test_call_ptr");
    //    ESP = pre_esp;
    
//    printf("=====test2.dll======\n");
//    g_engine->load_dll32("test2.dll",nullptr,true);
//
//    g_engine->call_win32_guest_method("test_cpp");
    
//    printf("========test lua!======\n");
//        g_engine->load_dll32("libs/lua53.dll",nullptr,true);
//        g_engine->load_dll32("testlua.dll",nullptr,true);
////
////
//        g_engine->call_win32_guest_method("testlua");
//        printf("lua ret code:%d\n",int(g_engine->last_ret));
    
}

inline WIN32_PTR get_win32_ptr(void* ptr)
{
    uint64_t p = (uint64_t)ptr;
    return (WIN32_PTR)(p&0xFFFFFFFF);
}


//int test_dll3(int a,const char* name,uint64 a64)
uint64_t wrap_guest_test_dll3(int arg1,const char* arg2,uint64_t arg3)
{
    typedef int T1;
    typedef WIN32_PTR T2;
    typedef uint64_t T3;
    auto len2 = strlen(arg2);
    T2 _arg2 = get_win32_ptr(host_malloc(len2+1));
    memcpy(getMemAddr(_arg2), arg2, len2);
    auto pre_esp = ESP;
    g_engine->cpu_ptr->push_64(arg3);
    g_engine->cpu_ptr->push_32(_arg2);
    g_engine->cpu_ptr->push_32(arg1);
    
    g_engine->call_win32_guest_method("test_dll3");
    host_free((void*)_arg2);
    //    ESP = EBP;
    ESP = pre_esp;
    
    return 0;
}
