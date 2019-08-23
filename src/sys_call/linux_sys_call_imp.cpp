//
//  linux_sys_call.cpp
//  x86emu
//
//  Created by tony on 2019/8/20.
//  Copyright © 2019 tony. All rights reserved.
//
#define NEED_CPU_REG_SHORTCUTS 1
#include <stdio.h>
//imp linux sys call;
#include "bochs/bochs.h"

#include "cpu/x86.h"
#include "sys_call/host_sys_call.hpp"


//Bit64 sys_read(unsigned int fd,char *buf,size_t count)
//{
//
//}

inline Bit8u* getMemAddr(Bit64u addr)
{
    Bit8u* ret = BX_MEM(0)->getHostMemAddr(BX_CPU(0), addr, BX_RW);
    return ret;
}

Bit64u sys_write(Bit64u idx,SYS_CALL_ARGS args)
{
    auto fd = static_cast<unsigned int>(args[0]);
//    const char *buf
//    size_t count
    auto buf =reinterpret_cast<const char*>(getMemAddr(args[1]));
    auto count = static_cast<size_t>(args[2]);
    
    auto ret = write(fd,buf,count);
    return ret;
}

