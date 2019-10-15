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
#include "wrap_host_call.hpp"
#include "wrap_guest_call.hpp"


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
typedef uint64_t (*wrap_static_func_ptr)(uint64_t* args);
//wrap guest function
struct BaseWrapFunction
{
    uint32_t guest_addr;
    bool is_host_imp;
    //a func imp in host, call by guest
    BaseWrapFunction(wrap_static_func_ptr pf);
    //a func imp in guest, call by host.
    BaseWrapFunction(uint32_t f_addr);
};
template<typename R,typename ...ARGS>
struct WrapFunction;


template<typename R,typename ...ARGS>
struct WrapFunction<R(ARGS...)>
{
    uint32_t guest_addr;
    WrapFunction(uint32_t addr):guest_addr(addr){}
    WrapFunction():guest_addr(0){}
    WrapFunction(host_fun_t hf){
        uint32_t addr = new_wrap_func(hf,"");
        guest_addr = addr;
    }
    
//    template <typename T>
    WrapFunction& operator=(uint32_t addr)
    {
//        callable_ = std::make_unique<CallableT<T>>(t);
        guest_addr = addr;
        
    }
    
    R operator()(ARGS... args) const
    {
        assert(guest_addr);
//        assert(callable_);
//        return callable_->Invoke(args...);
        wrap_win32_guest_call(guest_addr, args...);
        return g_engine->last_ret;
    }
//private:
//    class ICallable {
//    public:
//        virtual ~ICallable() = default;
//        virtual R Invoke(ARGS...) = 0;
//    };
//
//    template <typename T>
//    class CallableT : public ICallable
//    {
//    public:
//        CallableT(const T& t):t_(t) {}
//        ~CallableT() override = default;
//        R Invoke(ARGS... args) override
//        {
//            return t_(args...);
//        }
//    private:
//        T t_;
//    };
//
//    std::unique_ptr<ICallable> callable_;
};



#endif /* WrapCls_hpp */
