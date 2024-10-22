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




//class Interface
//{
//    virtual void* get_fun_ptr(const char* name)=0;
//    virtual int get_fun_num() = 0;
//};
#include "WrapPointer.hpp"
#include "WrapCls.hpp"

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
        *arg2 = n;
        for(int i=0;i<n;++i)
        {

            arg4[i] = names[i];
        }
        
    }
    return 0;
}


struct WrapTestCls:public BaseWrapCls
{
    const uint32_t size = 24;
    WrapTestCls(uint8_t* ptr,uint32_t addr):BaseWrapCls(ptr,addr)
    {
        bind();
    }
    WrapTestCls()
    {
        guest_addr = (uint32_t)(uint64_t)host_malloc(size);
        host_ptr = getMemAddr(guest_addr);
        bind();
        init();
    }
    void init()
    {
        _data.a = 87654321;
        strncpy(_data.name.get(),"hhhxxx",10);
    }
    void bind()
    {
        assert(host_ptr!=nullptr);
        _data.a.init(get_data<int>(host_ptr,0));
        _data.n = get_pointer<int>(host_ptr,4);
        _data.name = get_array<char>(host_ptr,8);
        _data.names = get_pointer<char**>(host_ptr,20);
    }
    struct Data
    {
        Ref<int> a;
        WrapPointer<int> n;
        WrapPointer<char> name; //char buf[10]
        WrapPointer<char**> names;
    } _data;
};

static uint64_t wrap_test2(uint64_t* args)
{
    printf("int wrap test2!\n");
    if(is_cpu_mode32())
    {
        WIN32_ARGS w32 = {args};
        WrapPointer<uint8_t> pobj = w32.next<WIN32_PTR>();
        WrapTestCls obj(pobj.get(),pobj.win32addr);
//        obj._data.name = "Test in wraptest2!";
        *(obj._data.n) = 3211234;
        obj._data.names[0] = "Test in WrapTest2";
        strcpy(obj._data.name.get(), "hhxxddff");
        obj._data.a = 33344455;
        printf("a=%d\n",obj._data.a.get());
        
        WrapTestCls ret;
        return ret.getaddr();
    }
    return 0;
}

void add_func_by_name(const char* name,host_fun_t ptr)
{
    auto addr = new_wrap_func(ptr, name);
    win32funcs[name] = addr;
}

struct SharedMem
{
    uint8_t* h_ptr;
    uint32_t size;
    uint32_t g_addr;
    uint8_t* g_ptr;
    SharedMem(uint8_t* ptr,uint32_t s):h_ptr(ptr),size(s){
        g_addr = (uint32_t)(uint64_t)host_malloc(s);
        g_ptr = getMemAddr(g_addr);
    }
    void host2guest()
    {
        memcpy(g_ptr, h_ptr, size);
    }
    void guest2host()
    {
        memcpy(h_ptr, g_ptr, size);
    }
    void sync()
    {
        
    }
    ~SharedMem()
    {
        void* ptr = (void*)g_addr;
        host_free(ptr);
    }
};


struct Test
{
    int a;
    int *ptr_n;
    const char* name;
    Test(int x):a(x){
        ptr_n = nullptr;
        name = "hello";
    }
};

//gen code
struct WrapTest
{
    struct Test* imp;
    void* g_ptr;
    struct ABI* ABI_Test;

    void _init_from_host(int x)
    {
        imp = new Test(x);
    }
    void _init_from_guest()
    {
        
    }
};


struct IClass
{
    const char* cls_name;
    void* guest_obj;
    std::unordered_map<std::string, uint32_t> cls_methods;
    void add_method(const char* name,host_fun_t pf)
    {
        cls_methods[name] = new_wrap_func(pf, name);
    }
    static uint64_t wrap_test(uint64_t* args)
    {
        return 0;
    }
#define DEF_CLS_METHOD(func) \
add_method(#func, wrap_##func);
    
    
    void init()
    {
        DEF_CLS_METHOD(test);
    }
#undef DEF_CLS_METHOD
};

void test_dll_func()
{
    add_func_by_name("wrap_test", wrap_test);
    add_func_by_name("wrap_test2", wrap_test2);
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
