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
const Bit64u DLL_LAOD_BASE =  RUN_BASE_ADDR + 0x200000; //load dll,so
Bit64u g_dll_next_ptr = DLL_LAOD_BASE;

const Bit64u BASE_HEAP_ADDR_END = g_dll_next_ptr;

#define BX_CR3_PAGING_MASK    (BX_CONST64(0x000ffffffffff000))

#define BX_CR3_PAGING_MASK    (BX_CONST64(0x000ffffffffff000))

std::string work_home = "/Users/tony/workspace/github/x86emu/TiniyOs/";
std::string g_so_path = work_home;
std::string start_elf_file = "tiniyos.elf";

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

void Engine::init()
{
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
    
    

//    const char* rom_path = "/Users/tony/workspace/github/x86emu/tool/BIOS-bochs-latest";
//    const char* elf_file = "/Users/tony/workspace/github/x86emu/TiniyOs/tiniy";
//    const char* elf_file = "/ldso";

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
    load_elf(start_elf_file.c_str());
    
    // First load the system BIOS (VGABIOS loading moved to the vga code)
    //    SIM->get_param_string(BXPN_ROM_PATH)->set(rom_path);
    //    bx_init_hardware();
    
    
    
}

void Engine::run()
{
//   cpu_ptr->prev_rip = cpu_ptr->gen_reg[BX_64BIT_REG_RIP].rrx = entry_addr;
    call_host_ret_addr = global_sym_tbl["call_host_ret"];
    printf("call_host_func_addr:0x%0lx\n",call_host_ret_addr);
    cpu_ptr->push_64(call_host_ret_addr); //ret address call_host_func
    
    cpu_ptr->prev_rip = RIP = entry_addr;
//    cpu_ptr->PUSH
    cpu_ptr->cpu_loop();
    

    call_guest_method1("test_pow2",100);
    printf("100*100=%d\n",(int)last_ret);
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
        printf("can't find symbol :%s in global sym tbl!\n",method);
    }
}


