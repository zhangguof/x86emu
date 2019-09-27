//
//  WrapCls.hpp
//  x86emu
//
//  Created by tony on 2019/9/27.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef WrapCls_hpp
#define WrapCls_hpp

#include <stdint.h>
#include "WrapPointer.hpp"

struct BaseWrapCls
{
    uint8_t* host_ptr;
    uint32_t guest_addr;
    BaseWrapCls(uint8_t*p, uint32_t addr):host_ptr(p),guest_addr(addr){}
    BaseWrapCls():host_ptr(nullptr),guest_addr(0){}
    uint32_t getaddr() const
    {
        return guest_addr;
    }
};

template<typename T>
inline T& get_data(uint8_t* ptr,uint32_t off)
{
    T& ret = *(T*)(ptr+off);
    //    off+=sizeof(T);
    return ret;
}


template<typename T>
inline WrapPointer<T> get_pointer(uint8_t* ptr,uint32_t off)
{
    auto addr = (uint32_t*)(ptr+off);
    WrapPointer<T> ret(*addr);
    //    off += sizeof(WIN32_PTR);
    ret.set_ref(addr);
    return ret;
}

template<typename T>
inline WrapPointer<T> get_array(uint8_t* ptr,uint32_t off)
{
    //    auto ret = *(T*)(ptr+off);
    WrapPointer<T> ret((void*)(ptr+off));
    //    off += sizeof(WIN32_PTR);
    return ret;
}




#endif /* WrapCls_hpp */
