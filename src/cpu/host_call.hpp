//
//  host_call.hpp
//  x86emu
//
//  Created by tony on 2019/9/4.
//  Copyright © 2019 tony. All rights reserved.
//

//#ifndef host_call_h
//#define host_call_h

//#define DEF_HOST_FUNC(func,f) \
//{\
//(void*)func,#func,wrap_##funct \
//},

DEF_HOST_FUNC(do_ret)
DEF_HOST_FUNC(puts)
DEF_HOST_FUNC(host_malloc)
DEF_HOST_FUNC(host_free)

//#undefine DEF_HOST_FUNC



//#endif /* host_call_h */
