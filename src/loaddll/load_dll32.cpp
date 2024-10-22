//
//  load_dll32.cpp
//  x86emu
//
//  Created by tony on 2019/9/6.
//  Copyright © 2019 tony. All rights reserved.
//

extern "C"
{
    
#include "util.h"
#include "winnt_types.h"
#include "pe_linker.h"
#include "ntoskernel.h"
#include <sys/mman.h>
#include <err.h>
    
}


#include "logger.hpp"
#include "x86.h"
#include "loadelf/elf.h"
#include "loadelf/elf-ext.hpp"
#include <string>
#include "load_dll.hpp"
#include "engine.hpp"
#include "wrap_host_call.hpp"

#include "debug.hpp"


#define RVA2VA(image, rva, type) (type)(ULONG_PTR64)((char *)image + rva)

#define EXPORT_DLL_SYMBOL 0

static void ordinal_import_stub(void)
{
    warnx("function at %p attempted to call a symbol imported by ordinal", __builtin_return_address(0));
    __debugbreak();
}

static void unknown_symbol_stub(void)
{
    warnx("function at %p attempted to call an unknown symbol", __builtin_return_address(0));
    __debugbreak();
}



//copy to guest mem
static int fix_pe_image(struct pe_image32 *pe,dll* pdll,bx_phy_address base_addr)
{
    void *image; //guest addr
    IMAGE_SECTION_HEADER *sect_hdr;
    int i, sections;
    int image_size;
    
    
    if (pe->size == pe->opt_hdr->SizeOfImage) {
        /* Nothing to do */
        return 0;
    }
    
    image_size = pe->opt_hdr->SizeOfImage;
    pdll->code = (void*)base_addr;
    pdll->codelen = image_size;
    pdll->host_code = getMemAddr(base_addr);
    image = pdll->code;
    void *host_image = pdll->host_code;
    
    LOG_INFO("try load 32bit dll:%s,base(0x%0x),size(0x%0x),load in(0x%0x)\n",
             pe->name,
             pe->opt_hdr->ImageBase,
             image_size,(uint64_t)pdll->code);
    
    //    image      = code_malloc(image_size + getpagesize());
    
    // Round to page size?
    //image      = (PVOID)(ROUND_UP((ULONG)(image), getpagesize()));
    
    if (host_image == NULL) {
        ERROR("failed to allocate enough space for new image: %d bytes, %m", image_size);
        return -1;
    }
    //    memset(host_image, 0, image_size);
    
    /* Copy all the headers, ie everything before the first section. */
    
    sections = pe->nt_hdr->FileHeader.NumberOfSections;
    sect_hdr = IMAGE_FIRST_SECTION64(pe->nt_hdr);
    
    LOG_INFO("copying headers: %u bytes", sect_hdr->PointerToRawData);
    
    host_memcpy((char*)image, (char*)pe->image, sect_hdr->PointerToRawData);
    //    load_ram((Bit8u*)pe->image, (Bit32u)sect_hdr->PointerToRawData, (bx_phy_address)image);
    
    /* Copy all the sections */
    for (i = 0; i < sections; i++) {
        LOG_INFO("Copy section %s from %x to %x (%d)",
                 sect_hdr->Name, sect_hdr->PointerToRawData,
                 sect_hdr->VirtualAddress,sect_hdr->SizeOfRawData);
        if (sect_hdr->VirtualAddress+sect_hdr->SizeOfRawData >
            image_size) {
            ERROR("Invalid section %s in driver", sect_hdr->Name);
            //            code_free(image);
            return -1;
        }
        if(sect_hdr->SizeOfRawData==0)
        {
            char* dst = (char*)image + sect_hdr->VirtualAddress;
            dst = (char*)getMemAddr((bx_phy_address)dst);
            memset(dst, 0, sect_hdr->Misc.VirtualSize);
        }
        else
        {
            host_memcpy((char*)image+sect_hdr->VirtualAddress,
                        (char*)pe->image + sect_hdr->PointerToRawData,
                        sect_hdr->SizeOfRawData);
        }

        
        
        sect_hdr++;
    }
    
    // If the original is still there, clean it up.
    munmap(pe->image, pe->size);
    
    pe->image = image; //guest addr
    pe->size = image_size;
    
    
    
    /* Update our internal pointers */
    pe->nt_hdr = (IMAGE_NT_HEADERS32 *)
    ((char*)host_image+ ((IMAGE_DOS_HEADER *)host_image)->e_lfanew);
    pe->opt_hdr = &pe->nt_hdr->OptionalHeader;
    
    pe->entry = (void*)((bx_phy_address)image + pe->opt_hdr->AddressOfEntryPoint);
    
    LOG_INFO("set nt headers: nt_hdr=%p, opt_hdr=%p, image=%p(%p)",
             pe->nt_hdr, pe->opt_hdr, pe->image,host_image);
    return image_size;
}

void * get_export_address32(const char *name)
{
    void *address;
    if (get_export32(name, &address) != -1)
        return address;
    return NULL;
}

int get_export32(const char *name, void *result)
{
    
    void **func = (void**)result;
    auto it = global_sym_tbl_win32.find(name);
    if(it!=global_sym_tbl_win32.end())
    {
        *func = (void*) it->second;
        return 0;
    }
    *func = nullptr;
    return -1;
}

int add_dll_export32(const char* name,uint32_t addr,
                      const char* dll_name,uint64_t dll_base,
                      uint64_t load_base)
{
    auto it = global_sym_tbl_win32.find(name);
    if(it==global_sym_tbl_win32.end())
    {
        global_sym_tbl_win32[name] = addr;
        global_addr2sym_win32[addr] = std::make_shared<sym_info>(dll_name,name,dll_base,load_base);
        return 1;
    }
    LOG_DEBUG("has add export symbol32 (%s) 0x%0x\n",name,addr);
    return 0;
}

void add_export32(const char* name,uint32_t addr)
{
    auto ret  = add_dll_export32(name, addr, "Host32.dll", 0, 0);
    if(ret)
    {
        LOG_DEBUG("HostDLL export32:%s,0x%0x\n",name,addr);
    }
}

extern void* host_malloc(Bit64u size);
static void* gen_unknow_info(const char* dll_name, const char* fname, bx_phy_address fun_addr)
{
    void* ptr = host_malloc(32);
    uint8_t* host_ptr = getMemAddr((bx_phy_address)ptr);
    uint32_t idx = unknow_sym_tbl.size();
    unknow_sym_tbl.push_back(unknow_sym_info(dll_name,fname));
    gen_unkown_code(host_ptr, idx, fun_addr, true);
    
    return ptr;
}

static int import(void *image, IMAGE_IMPORT_DESCRIPTOR *dirent, char *dll)
{
    ULONG_PTR32 *lookup_tbl, *address_tbl;
    char *symname = NULL;
    int i;
    int ret = 0;
    generic_func adr;
    void* host_image = getMemAddr((bx_phy_address)image);
    
    lookup_tbl = RVA2VA(host_image, dirent->u.OriginalFirstThunk, ULONG_PTR32 *);
    address_tbl = RVA2VA(host_image, dirent->FirstThunk, ULONG_PTR32 *);
    auto unknow_sym_addr = g_engine->call_win32_unknow_sym_addr;
    
    for (i = 0; lookup_tbl[i]; i++) {
        if (IMAGE_SNAP_BY_ORDINAL32(lookup_tbl[i])) {
            ERROR("ordinal import not supported: %llu", (uint64_t)lookup_tbl[i]);
            address_tbl[i] = (bx_phy_address) ordinal_import_stub;
            continue;
        }
        else {
            symname = RVA2VA(host_image, ((lookup_tbl[i] & ~IMAGE_ORDINAL_FLAG32) + 2), char *);
        }
        
        if (get_export32(symname, &adr) < 0) {
            LOG_WARN("unknown symbol: %s:%s", dll, symname);
     
            address_tbl[i] = (bx_phy_address) 0;
            if(unknow_sym_addr)
            {
                uint64_t new_addr = (uint64_t)gen_unknow_info(dll, symname,unknow_sym_addr);
//                global_sym_tbl_win32[symname] = new_addr;
//                global_addr2sym_win32[new_addr] = std::make_shared<sym_info>(dll,symname,0,0);
                add_dll_export32(symname, new_addr, dll, 0, 0);
//
                address_tbl[i] = (uint32_t)new_addr;

                
            }
            continue;
        } else {
            LOG_INFO("found symbol: %s:%s: addr: %p, rva = %llu",
                      dll, symname, adr, (uint64_t)address_tbl[i]);
            address_tbl[i] = (bx_phy_address)adr;
        }
    }
    
    return 0;
}

static void get_path(struct pe_image32 *pe)
{
    const char* path = pe->name;
    const char* p = strrchr(path, '/');
//    const char* name = p+1;
    std::string dll_name = p+1;
    std::string dll_path = std::string(path,p - path);
    strcpy(pe->name, dll_name.c_str());
    strcpy(pe->dll_path, dll_path.c_str());
    
//    printf("%s,%s\n",dll_path.c_str(),dll_name.c_str());
//    return dll_path.c_str();
    
    
    
}
static void write_file(FILE* s, char* ptr, size_t size)
{
    fwrite(ptr, 1, size, s);
}

static int read_exports(struct pe_image32 *pe)
{
    get_path(pe);
#if EXPORT_DLL_SYMBOL
    const char* ext = strrchr(pe->name, '.');
    std::string name = std::string(pe->name,ext - pe->name);
    name = name + ".txt";
    std::string path = pe->dll_path + name;
    printf("try to open %s\n",path.c_str());
    auto f = fopen(path.c_str(), "w");
    
#endif
    
    
    IMAGE_EXPORT_DIRECTORY *export_dir_table;
    int i;
    uint32_t *name_table;
    uint16_t *ordinal_table;
    PIMAGE_OPTIONAL_HEADER32 opt_hdr;
    IMAGE_DATA_DIRECTORY *export_data_dir;
    
    Bit8u* host_image = getMemAddr(Bit64u(pe->image));
    
    opt_hdr = &pe->nt_hdr->OptionalHeader;
    export_data_dir =
    &opt_hdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    
    if (export_data_dir->Size == 0) {
        LOG_DEBUG("no exports");
        return 0;
    }
    
    export_dir_table =
    RVA2VA(host_image, export_data_dir->VirtualAddress,
           IMAGE_EXPORT_DIRECTORY *);
    
    
    
    name_table = (unsigned int *)(host_image +
                                  export_dir_table->AddressOfNames);
    ordinal_table = (uint16_t *)(host_image +
                                 export_dir_table->AddressOfNameOrdinals);
    
    //    pe_exports = calloc(export_dir_table->NumberOfNames, sizeof(struct pe_exports));
    
    for (i = 0; i < export_dir_table->NumberOfNames; i++) {
        //guest address
        uint32_t address = ((uint32_t *) ((char*)host_image + export_dir_table->AddressOfFunctions))[*ordinal_table];
        
        if (export_data_dir->VirtualAddress <= address ||
            address >= (export_data_dir->VirtualAddress +
                        export_data_dir->Size)) {
                //DBGLINKER("forwarder rva");
            }
        
        LOG_INFO("export symbol: %s, at %p",
                  (char *)((char*)host_image + *name_table),
                  (char*)pe->image + address);
        
#if EXPORT_DLL_SYMBOL
//        char buf[1024];
        std::string pname = (char *)((char*)host_image + *name_table);
        pname.push_back('\n');
        write_file(f,(char*) pname.c_str(), pname.size());
#endif
        

        std::string name = (char*)host_image + *name_table;
        bx_phy_address addr = (bx_phy_address)pe->image + address;
        auto it = global_sym_tbl_win32.find(name);
        if(it == global_sym_tbl_win32.end()||it->second ==
           g_engine->call_win32_unknow_sym_addr)
        {

            add_dll_export32(name.c_str(), addr, pe->name,
                             pe->opt_hdr->ImageBase, (uint64_t)pe->image);
            
            LOG_INFO("read symbol:%s:%s\n",pe->name,name.c_str());
        }
        else
        {
            LOG_WARN("has imported symbol:%s in :0x%0llx\n",name.c_str(),it->second);
        }
        
        //        num_pe_exports++;
        name_table++;
        ordinal_table++;
    }
#if EXPORT_DLL_SYMBOL
    if(f)
        fclose(f);
#endif
    return 0;
}

static int fixup_imports(void *image, IMAGE_NT_HEADERS32 *nt_hdr)
{
    int i;
    char *name;
    int ret = 0;
    IMAGE_IMPORT_DESCRIPTOR *dirent;
    IMAGE_DATA_DIRECTORY *import_data_dir;
    PIMAGE_OPTIONAL_HEADER32 opt_hdr;
    void* host_image = getMemAddr((bx_phy_address)image);
    
    opt_hdr = &nt_hdr->OptionalHeader;
    import_data_dir =
    &opt_hdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    dirent = RVA2VA(host_image, import_data_dir->VirtualAddress,
                    IMAGE_IMPORT_DESCRIPTOR *);
    
    for (i = 0; dirent[i].Name; i++) {
        name = RVA2VA(host_image, dirent[i].Name, char*);
        
//        LOG_DEBUG("imports from dll: %s", name);
        ret += import(image, &dirent[i], name);
    }
    return ret;
}

static int fixup_reloc(void *image, IMAGE_NT_HEADERS32 *nt_hdr)
{
    ULONG_PTR32 base;
    ULONG_PTR32 size;
    IMAGE_BASE_RELOCATION *fixup_block;
    IMAGE_DATA_DIRECTORY *base_reloc_data_dir;
    PIMAGE_OPTIONAL_HEADER32 opt_hdr;
    
    opt_hdr = &nt_hdr->OptionalHeader;
    base = opt_hdr->ImageBase;
    base_reloc_data_dir =
    &opt_hdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    if (base_reloc_data_dir->Size == 0)
        return 0;
    void* host_iamge = getMemAddr((bx_phy_address)image);
    
    fixup_block = RVA2VA(host_iamge, base_reloc_data_dir->VirtualAddress,
                         IMAGE_BASE_RELOCATION *);
    LOG_INFO("fixup_block=%p, image=%p", fixup_block, image);
    LOG_INFO("fixup_block info: %x %d",
              fixup_block->VirtualAddress, fixup_block->SizeOfBlock);
    
    while (fixup_block->SizeOfBlock) {
        int i;
        WORD fixup, offset;
        
        size = (fixup_block->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
        
        for (i = 0; i < size; i++) {
            fixup = fixup_block->TypeOffset[i];
            offset = fixup & 0xfff;
            switch ((fixup >> 12) & 0x0f) {
                case IMAGE_REL_BASED_ABSOLUTE:
                    break;
                    
                case IMAGE_REL_BASED_HIGHLOW: {
                    uint32_t addr;
                    uint32_t *loc =
                    RVA2VA(host_iamge,
                           fixup_block->VirtualAddress +
                           offset, uint32_t *);
                    addr = RVA2VA(image, (*loc - base), uint32_t);
                    *loc = addr;
                }
                    break;
                    
                case IMAGE_REL_BASED_DIR64: {
                    uint64_t addr;
                    uint64_t *loc =
                    RVA2VA(host_iamge,
                           fixup_block->VirtualAddress +
                           offset, uint64_t *);
                    addr = RVA2VA(image, (*loc - base), uint64_t);
                    LOG_DEBUG("relocation: *%p (Val:%llX)= %llx",
                              loc, *loc, addr);
                    *loc = addr;
                }
                    break;
                    
                default:
                    ERROR("unknown fixup: %08X",
                          (fixup >> 12) & 0x0f);
                    return -EOPNOTSUPP;
                    break;
            }
        }
        
        fixup_block = (IMAGE_BASE_RELOCATION *)
        ((char *)fixup_block + fixup_block->SizeOfBlock);
    };
    
    return 0;
}

// fix_pe_image->read_export->fix_reloc->fix_import
static int link_pe_image_in_host(struct pe_image32 *pe_image, bx_phy_address* base_addr,dll* pdll)
{
    struct pe_image32* pe = nullptr;
    
    IMAGE_DOS_HEADER *dos_hdr;
    pe = pe_image;
    dos_hdr = (IMAGE_DOS_HEADER*)pe->image;
    if (pe->size < sizeof(IMAGE_DOS_HEADER)) {
        LOG_ERROR("image too small: %d", pe->size);
        return -1;
    }
    
    pe->nt_hdr =
    (IMAGE_NT_HEADERS32 *)((char*)pe->image + dos_hdr->e_lfanew);
    pe->opt_hdr = &pe->nt_hdr->OptionalHeader;
    
    pe->type = check_nt_hdr32(pe->nt_hdr);
    if (pe->type <= 0) {
        LOG_ERROR("type <= 0 (%0llu)",pe->type);
        return -1;
    }
    
    //    auto size = link_pe_image_in_host(pe, base_addr, pdll);
    int size = fix_pe_image(pe, pdll, *base_addr);
    if(size>0)
    {
        bx_phy_address next = *base_addr + size;
        next = get_next_page(next);
        *base_addr = next;
        LOG_DEBUG("next dll base:0x%0lx\n",*base_addr);
    }
    else
    {
        LOG_ERROR("link pe image error!\n");
        return 0;
    }
    read_exports(pe);
    if (fixup_reloc(pe->image, pe->nt_hdr)) {
        LOG_ERROR("fixup reloc failed");
        return -1;
    }
    if (fixup_imports(pe->image, pe->nt_hdr)) {
        LOG_ERROR("fixup imports failed");
        return -1;
    }
    
    return 0;
}

extern bx_phy_address g_dll_next_ptr;

int try_load_dll32(const char* dll_path,struct pe_image32** pe)
{
    //    size_t size = 0;
    *pe = new pe_image32;
    auto image = *pe;
    //    (*pe)->name = dll_path;
    strcpy(image->name, dll_path);
    
    // Load the mpengine module.
    if (pe_load_library(dll_path, &(image->image), &(image->size)) == false) {
        LOG_ERROR("load dll fail!");
        return 1;
    }
    dll* pdll = new dll;
    pdll->next = nullptr;
    link_pe_image_in_host(image, &g_dll_next_ptr, pdll);
    
    load_debug_info(dll_path, image->opt_hdr->ImageBase, (uint64_t)pdll->code);
    
    return 0;
}


#ifdef TESTLOADER
int main()
{
    get_logger(Logger::LV_INFO);
    LOG_INFO("test load dll!!");
    // void* image = nullptr;
    size_t size = 0;
    const char* name = "/Users/tony/workspace/github/x86emu/test/dll/test.dll";
    pe_image image = {
        .entry  = NULL,
        .name   = "test.dll",
    };
    
    // Load the mpengine module.
    if (pe_load_library(name, &image.image, &image.size) == false) {
        LOG_INFO("load dll fail!");
        return 1;
    }
    
    // Handle relocations, imports, etc.
    link_pe_images(&image, 1);
    
    
    
    return 0;
}

#endif

