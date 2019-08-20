//
//  linux_sys_call.hpp
//  x86emu
//
//  Created by tony on 2019/8/20.
//  Copyright © 2019 tony. All rights reserved.
//

//#ifndef linux_sys_call_h
//#define linux_sys_call_h
//x86-64 linux sys call

DEF_SYSCALL(0,read )
DEF_SYSCALL(1,write)
DEF_SYSCALL(2,open )
DEF_SYSCALL(3,close)
DEF_SYSCALL(4,stat )
DEF_SYSCALL(5,fstat)
DEF_SYSCALL(6,lstat)
DEF_SYSCALL(7,poll )



//#endif /* linux_sys_call_h */
