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



void test_dll_func()
{
    
    printf("======test dll func =======\n");
    
    printf("=====test.dll======\n");
    //    load test.dll
    //    g_engine->load_dll32("test.dll",nullptr,true);
    //    g_engine->call_win32_guest_method1("test_dll2", 0);
    //
    //
    //    auto f_ptr = new_wr]ap_func(wrap_test_func,"wrap_fun_ptr");
    //    auto pre_esp = ESP;
    //    g_engine->cpu_ptr->push_32(f_ptr);
    //    g_engine->call_win32_guest_method1("test_call_ptr", 0);
    //    ESP = pre_esp;
    
    printf("=====test2.dll======\n");
    g_engine->load_dll32("test2.dll",nullptr,true);
    
    g_engine->call_win32_guest_method1("test_cpp", 0);
    
    
    //    g_engine->load_dll32("libs/lua53_1.dll");
    //    g_engine->load_dll32("testlua.dll");
    
    
    //    g_engine->call_win32_guest_method1("testlua", 0);
    //    printf("lua ret code:%d\n",int(g_engine->last_ret));
    
}
