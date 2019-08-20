//
//  host_sys_call.hpp
//  x86emu
//
//  Created by tony on 2019/8/20.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef host_sys_call_hpp
#define host_sys_call_hpp

#include <stdio.h>
#include "bochs/bochs.h"

typedef Bit64u SYS_CALL_ARGS[6];
extern Bit64u host_sys_call();
extern Bit64u do_host_sys_call(SYS_CALL_ARGS args);



#endif /* host_sys_call_hpp */
