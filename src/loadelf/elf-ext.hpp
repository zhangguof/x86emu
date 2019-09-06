//
//  elf-ext.hpp
//  x86emu
//
//  Created by tony on 2019/8/24.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef elf_ext_h
#define elf_ext_h
#include "buffer.hpp"
#include <functional>
#include "bochs/bochs.h"
#include <algorithm>
#include <unordered_map>

typedef Elf64_Ehdr ehdr;


//#define GNU_HASH_BUCKET_LEN(p)    (*((p)))
//#define GNU_HASH_BUCKET(p,n)    ((p)[(n)+1])
//#define GNU_HASH_CHAIN(p,n)    ((p)+(1+GNU_HASH_BUCKET_LEN(p)+(n)))

typedef uint64_t bloom_el_t;
#define ELFCLASS_BITS 64

//The first 4 32-bit words are:
//nbuckets    symndx        maskwords    shift2
struct gnu_hash_table_h{
    uint32_t nbuckets;
    uint32_t symoffset;
    uint32_t bloom_size;
    uint32_t bloom_shift;
};
struct gnu_hash_table;

//typedef void (*walk_hash_tbl_func)(uint32_t symix);
typedef std::function<void(uint32_t symix,uint32_t hash)> walk_hash_tbl_func;
struct gnu_hash_table
{
    struct gnu_hash_table_h* tbl_h;
    bloom_el_t *bloom; /* uint32_t for 32-bit binaries */
    uint32_t *buckets;
    uint32_t *chain;
    gnu_hash_table(uint8_t* hashtab);
    Elf64_Sym* lookup_sym(const char* name,Elf64_Sym* dyn_sym_tab,const char* str_tab);
    void print_all_hash(Elf64_Sym* dyn_sym_tab,const char* str_tab);
    void walk_all_hash(walk_hash_tbl_func fun_ptr);


};

struct dll_dyn
{
    Elf64_Dyn* p_dyn;
    uint32_t dn;
    gnu_hash_table* g_hash_table;
    Elf64_Sym* dynsym;
    const char* strtab;
    Elf64_Rela* rela_got;
    uint32_t nrela;
    Elf64_Rela* rela_plt;
    uint32_t nplt;
    dll_dyn():p_dyn(nullptr),dn(0),g_hash_table(nullptr),
    dynsym(nullptr),strtab(nullptr),
    rela_got(nullptr),nrela(0),rela_plt(nullptr),nplt(0){}
};

struct Needlist
{
    struct Needlist* next;
    const char* name;
    Needlist():next(nullptr),name(nullptr){}
};

struct dll {
    struct dll* next;
    ehdr* e;
    void* code,* data;
    void* host_code, *host_data;
    size_t codelen,datalen,codeplus;
    
//    Elf64_Dyn* p_dyn;
//    uint32_t dn;
//    gnu_hash_table* g_hash_table;
    struct dll_dyn* dyn;
    struct Needlist* need_list;
//    bool is_so;
    uint64_t vaddr_base; // so file adjust
    char name[1];        // asciiz of library name
    dll():next(nullptr),e(nullptr),
    code(nullptr),data(nullptr),host_code(nullptr),host_data(nullptr),
    codelen(0),datalen(0),codeplus(0),
    dyn(nullptr),need_list(nullptr),vaddr_base(0)
//    p_dyn(nullptr),dn(0),g_hash_table(nullptr)
    {}
    ~dll(){
        if(dyn)
        {
            delete dyn;
            dyn = nullptr;
        }
    }
};

class ELF
{
    typedef Elf64_Ehdr Elf_Ehdr;
    typedef Elf64_Dyn Elf_Dyn;
    typedef Elf64_Sym Elf_Sym;
    
};

void load_elf_bin(const char*path, BufPtr& pdata);
void print_elf_info(Elf64_Ehdr* p_elfh);

dll* load_lib(ehdr* eh,bx_phy_address *base_addr,bool is_so = false);
void load_dyn(dll* p_dll);
dll* try_load_so(const char* name,bx_phy_address* base_addr, bool is_so=false);

extern std::unordered_map<std::string, bx_phy_address> global_sym_tbl;
extern std::unordered_map<std::string, bx_phy_address> global_sym_tbl_win32;
extern std::unordered_map<std::string, bx_phy_address> global_sym_tbl_win64;


static bx_phy_address get_next_page(bx_phy_address addr)
{
    //4k page alignment 1<<12
    const uint64_t align = 0x1000;
    if((addr & (align-1)) != 0)
    {
        //next page
        addr = (addr + align) & (~(align-1));
    }
    return addr;
}

#endif /* elf_ext_h */
