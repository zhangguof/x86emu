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

int try_load_dll64(const char* dll_path,struct pe_image** pe);
void * get_export_address(const char *name);

int get_export(const char *name, void *result);

#endif /* load_dll_hpp */
