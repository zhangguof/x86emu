//
//  engine.cpp
//  x86emu
//
//  Created by tony on 2019/8/19.
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
extern "C"{
    extern const bool DEBUG_DASM = false;
}


extern void bx_init_options();
extern void bx_init_siminterface();
extern void bx_init_hardware();
//extern void plugin_startup();

void env_init()
{
    //    plugin_startup();
    //    pluginlog = new logfunctions();
    //    pluginlog->put("PLUGIN");
    
    SAFE_GET_IOFUNC();  // never freed
    SAFE_GET_GENLOG();  // never freed
    genlog->setonoff(LOGLEV_DEBUG, ACT_REPORT);
    
    bx_init_siminterface();
    bx_init_options();
    //    bx_init_hardware();
}

const Bit64u memSize = 32*1024*1024; //32M
const Bit64u hostMemSize = 32*1023*1024;

const Bit64u PAGE_BASE_ADDR = (0x100000);// page data.
const Bit64u RUN_BASE_ADDR  = (0x400000); //4M start. load exe
const Bit64u DLL_LAOD_BASE =  RUN_BASE_ADDR + 0x400000; //load dll,so
Bit64u g_dll_next_ptr = DLL_LAOD_BASE;

const Bit64u BASE_HEAP_ADDR_END = g_dll_next_ptr;

#define BX_CR3_PAGING_MASK    (BX_CONST64(0x000ffffffffff000))

#define BX_CR3_PAGING_MASK    (BX_CONST64(0x000ffffffffff000))

std::string work_home = "/Users/tony/workspace/github/x86emu/TiniyOs/";
std::string g_so_path = work_home;
std::string start_elf_file = "tiniyos.elf";
std::string g_dll32_path = work_home + "crtdll/win32/";
std::string crt32_dll_file = "crt32.dll";

/*
 load Tiniy os exe file.
 load elf --> load needed so --> load other so file.
 
 
 */
std::shared_ptr<Engine> g_engine = nullptr;

void Engine::load_elf(const char* elf_file_name)
{
    
    bx_phy_address base_addr = RUN_BASE_ADDR; //2M start.
    
    auto pdll = try_load_so(elf_file_name, &base_addr,false);
    
    Elf64_Ehdr* p_elf_h = (Elf64_Ehdr*) pdll->host_code;
//    print_elf_info(p_elf_h);
    
    bx_phy_address entry_addr = p_elf_h->e_entry;

    
    this->entry_addr = entry_addr;
//    cpu_ptr->prev_rip = cpu_ptr->gen_reg[BX_64BIT_REG_RIP].rrx = entry_addr;
    bx_phy_address heap_start = 0;
    if(pdll->data)
    {
        heap_start = (bx_phy_address)pdll->data + pdll->datalen;
        
    }
    else
    {
        heap_start = (bx_phy_address)pdll->code + pdll->codelen;
    }
    heap_start = (heap_start+0xff) & (~0xff);
    init_mem_allocate(heap_start, BASE_HEAP_ADDR_END);
    setup_os_env();
}

void Engine::load_dll32(const char* dll_file_name,struct pe_image32** pe32,bool call_entry)
{
    //load dll
    //    const char* dll_path = "/Users/tony/workspace/github/x86emu/Tin/dll/test.dll";
    std::string dll_path = g_dll32_path+dll_file_name;
    struct pe_image32* pe;
//    if(pe32 == nullptr)
    //    try_load_dll64(dll_path.c_str(),&pe);
    if(pe32 == nullptr)
    {
        pe32 = &pe;
    }
    try_load_dll32(dll_path.c_str(), pe32);
    if(call_entry)
    {
        char *name = (*pe32)->name;
        void* addr = (*pe32)->entry;
        LOG_DEBUG("try call dll entry:%s,%p\n",name,addr);
        call_win32_dll_entry((bx_phy_address)addr);
    }
}


void Engine::setup_os_env()
{
    // The RESET function will have been called first.
    // Set CPU and memory features which are assumed at this point.
    
    // EIP deltas
    //    BX_CPU(0)->prev_rip = BX_CPU(0)->gen_reg[BX_32BIT_REG_EIP].dword.erx = 0x100000;
    
    
    // CS deltas
    cpu_ptr->sregs[BX_SEG_REG_CS].cache.u.segment.base = 0x00000000;
    cpu_ptr->sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled = 0xFFFFFFFF;
    cpu_ptr->sregs[BX_SEG_REG_CS].cache.u.segment.g   = 1; // page granularity
    cpu_ptr->sregs[BX_SEG_REG_CS].cache.u.segment.d_b = 1; // 32bit
    cpu_ptr->sregs[BX_SEG_REG_CS].cache.u.segment.l = 1; // 64bit
    
    // DS deltas
    cpu_ptr->sregs[BX_SEG_REG_DS].cache.u.segment.base = 0x00000000;
    cpu_ptr->sregs[BX_SEG_REG_DS].cache.u.segment.limit_scaled = 0xFFFFFFFF;
    cpu_ptr->sregs[BX_SEG_REG_DS].cache.u.segment.g   = 1; // page granularity
    cpu_ptr->sregs[BX_SEG_REG_DS].cache.u.segment.d_b = 1; // 32bit
    
    // SS deltas
    Bit64u Last_1M = mem_ptr->get_memory_len() - 1024*1024;
    
    cpu_ptr->sregs[BX_SEG_REG_SS].cache.u.segment.base = 0x00000000;
    cpu_ptr->sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled = 0xFFFFFFFF;
    cpu_ptr->sregs[BX_SEG_REG_SS].cache.u.segment.g   = 1; // page granularity
    cpu_ptr->sregs[BX_SEG_REG_SS].cache.u.segment.d_b = 1; // 32bit
    
    //ES deltas
    cpu_ptr->sregs[BX_SEG_REG_ES].cache.u.segment.base = 0x00000000;
    cpu_ptr->sregs[BX_SEG_REG_ES].cache.u.segment.limit_scaled = 0xFFFFFFFF;
    cpu_ptr->sregs[BX_SEG_REG_ES].cache.u.segment.g   = 1; // page granularity
    cpu_ptr->sregs[BX_SEG_REG_ES].cache.u.segment.d_b = 1; // 32bit
    
    RSP = Last_1M;
    
    
    
    // CR0 deltas
    cpu_ptr->cr0.set_PE(1); // protected mode
    cpu_ptr->cr0.set_PG(1);
    //Emulation    If set, no x87 floating-point unit present, if clear, x87 FPU present
    cpu_ptr->cr0.set_EM(0);
    //Monitor co-processor    Controls interaction of WAIT/FWAIT instructions with TS flag in CR0
    cpu_ptr->cr0.set_MP(1);
    //SSE support.
    cpu_ptr->cr4.set_OSFXSR(1);
    cpu_ptr->cr4.set_OSXMMEXCPT(1);
    
    cpu_ptr->cr4.set_PAE(1);
    
    
    //load page
    mem_ptr->alloca_pagebit64(PAGE_BASE_ADDR);
    Bit64u cr3_val = (PAGE_BASE_ADDR & BX_CR3_PAGING_MASK);
    
    
    cpu_ptr->efer.set_LME(1);
    cpu_ptr->efer.set_LMA(1);
    cpu_ptr->efer.set_SCE(1); //syscall enable;
    
    cpu_ptr->handleSseModeChange();
    
    cpu_ptr->handleCpuModeChange();
    cpu_ptr->SetCR3(cr3_val);
    
}

extern void init_host_call_tbl();
std::vector<uint64_t> call_trace_win32;

void Engine::init()
{
    init_host_call_tbl();
    call_trace_win32.clear();
    call_trace_win32.reserve(128);
    
    env_init();
    //    bx_cpu_ptr = &::bx_cpu; //global bx cpu!
    if(bx_cpu)
    {
        cpu_ptr = dynamic_cast<XE_CPU_C*>(bx_cpu);
    }
    else
    {
        //        bx_cpu_ptr = bx_cpu = new BX_CPU_C();
        cpu_ptr = new XE_CPU_C();
        bx_cpu = cpu_ptr;
    }
    if(bx_mem)
    {
        mem_ptr = dynamic_cast<XE_MEM_C*>(bx_mem);
    }
    else
    {
        mem_ptr = new XE_MEM_C();
        bx_mem = mem_ptr;
    }
    
    

    Bit32u ips = 4000000;
    
//    bx_pc_system.setonoff(LOGLEV_DEBUG, ACT_REPORT);
    bx_pc_system.initialize(ips);
    
    mem_ptr->init_memory(memSize, hostMemSize);
//    mem_ptr->load_ROM(rom_path, 0, 0);
    
    cpu_ptr->initialize();
    cpu_ptr->sanity_checks();
    cpu_ptr->register_state();
//    cpu_ptr->setonoff(LOGLEV_DEBUG, ACT_REPORT);
    
    bx_pc_system.register_state();
    // will enable A20 line and reset CPU and devices
    bx_pc_system.Reset(BX_RESET_HARDWARE);
    cpu_ptr->xrstor_init_x87_state();
    
    
    load_elf(start_elf_file.c_str());
    
    struct pe_image32* pe;
    load_dll32(crt32_dll_file.c_str(),&pe);
    
    crt32_entry_addr = (bx_phy_address)pe->entry;
    assert(crt32_entry_addr!=0);
    
    call_win32_unknow_sym_addr = global_sym_tbl_win32["unknow_sym"];
    assert(call_win32_unknow_sym_addr!=0);
    
    call_host_ret_addr = global_sym_tbl["call_host_ret"];
    call_host_win32_ret_addr = global_sym_tbl_win32["call_host_ret"];
    HOST_CALL_PTR32_addr = global_sym_tbl_win32["HOST_CALL_PTR32"];
    
    
    LOG_INFO("call_host_func_addr:0x%0lx,win32:0x%0lx,host_call_ptr32:0x%0x\n",
              call_host_ret_addr,call_host_win32_ret_addr,
              HOST_CALL_PTR32_addr);
    
    //os init success!
    
    //init cls host call
    HostCallerBase::init();
    
    //here can load other dlls.
    
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
    
    g_engine->call_win32_guest_method1("test_dll3", 0);
    host_free((void*)_arg2);
//    ESP = EBP;
    ESP = pre_esp;
    
    return 0;
}

struct call_func
{
    WIN32_PTR name;
    WIN32_PTR pf;
};

extern uint64_t wrap_test_func(uint64_t* args);

void test_dll_func()
{

    printf("======test dll func =======\n");
    
    
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

    g_engine->load_dll32("test2.dll",nullptr,true);

    g_engine->call_win32_guest_method1("test_cpp", 0);
    
    
//    g_engine->load_dll32("libs/lua53_1.dll");
//    g_engine->load_dll32("testlua.dll");
    
    
//    g_engine->call_win32_guest_method1("testlua", 0);
//    printf("lua ret code:%d\n",int(g_engine->last_ret));
    
}

void Engine::run()
{
    
    cpu_ptr->push_64(call_host_ret_addr); //ret address call_host_func
    
    cpu_ptr->prev_rip = RIP = entry_addr;
#if BX_GDBSTUB
    // If using gdbstub, it will take control and call
    // bx_init_hardware() and cpu_loop()
    bx_dbg.gdbstub_enabled = 1;
    if (bx_dbg.gdbstub_enabled) bx_gdbstub_init();
#endif
    cpu_ptr->cpu_loop();//run in TiniyOs
    //call crt32 dll.
    
    call_win32_guest_method1("_crt32_pre_init", 0);
    call_win32_dll_entry(crt32_entry_addr); //call crt32.dll DllMain()
    
    
    load_base_dlls();
    

    
    test_dll_func();
}

void Engine::load_base_dlls()
{
    const char* base_dlls[] = {
        //win32 crt.
        "libs/msvcrt.dll",
        //mingw support dll.
        "libs/mingw/libwinpthread-1.dll",
        "libs/mingw/libgcc_s_sjlj-1.dll",
        //mingw c++ lib
        "libs/mingw/libstdc++-6.dll",
    };
    for(auto p:base_dlls)
    {
        load_dll32(p,nullptr,true);
    }
}

void Engine::call_guest_method1(const char* method,uint64_t arg1)
{
    auto it = global_sym_tbl.find(method);
    if(it!=global_sym_tbl.end())
    {
        bx_phy_address fun_ptr = it->second;
        RDI = arg1;
        
        cpu_ptr->push_64(call_host_ret_addr);
        
        RIP = fun_ptr;
//        RDI = fun_ptr;
//        RSI = arg1;
//        RIP = (Bit64u)call_guess_method->ptr;
        cpu_ptr->cpu_loop();
        
    }
    else
    {
        LOG_ERROR("can't find symbol :%s in global sym tbl!\n",method);
    }
}
//in win fast call
//arg1,arg2,arg3,arg4
//rcx,rdx,r8,r9
void Engine::call_win_guest_method1(const char* method,uint64_t arg1)
{
    auto pre_cpu_mode = cpu_ptr->cpu_mode;
    sw_cpu_mode(BX_MODE_LONG_64);
    
    auto it = global_sym_tbl.find(method);
    if(it!=global_sym_tbl.end())
    {
        bx_phy_address fun_ptr = it->second;
        RCX = arg1;
        
        cpu_ptr->push_64(call_host_ret_addr);
        
        RIP = fun_ptr;
        cpu_ptr->cpu_loop();
        
    }
    else
    {
        LOG_ERROR("can't find symbol :%s in global sym tbl!\n",method);
    }
    sw_cpu_mode(pre_cpu_mode);
}

void Engine::call_win32_guest_method1(const char* method,uint64_t arg1)
{
    auto pre_cpu_mode = cpu_ptr->cpu_mode;
    sw_cpu_mode(BX_MODE_LONG_COMPAT);
    
    auto it = global_sym_tbl_win32.find(method);
    if(it!=global_sym_tbl_win32.end())
    {
        bx_phy_address fun_ptr = it->second;
//        RCX = arg1;
        
//        cpu_ptr->push_64(call_host_ret_addr);
        cpu_ptr->push_32(call_host_win32_ret_addr);
        
        RIP = fun_ptr;
        push_call(fun_ptr);

        cpu_ptr->cpu_loop();
        
    }
    else
    {
        LOG_ERROR("can't find symbol :%s in global sym32bit tbl!\n",method);
    }
    sw_cpu_mode(pre_cpu_mode);
}
//BOOL WINAPI (*entry)(PVOID hinstDLL, DWORD fdwReason, PVOID lpvReserved);
void Engine::call_win32_dll_entry(bx_phy_address addr)
{
    auto pre_cpu_mode = cpu_ptr->cpu_mode;
    sw_cpu_mode(BX_MODE_LONG_COMPAT);
    
    bx_phy_address fun_ptr = addr;
    auto pre_esp = ESP;
    cpu_ptr->push_32(0);
    cpu_ptr->push_32(DLL_PROCESS_ATTACH);
    cpu_ptr->push_32(0);
    cpu_ptr->push_32(call_host_win32_ret_addr);
    
    RIP = fun_ptr;
    push_call(fun_ptr);
    
    cpu_ptr->cpu_loop();
    ESP = pre_esp;
    sw_cpu_mode(pre_cpu_mode);
}

void Engine::sw_cpu_mode(uint32_t mode)
{
    if(cpu_ptr->cpu_mode == BX_MODE_LONG_64 && mode == BX_MODE_LONG_COMPAT)
    {
        cpu_ptr->sregs[BX_SEG_REG_CS].cache.u.segment.l = 0; // 64bit
         cpu_ptr->handleCpuModeChange();
    }
    else if(cpu_ptr->cpu_mode == BX_MODE_LONG_COMPAT && mode==BX_MODE_LONG_64)
    {
        cpu_ptr->sregs[BX_SEG_REG_CS].cache.u.segment.l = 1; // 64bit
        cpu_ptr->handleCpuModeChange();
    }
}


void Engine::push_call(uint64_t addr)
{

    call_trace_win32.push_back(addr);

}
void Engine::pop_call()
{

    if(call_trace_win32.size()> 0)
        call_trace_win32.pop_back();
}

void Engine::print_call_trace_win32()
{
    printf("call trace:%d\n",call_trace_win32.size());
    for(auto addr:call_trace_win32)
    {
        const char* name = nullptr;
        auto it2 = global_addr2sym_win32.find(addr);
        if(it2!=global_addr2sym_win32.end())
        {
            name = it2->second->fname.c_str();
        }
        

        if(name==nullptr)
        {
            auto it = global_debug_info.find(addr);
            if(it!=global_debug_info.end())
            {
                name = it->second->name.c_str();
            }
            
        }
        printf("==addr:0x%0lx:%s\n",addr,name);
    }
}


