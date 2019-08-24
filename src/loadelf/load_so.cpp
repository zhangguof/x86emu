
#include "stdio.h"
#include "string.h"
#include "assert.h"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include "cpu/x86.h"
//#include "bochs/bochs.h"
#include <algorithm>
#include <unordered_map>
#include <string>

#define ELFTEST
#include "elf.h"
#include "elf-ext.hpp" //gnu_hash_table
#include "buffer.hpp"

extern std::string g_so_path;



std::unordered_map<const char*, bx_phy_address> global_sym_tbl;


//get host mem
inline Bit8u* getMemAddr(Bit64u addr)
{
    Bit8u* ret = BX_MEM(0)->getHostMemAddr(BX_CPU(0), addr, BX_RW);
    return ret;
}

//void load_RAM_from_data(Bit8u* data, Bit32u len, bx_phy_address ramaddress);
inline void load_ram(Bit8u* data,Bit32u len,bx_phy_address ram_addr)
{
    BX_MEM(0)->load_RAM_from_data(data,len,ram_addr);
}

uint32_t gnu_hash(const char* name) {
    uint32_t h = 5381;
    
    for (; *name; name++) {
        h = (h << 5) + h + *name;
    }
    
    return h;
}

static dll *dlls=nullptr, *dllroot=nullptr;

#define PRINT_ATTR(name,ptr) \
printf(#name":0x%0llx\n",(ptr)->name);

void print_sym_info(Elf64_Sym* sym)
{
    PRINT_ATTR(st_name,sym);        /* Symbol name, index in string tbl */
    PRINT_ATTR(st_info,sym);    /* Type and binding attributes */
    PRINT_ATTR(st_other,sym);    /* No defined meaning, 0 */
    PRINT_ATTR(st_shndx,sym);        /* Associated section index */
    PRINT_ATTR(st_value,sym);        /* Value of the symbol */
    PRINT_ATTR(st_size,sym);        /* Associated symbol size */
    
}

const char* get_tag_name(uint32_t tag)
{
    size_t n = sizeof(dtag)/sizeof(lookup);
    lookup t = {tag,"0"};
    auto ite = std::lower_bound(dtag,dtag+n,t,
                                [](const lookup& a,const lookup& b){
                                    return a.n < b.n;
                                });
    return ite->s;
}
const char* get_rtype_name(uint32_t tag)
{
    size_t n = sizeof(rtype)/sizeof(lookup);
    lookup t = {tag,"0"};
    auto ite = std::lower_bound(rtype,rtype+n,t,
                                [](const lookup& a,const lookup& b){
                                    return a.n < b.n;
                                });
    return ite->s;
}

inline uint8_t* get_addr(uint64_t vaddr, dll* pd)
{
    return (uint8_t*) ((uint8_t*)pd->host_code + ((uint8_t*)vaddr - (uint8_t*)(pd->code)));
}

gnu_hash_table::gnu_hash_table(uint8_t* hashtab){
    tbl_h = ((gnu_hash_table_h*)(hashtab));
    hashtab+= sizeof(gnu_hash_table_h);
    bloom = (bloom_el_t*)hashtab;
    buckets = (uint32_t*)(bloom + tbl_h->bloom_size);
    chain = buckets + tbl_h->nbuckets;
}

Elf64_Sym* gnu_hash_table::lookup_sym(const char* name,Elf64_Sym* dyn_sym_tab,const char* str_tab)
{
    uint32_t namehash = gnu_hash(name);
    uint32_t symix = buckets[namehash % tbl_h->nbuckets];
    //do bloom check
    bloom_el_t word = bloom[(namehash / ELFCLASS_BITS) % tbl_h->bloom_size];
    bloom_el_t mask = 0
    | (bloom_el_t)1 << (namehash % ELFCLASS_BITS)
    | (bloom_el_t)1 << ((namehash >> tbl_h->bloom_shift) % ELFCLASS_BITS);
        
    /* If at least one bit is not set, a symbol is surely missing. */
    if ((word & mask) != mask) {
        return nullptr;
    }
    
    if (symix < tbl_h->symoffset) {
        return nullptr;
    }
    /* Loop through the chain. */
    while (true) {
        const char* symname = str_tab + dyn_sym_tab[symix].st_name;
        const uint32_t hash = chain[symix - tbl_h->symoffset];
        
        if ((namehash|1) == (hash|1) && strcmp(name, symname) == 0) {
            return dyn_sym_tab + symix;
        }
        
        /* Chain ends with an element with the lowest bit set to 1. */
        if (hash & 1) {
            break;
        }
        
        symix++;
    }
        
    return nullptr;
}


void gnu_hash_table::walk_all_hash(walk_hash_tbl_func fun_ptr)
{
    for(int i = 0;i<tbl_h->nbuckets;++i){
        /* Loop through the chain. */
        uint32_t symix = buckets[i];
        while (true) {
//            const char* symname = str_tab + dyn_sym_tab[symix].st_name;
            if(symix < tbl_h->symoffset)
                break;
            const uint32_t hash = chain[symix - tbl_h->symoffset];
//            printf("#%d,name:%s,hash:%0x\n",symix,symname,hash);
            fun_ptr(symix,hash);
            /* Chain ends with an element with the lowest bit set to 1. */
            if (hash & 1) {
                break;
            }
            symix++;
        }
    }
}
void gnu_hash_table::print_all_hash(Elf64_Sym* dyn_sym_tab,const char* str_tab)
{
    walk_all_hash([&dyn_sym_tab,&str_tab](uint32_t symix,uint32_t hash){
        const char* symname = str_tab + dyn_sym_tab[symix].st_name;
        printf("#%d,name:%s,hash:%0x\n",symix,symname,hash);
    });
}




void load_hash_info(uint8_t* hashtab,Elf64_Sym* dyn_sym_tab,const char* str_tab)
{
    
    gnu_hash_table hash_table((uint8_t*)hashtab);
    
    hash_table.print_all_hash(dyn_sym_tab, str_tab);
    
//    //check name;
//    const char *names[]={
//        "test_f1",
//        "printf",
//        "test_f2",
//        "a",
//        "aaa",
//        "test"
//    };
//    for(int i=0;i<sizeof(names)/sizeof(const char*);++i)
//    {
//        auto sym = hash_table.lookup_sym(names[i], dyn_sym_tab, str_tab);
//        if(sym)
//        {
//            printf("find %s,in :%d,val: %0llx\n",names[i],sym->st_shndx,sym->st_value);
//        }
//        else{
//            printf("%s:not found!\n",names[i]);
//        }
//    }
    
}

#define get_dyn(pd,name) (pd)->dyn.name

void load_global_sym_tbl(dll* p_dll)
{
    Elf64_Sym* dynsym = get_dyn(p_dll, dynsym);
    const char* strtab = get_dyn(p_dll, strtab);
    if(get_dyn(p_dll, g_hash_table) != nullptr)
    {
        p_dll->dyn.g_hash_table->walk_all_hash([&dynsym, &strtab](uint64_t symix, uint32_t hash){
            const char* name = strtab + dynsym[symix].st_name;
//            global_sym_tbl[name] = dynsym+symix;
            global_sym_tbl[name] = dynsym[symix].st_value;
        });
    }
}

const size_t max_needed = 32;
void try_load_so(const char* name)
{
    std::string so_path = g_so_path + name;
    const char* c_path = so_path.c_str();
//    uint8_t* pdata = nullptr;
    uint32_t len = 0;

    
}

void load_dyn(dll* p_dll)
{
//    if(p_dll->dyn.p_dyn != nullptr) return;
    
//    int dn = p_dll->dyn.dn;
//    Elf64_Dyn *pdyn = p_dll->dyn.p_dyn;
    int dn = get_dyn(p_dll, dn);
    Elf64_Dyn* pdyn = get_dyn(p_dll, p_dyn);
    
    uint8_t* buf = (uint8_t*)p_dll->host_code;
    
    printf("==loading dyn head:%d\n",dn);
    Elf64_Sym* dy_sym = nullptr;
    char* dy_strtab = nullptr;
    uint8_t* gnu_hash_tab = nullptr;
    
    Elf64_Rela* rela = nullptr; // section .rela.dyn, rel for .got
    Elf64_Rela* plt_rela = nullptr; //section .rela.plt rel for .plt
    int rnum = 0; //rel num;
    int plt_rnum = 0; //plt rel num;
    uint32_t needed_n = 0;
    uint32_t needed_idxs[max_needed]; //need idx
    
    

    for(int i=0;i<dn;++i)
    {
        Elf64_Dyn* p = pdyn + i;
        printf("==dyn:#%d,tag:%s(0x%0x),d_val:0x%0x\n",i,
               get_tag_name(p->d_tag),p->d_tag,p->d_un.d_val);
        switch(p->d_tag)
        {
            case DT_STRTAB:
//                dy_strtab = (char*)(buf + p->d_un.d_ptr);
                dy_strtab = (char*)get_addr(p->d_un.d_ptr, p_dll);
//                p_dll->dyn.strtab = dy_strtab;
                get_dyn(p_dll, strtab) = dy_strtab;
                break;
            case DT_SYMTAB:
//                dy_sym = (Elf64_Sym*)(buf + p->d_un.d_ptr);
                dy_sym = (Elf64_Sym*)get_addr(p->d_un.d_ptr, p_dll);
//                p_dll->dyn.dynsym = dy_sym;
                get_dyn(p_dll, dynsym) = dy_sym;
                break;
            case DT_RELA:
//                rela = (Elf64_Rela*)(buf + p->d_un.d_ptr);
                rela = (Elf64_Rela*)get_addr(p->d_un.d_ptr, p_dll);
                get_dyn(p_dll, rela_got) = rela;
                break;
            case DT_JMPREL:
                //                plt_rela = (Elf64_Rela*)(buf + p->d_un.d_ptr);
                plt_rela = (Elf64_Rela*)get_addr(p->d_un.d_ptr, p_dll);
                get_dyn(p_dll, rela_plt) = plt_rela;
                break;
            case DT_RELASZ:
                rnum = p->d_un.d_val / (sizeof(Elf64_Rela));
                get_dyn(p_dll, nrela) = rnum;
                break;
            case DT_PLTRELSZ:
                plt_rnum = p->d_un.d_val / (sizeof(Elf64_Rela));
                get_dyn(p_dll, nplt) = plt_rnum;
                break;
            case DT_GNU_HASH:
                gnu_hash_tab = (uint8_t*)get_addr(p->d_un.d_ptr, p_dll);
                get_dyn(p_dll, g_hash_table) = new gnu_hash_table(gnu_hash_tab);
                break;
            case DT_NEEDED:
                needed_idxs[needed_n++] = p->d_un.d_val;
//                needed_name = dy_strtab+needed_idx;
                break;
//            case DT_SONAME:
            case DT_NULL:
            default:
                break;
        }
        if(p->d_tag == DT_NULL) break;
    }
    Needlist* last = nullptr;
    for(size_t i =0;i<needed_n;++i)
    {
        auto need_list = new Needlist;
        need_list->next = nullptr;
        need_list->name = dy_strtab + needed_idxs[i];
        if(p_dll->need_list==nullptr)
        {
            p_dll->need_list = need_list;
            last = need_list;
        }
        else{
            last->next = need_list;
            last = need_list;
        }
        printf("NEEDED SO:%s\n",dy_strtab + needed_idxs[i]);
    }
//    printf("===rel sym num:%d,sizeof:%d\n",rnum,sizeof(Elf64_Rela));
//    for(int i=0;i<rnum;++i)
//    {
//        Elf64_Rela* r = rela + i;
//        auto sym = dy_sym[ELF64_R_SYM(r->r_info)];
//        uint32_t r_type = ELF64_R_TYPE(r->r_info);
//        printf("=====rel:#%d,addr:r_offset:%0x,r_type:%s(0x%0x),name:%s,r_addend:0x%0x\n",
//               i,r->r_offset,get_rtype_name(r_type),r_type,
//               dy_strtab+sym.st_name,r->r_addend);
//        print_sym_info(&sym);
//
//    }
//
//    for(int i=0;i<plt_rnum;++i)
//    {
//        Elf64_Rela* r = plt_rela + i;
//        auto sym = dy_sym[ELF64_R_SYM(r->r_info)];
//        uint32_t r_type = ELF64_R_TYPE(r->r_info);
//        printf("=====plt:#%d,addr:r_offset:%0x,r_type:%s(0x%0x),name:%s,r_addend:0x%0x\n",
//               i,r->r_offset,get_rtype_name(r_type),r_type,
//               dy_strtab+sym.st_name,r->r_addend);
//        print_sym_info(&sym);
//
//    }
    p_dll->dyn.g_hash_table->print_all_hash(dy_sym, dy_strtab);
    load_global_sym_tbl(p_dll);
}

dll* load_lib(ehdr* eh,bx_phy_address base_addr)
{
    //load from file buf
    uint8_t* buf = (uint8_t*) eh;
    dll* pdll = new dll;
    pdll->next =  nullptr;
    int phnum = eh->e_phnum;
    Elf64_Phdr* ph = (Elf64_Phdr*)(buf + eh->e_phoff);
    
    int dn = 0;
    Elf64_Dyn* pdyn = nullptr;
    
    for(int i=0;i<phnum;++i)
    {
        auto p = ph + i;
        if(p->p_type == PT_LOAD)
        {
            load_ram(buf+p->p_offset, p->p_filesz, p->p_vaddr);
            Bit8u* host_mem = getMemAddr(p->p_vaddr);
            if(pdll->code == nullptr)
            {
                pdll->code = (void*)p->p_vaddr;
                pdll->codelen = p->p_memsz;
                pdll->host_code = host_mem;
            }
            else if(pdll->data == nullptr)
            {
                pdll->data = (void*)p->p_vaddr;
                pdll->datalen = p->p_memsz;
                pdll->host_data = host_mem;
            }
            printf("ELF loading:%0x,size:%d\n",p->p_vaddr,p->p_filesz);
                
        }
        else if(p->p_type == PT_DYNAMIC)
        {
            Bit8u* host_mem = getMemAddr(p->p_vaddr);
            dn = p->p_memsz / sizeof(Elf64_Dyn);
//            pdyn = (Elf64_Dyn*)(host_mem);
            pdyn = (Elf64_Dyn*)(buf + p->p_offset);
//            pdll->p_dyn = pdyn;
//            pdll->dn = dn;
            pdll->dyn.p_dyn = pdyn;
            pdll->dyn.dn = dn;
        }
    }
    if(dllroot==nullptr)
    {
        dllroot = pdll;
        dlls = pdll;
    }
    else
    {
//        auto tmp = dlls;
        dlls->next = pdll;
        dlls = pdll;
    }
    if(pdll->dyn.p_dyn)
    {
        load_dyn(pdll);
    }
    
    return pdll;
}