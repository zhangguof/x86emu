//
//  mmu.cpp
//  x86emu
//
//  Created by tony on 2019/8/18.
//  Copyright © 2019 tony. All rights reserved.
//

#include "mmu.hpp"
#include "x86.h"
//memory manage unit
const Bit32u PAGE_SIZE = 4*1024 ; //4kb page size
const Bit32u PDE_NUM = (1<<9) ; //page directory entry max num;

//PML4E-> PDPTE -> PDE -> PTE -> 4k page
#define GET_NEXT_PAGE(addr) \
    addr = (addr&0xFFF)?(((addr>>12)+1)<<12):addr;




void XE_MEM_C::alloca_pagebit64(Bit64u start_addr)
{
    Bit64u mem_size = this->get_memory_len();
    Bit32u npte = mem_size / PAGE_SIZE;
    
    //L3 idx page table entry
    PTE ptes[npte];
    Bit64u base_addr = 0;
    PTE tmp_pte;
    tmp_pte.set64(0);
    tmp_pte.set_P(1);
    tmp_pte.set_RW(1);
    tmp_pte.set_US(1);
    tmp_pte.set_A(1);
    
    for(int i = 0;i<npte;++i)
    {
        base_addr = i*PAGE_SIZE;
        tmp_pte.set_phaddr(base_addr);
        ptes[i].set64(tmp_pte.get64());
    }
    
    //L2 idx page directory  entry
    Bit32u npde = (npte + PDE_NUM - 1) / PDE_NUM;
    PDE pdes[npde];
    PDE tmp_pde;
    tmp_pde.set64(0);
    tmp_pde.set_P(1);
    tmp_pde.set_RW(1);
    tmp_pde.set_US(1);
    tmp_pde.set_A(1);
    
    //L1 idx page directory point table entry
    Bit32u npdpte = (npde + PDE_NUM - 1) / PDE_NUM;
    PDPTE pdptes[npdpte];
    PDPTE tmp_pdpte;
    tmp_pdpte.set64(0);
    tmp_pdpte.set_P(1);
    tmp_pdpte.set_RW(1);
    tmp_pdpte.set_US(1);
    tmp_pdpte.set_A(1);
    
    //L0 idx PML4E
    Bit32u npml4e = (npdpte + PDE_NUM - 1) / PDE_NUM;
    assert(npml4e == 1); //now for 1
    PML4E pml4es[npml4e];
    PML4E tmp_pml4e;
    tmp_pml4e.set64(0);
    tmp_pml4e.set_P(1);
    tmp_pml4e.set_RW(1);
    tmp_pml4e.set_US(1);
    tmp_pml4e.set_A(1);
    
    //copy to guess memory
    
    
    Bit64u offest = start_addr + sizeof(PML4E)*npml4e;
    GET_NEXT_PAGE(offest);
   
    for(int i=0;i<npml4e;++i)
    {
        tmp_pml4e.set_phaddr(offest);
        pml4es[i].set64(tmp_pml4e.get64());
        
        offest += sizeof(PDPTE)*PDE_NUM;
    }
    assert((offest&0xfff)==0);
    
    
    Bit64u guest_offest = start_addr;
    
    Bit8u* data = (Bit8u*)pml4es;
    Bit32u len = sizeof(PML4E)*npml4e;
    this->load_RAM_from_data(data,len,guest_offest);
    guest_offest += len;
    GET_NEXT_PAGE(guest_offest);
//    if(guest_offest % PAGE_SIZE!=0)
//    {
//        guest_offest = (guest_offest/PAGE_SIZE + 1)*PAGE_SIZE;
//    }
    
    
    
    for(int i=0;i<npdpte;++i)
    {
        tmp_pdpte.set_phaddr(offest);
        pdptes[i].set64(tmp_pdpte.get64());
        offest += sizeof(PDE) * PDE_NUM;
    }
    assert((offest & 0xfff) == 0);
    
    data = (Bit8u*) pdptes;
    len = sizeof(PDPTE) * npdpte;
    this->load_RAM_from_data(data, len, guest_offest);
    guest_offest += len;
    GET_NEXT_PAGE(guest_offest);
    
    //128M for 64 here.
    for(int i=0;i<npde;++i)
    {
        tmp_pde.set_phaddr(offest);
        pdes[i].set64(tmp_pde.get64());
        offest += sizeof(PTE) * PDE_NUM;
    }
    data = (Bit8u*) pdes;
    len = sizeof(PDE) * npde;
    this->load_RAM_from_data(data, len, guest_offest);
    guest_offest += len;
    GET_NEXT_PAGE(guest_offest);
    
    data = (Bit8u*) ptes;
    len = sizeof(PTE) * npte;
    this->load_RAM_from_data(data, len, guest_offest);
    
    //set cr3
    
}
