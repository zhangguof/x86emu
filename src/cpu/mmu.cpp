//
//  mmu.cpp
//  x86emu
//
//  Created by tony on 2019/8/18.
//  Copyright © 2019 tony. All rights reserved.
//

#include "mmu.hpp"
#include "x86.h"
#include "engine.hpp"
#include "logger.hpp"
#include <algorithm>
#include <unordered_map>
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
    tmp_pte.set_phaddr(0);
    //first page can't access
    ptes[0].set64(tmp_pte.get64());
    
    tmp_pte.set_P(1);
    tmp_pte.set_RW(1);
    tmp_pte.set_US(1);
    tmp_pte.set_A(1);
    
    for(int i = 1;i<npte;++i)
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

static bx_phy_address heap_start_addr = 0;
static bx_phy_address heap_end_addr = 0;

//less than 2kB
//min=1<<5 = 32B mask 0xf
const uint32_t mini_mem[]={1<<5,1<<6,1<<7,1<<8,1<<9,1<<10,1<<11};
const uint32_t max_mem_idx = sizeof(mini_mem)/sizeof(uint32_t) - 1;
const uint64_t max_free_mem_size = 1<<11;
alloc_t* _free_mem[max_mem_idx+1];
alloc_t* _any_free_mem; //any size free mem
//alloc_t* _used_mem[max_mem_idx+1];
//guest mem addr -> guest mem addr size
std::unordered_map<void*, Bit64u> _used_mem;
const Bit64u min_alloc_size = 0x20; //32B

inline Bit64u GETSIZE(Bit64u size)
{
    if((size & (~(min_alloc_size-1))) == size)
        return size;
    return (size+(min_alloc_size-1))&(~(min_alloc_size-1));
}

alloc_t* find_next_insert(alloc_t* free,Bit64u size)
{
    auto tmp = free;
//    if(tmp && tmp->size >= size) return tmp;
    assert(tmp && tmp->size < size);
    while(tmp && tmp->next)
    {
        if(tmp->next->size >= size)
            break;
        tmp = tmp->next;
    }
    return tmp;
}

void print_any_free_list()
{
    printf("===any_free_list=====\n");
    auto tmp = _any_free_mem;
    while(tmp)
    {
        printf("0x%0lx(%lu)->",tmp->vaddr,tmp->size);
        tmp = tmp->next;
    }
    printf("\n");
}

//sorted list!
void add_any_free_mem(alloc_t* host_ptr)
{
    alloc_t* f = _any_free_mem;
    if(f==nullptr || f->size >= host_ptr->size)
    {
        host_ptr->next = f;
        _any_free_mem = host_ptr;
    }
    else
    {
        auto tmp = find_next_insert(_any_free_mem, host_ptr->size);
        //insert in tmp->next
        assert(tmp->size < host_ptr->size);
        assert(tmp->next == nullptr || tmp->next->size >= host_ptr->size);
        
        host_ptr->next = tmp->next;
        tmp->next = host_ptr;
    }
    
}

void init_mem_allocate(bx_phy_address start, bx_phy_address end)
{
    assert(start< end);
    heap_start_addr = start;
    heap_end_addr = end;
    int n = sizeof(mini_mem) / sizeof(uint32_t);
    for(int i=0;i<n;++i)
    {
        _free_mem[i] =  nullptr;
    }
    _used_mem.clear();
    _any_free_mem = nullptr;
}

uint32_t get_mem_idx(Bit64u size)
{
    auto idx = std::lower_bound(mini_mem, mini_mem+max_mem_idx+1, size);
    assert(size <= mini_mem[*idx]);
    return idx - mini_mem;
}

void* do_guest_mem_allocate(Bit64u size)
{
    if(heap_start_addr + size < heap_end_addr)
    {
        void* ret = (void*) heap_start_addr;
        heap_start_addr += size;
        LOG_DEBUG("===guest_mem allocate:0x%0lx,%lu\n",ret,size);
        return ret;
    }
    LOG_ERROR("!!!!Full memory!!!\n");
    return nullptr;
}

void* host_malloc(Bit64u size)
{
    size = GETSIZE(size);
    
    if(size <= max_free_mem_size)
    {
        auto idx = get_mem_idx(size);
        alloc_t* free_ptr = _free_mem[idx];
        void* ptr = nullptr;
        if(free_ptr == nullptr)
        {
//            free_ptr =do_guest_mem_allocate
            ptr = do_guest_mem_allocate(size);
        }
        else
        {
            alloc_t* next = free_ptr->next;
            _free_mem[idx] = next;
            ptr = (void*)free_ptr->vaddr;
        }
        _used_mem[ptr] = size;
        LOG_DEBUG("[host_malloc]0x%0lx,%u\n",ptr,size);
        return ptr;
    }
    
    void* ptr = nullptr;
    do{
        //remove list
//        print_any_free_list();
        
        if(_any_free_mem)
        {
            alloc_t* f = _any_free_mem;
            if(f->size >= size)
            {
                _any_free_mem = f->next;
                ptr = (void*)f->vaddr;
                break;
//                _used_mem[ptr] = size;
//                return ptr;
            }
            else
            {
                f = find_next_insert(_any_free_mem, size);
                assert(f->size < size);
                if(f->next == nullptr)
                    break;
                assert(f->next->size >= size);
                auto tmp = f->next;
                f->next = tmp->next;
                ptr = (void*)tmp->vaddr;
                break;
//                _used_mem[ptr] = size;
//                return ptr;
            }
        }
    }while(0);
    
    if(ptr==nullptr)
        ptr = do_guest_mem_allocate(size);
    
    _used_mem[ptr] = size;
    LOG_DEBUG("[host_malloc]0x%0lx,%u\n",ptr,size);
    return ptr;
}
void host_free(void* ptr)
{
    auto p = _used_mem.find(ptr);
    if(p==_used_mem.end()||_used_mem[ptr]==0)
    {
        LOG_ERROR("error not malloc mem!!!\n");
        return;
    }
    auto size = _used_mem[ptr];
    alloc_t* host_ptr = (alloc_t*)g_engine->getHostMemAddr((bx_phy_address)ptr);
    
    host_ptr->size = size;
    host_ptr->vaddr = (bx_phy_address) ptr;
    if(size<=max_free_mem_size)
    {
        auto idx = get_mem_idx(size);
        alloc_t* f = _free_mem[idx];
        host_ptr->next = f;
        _free_mem[idx] = host_ptr;
    }
    else
    {
        add_any_free_mem(host_ptr);
//        print_any_free_list();
    }
    _used_mem[ptr] = 0;
    LOG_DEBUG("[host_free]0x%0lx,%u\n",ptr,size);
}

void* host_realloc(void* ptr, Bit64u new_size)
{
    if(ptr == nullptr)
    {
        return host_malloc(new_size);
    }
    auto p = _used_mem.find(ptr);
    if(p==_used_mem.end()||_used_mem[ptr]==0)
    {
        LOG_ERROR("error not malloc mem!!!\n");
        return nullptr;
    }
    Bit64u old_size = p->second;
    if(old_size <= new_size) return ptr;
    
    void* new_ptr = host_malloc(new_size);
    void* ptr_dst = getMemAddr((bx_phy_address)new_ptr);
    void* ptr_src = getMemAddr((bx_phy_address)ptr);
    memcpy(ptr_dst, ptr_src, old_size);
    host_free(ptr);
    
    return new_ptr;
    
}
