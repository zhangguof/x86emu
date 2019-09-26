//
//  utils.h
//  x86emu
//
//  Created by tony on 2019/9/8.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef utils_h
#define utils_h

//template<typename T>
//struct WrapPointer;
//
//template <typename T>
//struct t_getvalue
//{
//    typedef T   value;
//};
//
//
//template <typename T>
//struct t_getvalue<T*>
//{
//    typedef WrapPointer<T> value;
//};
//
//
//
//template <typename T>
//struct t_traits
//{
//    typedef typename T::pointer     pointer;
//    typedef typename T::reference   reference;
//    typedef typename T::value       value;
//};
//
//template <typename T>
//struct t_traits<T*> {
//    typedef T*  pointer;
//    //    typedef T&  reference;
//    typedef WrapRef<T> reference;
//    typedef T   value;
//    //    typedef t_getvalue<T> value;
//};



/*
 char a[16];
 auto p = WrapPointer<char*>(a)
 *p = 'c'
 char** aa = nullptr
 auto pp = WrapPointer<char**>(aa);
 *pp = a;
 */


//template<typename T>
//struct WrapPointer
//{
//    //    typedef typename t_traits<T>::pointer    pointer;
//    //    typedef typename t_traits<T>::reference  reference;
//    //    typedef typename t_traits<T>::value      value;
//    typedef T* pointer;
//    typedef T& reference;
//    typedef T   value;
//    
//    
//    uint32_t addr;
//    void* host_ptr;
//    WrapPointer(uint32_t p){
//        addr = p;
//        host_ptr = getMemAddr(p);
//    }
//    WrapPointer(){
//        addr = 0;
//        host_ptr = 0;
//    }
//    pointer get() const
//    {
//        return (pointer)host_ptr;
//    }
//    reference operator*() const {
//        
//        //        return *((pointer)host_ptr);
//        return reference((pointer)host_ptr);
//    }
//    
//};
//
////auto p = WrapPointer<int*>;
////as int** p;
////
//template<typename T>
//struct WrapPointer<T*>
//{
//    //    typedef typename t_traits<T>::pointer    pointer;
//    //    typedef typename t_traits<T>::reference  reference;
//    //    typedef typename t_traits<T>::value      value;
//    typedef T* pointer;
//    typedef T& reference;
//    typedef T   value;
//    
//    
//    uint32_t addr;
//    void* host_ptr;
//    WrapPointer(uint32_t p){
//        addr = p;
//        host_ptr = getMemAddr(p);
//    }
//    WrapPointer(){
//        addr = 0;
//        host_ptr = 0;
//    }
//    pointer get() const
//    {
//        return (pointer)host_ptr;
//    }
//    reference operator*() const {
//        
//        //        return *((pointer)host_ptr);
//        return reference((pointer)host_ptr);
//    }
//    
//};
//
//template<typename T>
//struct WrapRef
//{
//    T* val;
//    WrapRef(T* v):val(v){}
//    WrapRef<T>& operator=(T rhs)
//    {
//        val = rhs;
//        return *this;
//    }
//};




#endif /* utils_h */
