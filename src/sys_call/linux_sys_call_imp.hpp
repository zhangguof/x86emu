//
//  linux_sys_call_imp.hpp
//  x86emu
//
//  Created by tony on 2019/8/20.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef linux_sys_call_imp_h
#define linux_sys_call_imp_h

#ifndef IMP_SYS_CALL
#define IMP_SYS_CALL(idx,name)
#endif

extern Bit64u sys_write(Bit64u idx,SYS_CALL_ARGS args);

IMP_SYS_CALL(1,sys_write)

#endif /* linux_sys_call_imp_h */
