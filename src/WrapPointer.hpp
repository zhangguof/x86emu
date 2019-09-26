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


//template<typename T>
struct BaseWrapPointer
{
    uint32_t win32adr;
    uint32_t* ref_win32addr;
    uint8_t* host_ptr;
    void set_addr(uint32_t addr)
    {
        win32adr = addr;
//        ref_win32addr = 0;
        host_ptr = getMemAddr(addr);
    }
    BaseWrapPointer(uint32_t addr)
    {
        set_addr(addr);
        ref_win32addr = 0;
    }
    BaseWrapPointer(uint32_t* addr)
    {
        set_addr(*addr);
        ref_win32addr = addr;
    }
    BaseWrapPointer(const BaseWrapPointer& p)
    {
        this->win32adr = p.win32adr;
        this->host_ptr = p.host_ptr;
        this->ref_win32addr = p.ref_win32addr;
    }

    BaseWrapPointer& operator=(uint32_t addr)
    {
        set_addr(addr);
        if(ref_win32addr) *ref_win32addr = addr;
        return *this;
    }
};
template<typename T>
struct WrapPointer:public BaseWrapPointer
{
    WrapPointer(uint32_t addr):BaseWrapPointer(addr){}
    WrapPointer(uint32_t* addr):BaseWrapPointer(addr){}
    T* get() const
    {
        return (T*)host_ptr;
    }
    T& operator*() const
    {
        return *(T*)host_ptr;
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
    WrapPointer(uint32_t addr):BaseWrapPointer(addr){}
    WrapPointer(uint32_t* addr):BaseWrapPointer(addr){}
    T** get() const
    {
        return (T**)host_ptr;
    }
    WrapPointer<T> operator*() const
    {
        return WrapPointer<T>((uint32_t*)host_ptr);
    }
    WrapPointer<T> operator[](int i) const
    {
        auto addr = (uint32_t*)host_ptr + i;
        
        return WrapPointer<T>(addr);
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

#endif /* WrapPointer_hpp */
