//
//  WrapPointer.hpp
//  x86emu
//
//  Created by tony on 2019/9/26.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef WrapPointer_hpp
#define WrapPointer_hpp
#include <stdint.h>
#include "engine.hpp"

//extern uint8_t* getMemAddr(uint64_t addr);

extern uint32_t new_cosnt_buf(const char* str);

struct BaseWrapPointer
{
    uint32_t win32addr;//for debug
    uint32_t* ref_win32addr;
    uint8_t* host_ptr;
    
    void set_addr(uint32_t addr);
    BaseWrapPointer();
    BaseWrapPointer(uint32_t addr);
    BaseWrapPointer(void* addr);
    BaseWrapPointer(const BaseWrapPointer& p);
    
    void set_ref(uint32_t* addr)
    {
        ref_win32addr = addr;
    }
//    BaseWrapPointer& operator=(uint32_t addr);
};

template<typename T>
struct WrapPointer:public BaseWrapPointer
{
    WrapPointer():BaseWrapPointer(){}
    WrapPointer(uint32_t addr):BaseWrapPointer(addr){}
    WrapPointer(void* ptr):BaseWrapPointer(ptr){}
    
    T* get() const
    {
        return (T*)host_ptr;
    }
    T& operator*() const
    {
        return *(T*)host_ptr;
    }
    T& operator[](int i) const
    {
        auto addr = (T*)host_ptr + i;
        return *addr;
    }
    WrapPointer& operator=(uint32_t addr)
    {
        set_addr(addr);
        if(ref_win32addr) *ref_win32addr = addr;
        return *this;
    }
    WrapPointer& operator=(const char* str)
    {
        //        add = host_mall
        uint32_t addr = new_cosnt_buf(str);
        set_addr(addr);
        if(ref_win32addr) *ref_win32addr = addr;
        return *this;
    }
};

//like int** p;
//p = WrapPointer<int*>;
//*p == WraPointer<int>(*p)
template<typename T>
struct WrapPointer<T*>:public BaseWrapPointer
{
    WrapPointer():BaseWrapPointer(){}
    WrapPointer(uint32_t addr):BaseWrapPointer(addr){}
    WrapPointer(void* ptr):BaseWrapPointer(ptr){}
    
    T** get() const
    {
        return (T**)host_ptr;
    }
    WrapPointer<T> operator*() const
    {
        auto ret=WrapPointer<T>(*(uint32_t*)host_ptr);
        ret.set_ref((uint32_t*)host_ptr);
    }
    WrapPointer<T> operator[](int i) const
    {
        auto addr = (uint32_t*)host_ptr + i;
        
        auto ret =  WrapPointer<T>(*addr);
        ret.set_ref(addr);
        return ret;
    }
    WrapPointer& operator=(uint32_t addr)
    {
        set_addr(addr);
        if(ref_win32addr) *ref_win32addr = addr;
        return *this;
    }
    
    WrapPointer& operator=(const char* str)
    {
        uint32_t addr = new_cosnt_buf(str);
        set_addr(addr);
        if(ref_win32addr) *ref_win32addr = addr;
        return *this;
    }
};

#endif /* WrapPointer_hpp */
