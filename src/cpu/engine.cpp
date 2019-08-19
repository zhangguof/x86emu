//
//  engine.cpp
//  x86emu
//
//  Created by tony on 2019/8/19.
//  Copyright © 2019 tony. All rights reserved.
//

#define NEED_CPU_REG_SHORTCUTS 1

#include "cpu/x86.h"
#include "elf.h"
#include "mmu.hpp"

#include "engine.hpp"

// g_cpu = NULL;
extern void bx_init_hardware();
void load_elf_bin(const char* path,uint8_t **pdata,uint32_t &size);
extern void print_elf_info(Elf64_Ehdr*);

#define RUN_BASE_ADDR (0x400000) //4M start.
const Bit64u PAGE_BASE_ADDR = (0x100000);

#define BX_CR3_PAGING_MASK    (BX_CONST64(0x000ffffffffff000))

#define BX_CR3_PAGING_MASK    (BX_CONST64(0x000ffffffffff000))

void Engine::load_elf(const char* elf_file)
{
    // The RESET function will have been called first.
    // Set CPU and memory features which are assumed at this point.
    
    //    bx_load_kernel_image(SIM->get_param_string(BXPN_LOAD32BITOS_PATH)->getptr(), 0x100000);
    Bit8u* p_elf_data = NULL;
    
    Bit32u filesz = 0;
    load_elf_bin(elf_file,&p_elf_data,filesz);
    Elf64_Ehdr* p_elf_h = (Elf64_Ehdr*) p_elf_data;
    print_elf_info(p_elf_h);
    bx_phy_address base_addr = RUN_BASE_ADDR; //2M start.
    bx_phy_address entry_addr = p_elf_h->e_entry;
    
    mem_ptr->load_RAM_from_data(p_elf_data,filesz,base_addr);
    
    cpu_ptr->prev_rip = cpu_ptr->gen_reg[BX_64BIT_REG_RIP].rrx = entry_addr;
    
    setup_os_env();
}


void Engine::setup_os_env()
{
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
    cpu_ptr->cr4.set_PAE(1);
    
    //load page
    mem_ptr->alloca_pagebit64(PAGE_BASE_ADDR);
    Bit64u cr3_val = (PAGE_BASE_ADDR & BX_CR3_PAGING_MASK);
    
    
    cpu_ptr->efer.set_LME(1);
    cpu_ptr->efer.set_LMA(1);
    
    cpu_ptr->handleCpuModeChange();
    cpu_ptr->SetCR3(cr3_val);
    
}

void Engine::init()
{
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
    
    
    Bit64u memSize = 32*1024*1024; //32M
    Bit64u hostMemSize = 32*1023*1024;
    const char* rom_path = "/Users/tony/workspace/github/x86emu/tool/BIOS-bochs-latest";
    const char* elf_file = "/Users/tony/workspace/github/x86emu/TiniyOs/tiniy";
    
    Bit32u ips = 4000000;
    
    bx_pc_system.setonoff(LOGLEV_DEBUG, ACT_REPORT);
    bx_pc_system.initialize(ips);
    
    mem_ptr->init_memory(memSize, hostMemSize);
    mem_ptr->load_ROM(rom_path, 0, 0);
    
    cpu_ptr->initialize();
    cpu_ptr->sanity_checks();
    cpu_ptr->register_state();
    cpu_ptr->setonoff(LOGLEV_DEBUG, ACT_REPORT);
    
    bx_pc_system.register_state();
    // will enable A20 line and reset CPU and devices
    bx_pc_system.Reset(BX_RESET_HARDWARE);
    load_elf(elf_file);
    
    // First load the system BIOS (VGABIOS loading moved to the vga code)
    //    SIM->get_param_string(BXPN_ROM_PATH)->set(rom_path);
    //    bx_init_hardware();
    
}

void Engine::run()
{
    cpu_ptr->cpu_loop();
}
