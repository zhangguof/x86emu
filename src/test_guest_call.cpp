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

template<typename T>
struct traits_types
{
    static T get(void* ptr)
    {
        return *((T*)ptr);
    }
};

template<typename T>
struct traits_types<T*>
{
   static T* get(void* ptr)
    {
        auto p = traits_types<WIN32_PTR>::get(ptr);
        return (T*)getMemAddr(p);
    }
};


//guest obj
struct GuestObject
{
    void* obj; //ptr to host addr.
    GuestObject(void* p):obj(p){}
    template<typename T>
    T _obj_get(size_t offset)
    {
        auto p = (T*)((char*)obj + offset);
        return *p;
    }
};
/*
 // for mingw win32
class TestObj
{
    int a; //4
    int b;//4
    const char* name;//4
};
*/

//class Interface
//{
//    virtual void* get_fun_ptr(const char* name)=0;
//    virtual int get_fun_num() = 0;
//};
#include "WrapPointer.hpp"

#include <unordered_map>
static std::unordered_map<std::string, uint32_t> win32funcs;

uint32_t get_func_by_name(const char* name)
{
    auto it = win32funcs.find(name);
    if(it!=win32funcs.end())
    {
        return it->second;
    }
    return 0;
}
//typedef void (*testf_t)(int a,int* b,char* c,char** names);

static uint64_t wrap_test(uint64_t* args)
{
    printf("int wrap test!\n");
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        int a = w32.next<int>();
         WrapPointer<int> arg2(w32.next<uint32_t>());
//        WrapPointer<int> arg2 = w32.next<uint32_t>();
        WrapPointer<char> arg3(w32.next<uint32_t>());
        WrapPointer<char*> arg4(w32.next<uint32_t>());
//        *arg2 = 0x123456;
        printf("%s\n",arg3.get());
        const char* names[]={"test1111","test22222","test33333"};
        int n  = sizeof(names) / sizeof(const char*);
//        uint32_t buf = (uint32_t)(uint64_t)host_malloc(1024);
//        uint8_t* host_buf = getMemAddr((bx_phy_address)buf);
        *arg2 = n;
        for(int i=0;i<n;++i)
        {
//            arg4[i] = names[i]
//            int len = strlen(names[i]);
//            memcpy(host_buf,names[i],len+1);
            arg4[i] = names[i];
//            host_buf+= len+1;
//            buf += len+1;
        }
        
//        *arg4 = arg3.win32adr;
        
    }
    return 0;
}

void add_func_by_name(const char* name,host_fun_t ptr)
{
    auto addr = new_wrap_func(ptr, name);
    win32funcs[name] = addr;
}





//EXPORT void add_IObject(const char* obj_name,
//                        char* names,uint32_t stridx[],
//                        void* ptrs,uint32_t num)

struct IObject
{
    const char* obj_name;
    void add_func(const char* name,host_fun_t pf)
    {
        
    }
};

void test_dll_func()
{
    add_func_by_name("wrap_test", wrap_test);
//    WrapPointer ptr(0x400000);
//    const char* p = ptr.get<const char>();
//    printf("%s\n",p);
    
    printf("======test dll func =======\n");
    
    printf("=====test.dll======\n");
    
    //    load test.dll
//    g_engine->load_dll32("test.dll",nullptr,true);
    g_engine->load_dll32("test2.dll",nullptr,true);
//    void* r;
//    get_export32("get_interface", &r);
//    uint32_t addr = (uint64_t)r;
//    wrap_win32_guest_call("test_link", addr);
    wrap_win32_guest_call("test_cpp");
    
    
    
//    call_test_dll3();
    
    

    
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
