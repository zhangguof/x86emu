//
//  load_dll.hpp
//  x86emu
//
//  Created by tony on 2019/8/27.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef load_dll_hpp
#define load_dll_hpp

#include <stdio.h>

#include "loaddll/winnt_types.h"
#include "loaddll/pe_linker.h"
#include "loaddll/ntoskernel.h"
//#include "codealloc.h"
#include "loaddll/util.h"

//get host mem
//inline static Bit8u* getMemAddr(Bit64u addr)
//{
//    Bit8u* ret = BX_MEM(0)->getHostMemAddr(BX_CPU(0), addr, BX_RW);
//    return ret;
//}

//void load_RAM_from_data(Bit8u* data, Bit32u len, bx_phy_address ramaddress);
//inline static void load_ram(Bit8u* data,Bit32u len,bx_phy_address ram_addr)
//{
//    BX_MEM(0)->load_RAM_from_data(data,len,ram_addr);
//}
//dst == guest addr
//inline static void host_memcpy(char* guest_dst,char* src,Bit32u len)
//{
//    load_ram((Bit8u*)src, len, bx_phy_address(guest_dst));
//}

struct unkonw_sym_table
{
    uint32_t idx;
    
};


int try_load_dll64(const char* dll_path,struct pe_image** pe);
int try_load_dll32(const char* dll_path,struct pe_image32** pe);

void * get_export_address32(const char *name);
void * get_export_address64(const char *name);


int get_export32(const char *name, void *result);
int get_export64(const char *name, void *result);


#endif /* load_dll_hpp */
