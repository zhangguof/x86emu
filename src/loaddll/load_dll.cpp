//
//  load_dll.cpp
//  x86emu
//
//  Created by tony on 2019/8/27.
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

#include "load_dll.hpp"
#include "logger.hpp"
#include "x86.h"
#include "loadelf/elf.h"
#include "loadelf/elf-ext.hpp"
#include <string>


#define RVA2VA(image, rva, type) (type)(ULONG_PTR)((char *)image + rva)


//get host mem
inline static Bit8u* getMemAddr(Bit64u addr)
{
    Bit8u* ret = BX_MEM(0)->getHostMemAddr(BX_CPU(0), addr, BX_RW);
    return ret;
}

//void load_RAM_from_data(Bit8u* data, Bit32u len, bx_phy_address ramaddress);
inline static void load_ram(Bit8u* data,Bit32u len,bx_phy_address ram_addr)
{
    BX_MEM(0)->load_RAM_from_data(data,len,ram_addr);
}
//dst == guest addr
inline void host_memcpy(char* guest_dst,char* src,Bit32u len)
{
    load_ram((Bit8u*)src, len, bx_phy_address(guest_dst));
}

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
static int fix_pe_image(struct pe_image *pe,dll* pdll,bx_phy_address base_addr)
{
    void *image;
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
    
    LOG_INFO("try load dll:base(0x%0lx),size(0x%0lx)\n",pe->opt_hdr->ImageBase,image_size);
    
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
    sect_hdr = IMAGE_FIRST_SECTION(pe->nt_hdr);
    
    LOG_DEBUG("copying headers: %u bytes", sect_hdr->PointerToRawData);
    
    host_memcpy((char*)image, (char*)pe->image, sect_hdr->PointerToRawData);
//    load_ram((Bit8u*)pe->image, (Bit32u)sect_hdr->PointerToRawData, (bx_phy_address)image);
    
    /* Copy all the sections */
    for (i = 0; i < sections; i++) {
        LOG_INFO("Copy section %s from %x to %x",
                  sect_hdr->Name, sect_hdr->PointerToRawData,
                  sect_hdr->VirtualAddress);
        if (sect_hdr->VirtualAddress+sect_hdr->SizeOfRawData >
            image_size) {
            ERROR("Invalid section %s in driver", sect_hdr->Name);
//            code_free(image);
            return -1;
        }
        
        host_memcpy((char*)image+sect_hdr->VirtualAddress,
               (char*)pe->image + sect_hdr->PointerToRawData,
               sect_hdr->SizeOfRawData);
        
        sect_hdr++;
    }
    
    // If the original is still there, clean it up.
    munmap(pe->image, pe->size);
    
    pe->image = image; //guest addr
    pe->size = image_size;
    
    /* Update our internal pointers */
    pe->nt_hdr = (IMAGE_NT_HEADERS *)
    ((char*)host_image+ ((IMAGE_DOS_HEADER *)host_image)->e_lfanew);
    pe->opt_hdr = &pe->nt_hdr->OptionalHeader;
    
    LOG_INFO("set nt headers: nt_hdr=%p, opt_hdr=%p, image=%p(%p)",
              pe->nt_hdr, pe->opt_hdr, pe->image,host_image);
    return image_size;
}

void * get_export_address(const char *name)
{
    void *address;
    if (get_export(name, &address) != -1)
        return address;
    return NULL;
}

int get_export(const char *name, void *result)
{
//    ENTRY key = { (char *)(name) }, *item;
//    int i, j;
//    void **func = result;
//
//    if (crtexports.size) {
//        if (hsearch_r(key, FIND, &item, &crtexports)) {
//            *func = item->data;
//            return 0;
//        }
//    }
//
//    if (extraexports.size) {
//        if (hsearch_r(key, FIND, &item, &extraexports)) {
//            *func = item->data;
//            return 0;
//        }
//    }
//
//    // Search the ndiswrapper crt
//    for (i = 0; crt_exports[i].name != NULL; i++) {
//        if (strcmp(crt_exports[i].name, name) == 0) {
//            *func = crt_exports[i].func;
//            return 0;
//        }
//    }
//
//    // Search PE exports
//    for (i = 0; i < num_pe_exports; i++)
//        if (strcmp(pe_exports[i].name, name) == 0) {
//            *func = pe_exports[i].addr;
//            return 0;
//        }
    
    void **func = (void**)result;
    auto it = global_sym_tbl.find(name);
    if(it!=global_sym_tbl.end())
    {
        *func = (void*) it->second;
        return 0;
    }
    
    return -1;
}

static int import(void *image, IMAGE_IMPORT_DESCRIPTOR *dirent, char *dll)
{
    ULONG_PTR *lookup_tbl, *address_tbl;
    char *symname = NULL;
    int i;
    int ret = 0;
    generic_func adr;
    void* host_image = getMemAddr((bx_phy_address)image);
    
    lookup_tbl = RVA2VA(host_image, dirent->u.OriginalFirstThunk, ULONG_PTR *);
    address_tbl = RVA2VA(host_image, dirent->FirstThunk, bx_phy_address *);
    
    for (i = 0; lookup_tbl[i]; i++) {
        if (IMAGE_SNAP_BY_ORDINAL(lookup_tbl[i])) {
            ERROR("ordinal import not supported: %llu", (uint64_t)lookup_tbl[i]);
            address_tbl[i] = (bx_phy_address) ordinal_import_stub;
            continue;
        }
        else {
            symname = RVA2VA(host_image, ((lookup_tbl[i] & ~IMAGE_ORDINAL_FLAG) + 2), char *);
        }
        
        if (get_export(symname, &adr) < 0) {
            ERROR("unknown symbol: %s:%s", dll, symname);
            address_tbl[i] = (bx_phy_address) unknown_symbol_stub;
            continue;
        } else {
            LOG_DEBUG("found symbol: %s:%s: addr: %p, rva = %llu",
                      dll, symname, adr, (uint64_t)address_tbl[i]);
            address_tbl[i] = (bx_phy_address)adr;
        }
    }
    
    return 0;
}

static int read_exports(struct pe_image *pe)
{
    IMAGE_EXPORT_DIRECTORY *export_dir_table;
    int i;
    uint32_t *name_table;
    uint16_t *ordinal_table;
    PIMAGE_OPTIONAL_HEADER opt_hdr;
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
        
        LOG_DEBUG("export symbol: %s, at %p",
                  (char *)((char*)host_image + *name_table),
                  (char*)pe->image + address);
        
//        pe_exports[num_pe_exports].dll = pe->name;
//        pe_exports[num_pe_exports].name = pe->image + *name_table;
//        pe_exports[num_pe_exports].addr = pe->image + address;
        std::string name = (char*)host_image + *name_table;
        global_sym_tbl[name] = (bx_phy_address)pe->image + address;
        
//        num_pe_exports++;
        name_table++;
        ordinal_table++;
    }
    return 0;
}

static int fixup_imports(void *image, IMAGE_NT_HEADERS *nt_hdr)
{
    int i;
    char *name;
    int ret = 0;
    IMAGE_IMPORT_DESCRIPTOR *dirent;
    IMAGE_DATA_DIRECTORY *import_data_dir;
    PIMAGE_OPTIONAL_HEADER opt_hdr;
    void* host_image = getMemAddr((bx_phy_address)image);
    
    opt_hdr = &nt_hdr->OptionalHeader;
    import_data_dir =
    &opt_hdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    dirent = RVA2VA(host_image, import_data_dir->VirtualAddress,
                    IMAGE_IMPORT_DESCRIPTOR *);
    
    for (i = 0; dirent[i].Name; i++) {
        name = RVA2VA(host_image, dirent[i].Name, char*);
        
        LOG_DEBUG("imports from dll: %s", name);
        ret += import(image, &dirent[i], name);
    }
    return ret;
}

static int fixup_reloc(void *image, IMAGE_NT_HEADERS *nt_hdr)
{
    ULONG_PTR base;
    ULONG_PTR size;
    IMAGE_BASE_RELOCATION *fixup_block;
    IMAGE_DATA_DIRECTORY *base_reloc_data_dir;
    PIMAGE_OPTIONAL_HEADER opt_hdr;
    
    opt_hdr = &nt_hdr->OptionalHeader;
    base = opt_hdr->ImageBase;
    base_reloc_data_dir =
    &opt_hdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    if (base_reloc_data_dir->Size == 0)
        return 0;
    void* host_iamge = getMemAddr((bx_phy_address)image);
    
    fixup_block = RVA2VA(host_iamge, base_reloc_data_dir->VirtualAddress,
                         IMAGE_BASE_RELOCATION *);
    LOG_DEBUG("fixup_block=%p, image=%p", fixup_block, image);
    LOG_DEBUG("fixup_block info: %x %d",
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
int link_pe_image_in_host(struct pe_image *pe_image, bx_phy_address* base_addr,dll* pdll)
{
   struct pe_image* pe = nullptr;
    
    IMAGE_DOS_HEADER *dos_hdr;
    pe = pe_image;
    dos_hdr = (IMAGE_DOS_HEADER*)pe->image;
    if (pe->size < sizeof(IMAGE_DOS_HEADER)) {
        LOG_ERROR("image too small: %d", pe->size);
        return -1;
    }
    
    pe->nt_hdr =
    (IMAGE_NT_HEADERS *)((char*)pe->image + dos_hdr->e_lfanew);
    pe->opt_hdr = &pe->nt_hdr->OptionalHeader;
    
    pe->type = check_nt_hdr64(pe->nt_hdr);
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

int try_load_dll64(const char* dll_path,struct pe_image** pe)
{
//    size_t size = 0;
    *pe = new pe_image;
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

