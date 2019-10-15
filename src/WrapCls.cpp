//
//  WrapCls.cpp
//  x86emu
//
//  Created by tony on 2019/9/27.
//  Copyright © 2019 tony. All rights reserved.
//

#include "WrapCls.hpp"
#include "wrap_host_call.hpp"


//a func imp in host, call by guest
BaseWrapFunction::BaseWrapFunction(wrap_static_func_ptr pf)
{
    guest_addr = new_wrap_func(pf, "");
    is_host_imp = true;
}
//a func imp in guest, call by host.
BaseWrapFunction::BaseWrapFunction(uint32_t f_addr)
{
    guest_addr = f_addr;
    is_host_imp = false;
}
