
// file auto gen by antlr4 parser
#define NEED_CPU_REG_SHORTCUTS 1
#include "cpu/x86.h"
#include "param_names.h"
#include "elf.h"
#include <stdio.h>
#include "mmu.hpp"

#include <string.h>
#include <functional>
#include "engine.hpp"


//src:void * memchr ( const void * __s , int __c , size_t __n ) ; 
//fmt:void* memchr(const void*,int,size_t)
uint64_t wrap_memchr(uint64_t* args)
{
    typedef const void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef int _T_1;
    auto arg_1 = (_T_1)(args[1]);
    
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef void* _T_ret;

    auto ret = memchr( arg_0,  arg_1, arg_2 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:int memcmp ( const void * __s1 , const void * __s2 , size_t __n ) ; 
//fmt:int memcmp(const void*,const void*,size_t)
uint64_t wrap_memcmp(uint64_t* args)
{
    typedef const void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const void* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef int _T_ret;

    auto ret = memcmp( arg_0,  arg_1, arg_2 );
    
    
    return (uint64_t)ret;
    
}

//src:void * memcpy ( void * __dst , const void * __src , size_t __n ) ; 
//fmt:void* memcpy(void*,const void*,size_t)
uint64_t wrap_memcpy(uint64_t* args)
{
    typedef void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const void* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef void* _T_ret;

    auto ret = memcpy( arg_0,  arg_1, arg_2 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:void * memmove ( void * __dst , const void * __src , size_t __len ) ; 
//fmt:void* memmove(void*,const void*,size_t)
uint64_t wrap_memmove(uint64_t* args)
{
    typedef void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const void* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef void* _T_ret;

    auto ret = memmove( arg_0,  arg_1, arg_2 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:void * memset ( void * __b , int __c , size_t __len ) ; 
//fmt:void* memset(void*,int,size_t)
uint64_t wrap_memset(uint64_t* args)
{
    typedef void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef int _T_1;
    auto arg_1 = (_T_1)(args[1]);
    
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef void* _T_ret;

    auto ret = memset( arg_0,  arg_1, arg_2 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:char * strcat ( char * __s1 , const char * __s2 ) ; 
//fmt:char* strcat(char*,const char*)
uint64_t wrap_strcat(uint64_t* args)
{
    typedef char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef char* _T_ret;

    auto ret = strcat( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:char * strchr ( const char * __s , int __c ) ; 
//fmt:char* strchr(const char*,int)
uint64_t wrap_strchr(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef int _T_1;
    auto arg_1 = (_T_1)(args[1]);
    
    typedef char* _T_ret;

    auto ret = strchr( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:int strcmp ( const char * __s1 , const char * __s2 ) ; 
//fmt:int strcmp(const char*,const char*)
uint64_t wrap_strcmp(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef int _T_ret;

    auto ret = strcmp( arg_0, arg_1 );
    
    
    return (uint64_t)ret;
    
}

//src:int strcoll ( const char * __s1 , const char * __s2 ) ; 
//fmt:int strcoll(const char*,const char*)
uint64_t wrap_strcoll(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef int _T_ret;

    auto ret = strcoll( arg_0, arg_1 );
    
    
    return (uint64_t)ret;
    
}

//src:char * strcpy ( char * __dst , const char * __src ) ; 
//fmt:char* strcpy(char*,const char*)
uint64_t wrap_strcpy(uint64_t* args)
{
    typedef char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef char* _T_ret;

    auto ret = strcpy( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:size_t strcspn ( const char * __s , const char * __charset ) ; 
//fmt:size_t strcspn(const char*,const char*)
uint64_t wrap_strcspn(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_ret;

    auto ret = strcspn( arg_0, arg_1 );
    
    
    return (uint64_t)ret;
    
}

//src:char * strerror ( int __errnum ) __asm ( "_" "strerror" ) ; 
//fmt:char* strerror(int)
uint64_t wrap_strerror(uint64_t* args)
{
    typedef int _T_0;
    auto arg_0 = (_T_0)(args[0]);
    
    typedef char* _T_ret;

    auto ret = strerror(arg_0 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:size_t strlen ( const char * __s ) ; 
//fmt:size_t strlen(const char*)
uint64_t wrap_strlen(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef size_t _T_ret;

    auto ret = strlen(arg_0 );
    
    
    return (uint64_t)ret;
    
}

//src:char * strncat ( char * __s1 , const char * __s2 , size_t __n ) ; 
//fmt:char* strncat(char*,const char*,size_t)
uint64_t wrap_strncat(uint64_t* args)
{
    typedef char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef char* _T_ret;

    auto ret = strncat( arg_0,  arg_1, arg_2 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:int strncmp ( const char * __s1 , const char * __s2 , size_t __n ) ; 
//fmt:int strncmp(const char*,const char*,size_t)
uint64_t wrap_strncmp(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef int _T_ret;

    auto ret = strncmp( arg_0,  arg_1, arg_2 );
    
    
    return (uint64_t)ret;
    
}

//src:char * strncpy ( char * __dst , const char * __src , size_t __n ) ; 
//fmt:char* strncpy(char*,const char*,size_t)
uint64_t wrap_strncpy(uint64_t* args)
{
    typedef char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef char* _T_ret;

    auto ret = strncpy( arg_0,  arg_1, arg_2 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:char * strpbrk ( const char * __s , const char * __charset ) ; 
//fmt:char* strpbrk(const char*,const char*)
uint64_t wrap_strpbrk(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef char* _T_ret;

    auto ret = strpbrk( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:char * strrchr ( const char * __s , int __c ) ; 
//fmt:char* strrchr(const char*,int)
uint64_t wrap_strrchr(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef int _T_1;
    auto arg_1 = (_T_1)(args[1]);
    
    typedef char* _T_ret;

    auto ret = strrchr( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:size_t strspn ( const char * __s , const char * __charset ) ; 
//fmt:size_t strspn(const char*,const char*)
uint64_t wrap_strspn(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_ret;

    auto ret = strspn( arg_0, arg_1 );
    
    
    return (uint64_t)ret;
    
}

//src:char * strstr ( const char * __big , const char * __little ) ; 
//fmt:char* strstr(const char*,const char*)
uint64_t wrap_strstr(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef char* _T_ret;

    auto ret = strstr( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:char * strtok ( char * __str , const char * __sep ) ; 
//fmt:char* strtok(char*,const char*)
uint64_t wrap_strtok(uint64_t* args)
{
    typedef char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef char* _T_ret;

    auto ret = strtok( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:size_t strxfrm ( char * __s1 , const char * __s2 , size_t __n ) ; 
//fmt:size_t strxfrm(char*,const char*,size_t)
uint64_t wrap_strxfrm(uint64_t* args)
{
    typedef char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef size_t _T_ret;

    auto ret = strxfrm( arg_0,  arg_1, arg_2 );
    
    
    return (uint64_t)ret;
    
}

//src:char * strtok_r ( char * __str , const char * __sep , char * * __lasts ) ; 
//fmt:char* strtok_r(char*,const char*,char**)
uint64_t wrap_strtok_r(uint64_t* args)
{
    typedef char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef char** _T_2;
    auto arg_2 = (_T_2)(args[2]);
    arg_2 = (_T_2)getMemAddr((bx_phy_address)arg_2);
    typedef char* _T_ret;

    auto ret = strtok_r( arg_0,  arg_1, arg_2 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:int strerror_r ( int __errnum , char * __strerrbuf , size_t __buflen ) ; 
//fmt:int strerror_r(int,char*,size_t)
uint64_t wrap_strerror_r(uint64_t* args)
{
    typedef int _T_0;
    auto arg_0 = (_T_0)(args[0]);
    
    typedef char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef int _T_ret;

    auto ret = strerror_r( arg_0,  arg_1, arg_2 );
    
    
    return (uint64_t)ret;
    
}

//src:char * strdup ( const char * __s1 ) ; 
//fmt:char* strdup(const char*)
uint64_t wrap_strdup(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef char* _T_ret;

    auto ret = strdup(arg_0 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:void * memccpy ( void * __dst , const void * __src , int __c , size_t __n ) ; 
//fmt:void* memccpy(void*,const void*,int,size_t)
uint64_t wrap_memccpy(uint64_t* args)
{
    typedef void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const void* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef int _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef size_t _T_3;
    auto arg_3 = (_T_3)(args[3]);
    
    typedef void* _T_ret;

    auto ret = memccpy( arg_0,  arg_1,  arg_2, arg_3 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:char * stpcpy ( char * __dst , const char * __src ) ; 
//fmt:char* stpcpy(char*,const char*)
uint64_t wrap_stpcpy(uint64_t* args)
{
    typedef char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef char* _T_ret;

    auto ret = stpcpy( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:char * stpncpy ( char * __dst , const char * __src , size_t __n ) __attribute__ ( ( availability ( macosx , introduced = 10.7 ) ) ) ; 
//fmt:char* stpncpy(char*,const char*,size_t)
uint64_t wrap_stpncpy(uint64_t* args)
{
    typedef char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef char* _T_ret;

    auto ret = stpncpy( arg_0,  arg_1, arg_2 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:char * strndup ( const char * __s1 , size_t __n ) __attribute__ ( ( availability ( macosx , introduced = 10.7 ) ) ) ; 
//fmt:char* strndup(const char*,size_t)
uint64_t wrap_strndup(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef size_t _T_1;
    auto arg_1 = (_T_1)(args[1]);
    
    typedef char* _T_ret;

    auto ret = strndup( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:size_t strnlen ( const char * __s1 , size_t __n ) __attribute__ ( ( availability ( macosx , introduced = 10.7 ) ) ) ; 
//fmt:size_t strnlen(const char*,size_t)
uint64_t wrap_strnlen(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef size_t _T_1;
    auto arg_1 = (_T_1)(args[1]);
    
    typedef size_t _T_ret;

    auto ret = strnlen( arg_0, arg_1 );
    
    
    return (uint64_t)ret;
    
}

//src:char * strsignal ( int __sig ) ; 
//fmt:char* strsignal(int)
uint64_t wrap_strsignal(uint64_t* args)
{
    typedef int _T_0;
    auto arg_0 = (_T_0)(args[0]);
    
    typedef char* _T_ret;

    auto ret = strsignal(arg_0 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:errno_t memset_s ( void * __s , rsize_t __smax , int __c , rsize_t __n ) __attribute__ ( ( availability ( macosx , introduced = 10.9 ) ) ) ; 
//fmt:errno_t memset_s(void*,rsize_t,int,rsize_t)
uint64_t wrap_memset_s(uint64_t* args)
{
    typedef void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef rsize_t _T_1;
    auto arg_1 = (_T_1)(args[1]);
    
    typedef int _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef rsize_t _T_3;
    auto arg_3 = (_T_3)(args[3]);
    
    typedef errno_t _T_ret;

    auto ret = memset_s( arg_0,  arg_1,  arg_2, arg_3 );
    
    
    return (uint64_t)ret;
    
}

//src:void * memmem ( const void * __big , size_t __big_len , const void * __little , size_t __little_len ) __attribute__ ( ( availability ( macosx , introduced = 10.7 ) ) ) ; 
//fmt:void* memmem(const void*,size_t,const void*,size_t)
uint64_t wrap_memmem(uint64_t* args)
{
    typedef const void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef size_t _T_1;
    auto arg_1 = (_T_1)(args[1]);
    
    typedef const void* _T_2;
    auto arg_2 = (_T_2)(args[2]);
    arg_2 = (_T_2)getMemAddr((bx_phy_address)arg_2);
    typedef size_t _T_3;
    auto arg_3 = (_T_3)(args[3]);
    
    typedef void* _T_ret;

    auto ret = memmem( arg_0,  arg_1,  arg_2, arg_3 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:void memset_pattern4 ( void * __b , const void * __pattern4 , size_t __len ) __attribute__ ( ( availability ( macosx , introduced = 10.5 ) ) ) ; 
//fmt:void memset_pattern4(void*,const void*,size_t)
uint64_t wrap_memset_pattern4(uint64_t* args)
{
    typedef void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const void* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef void _T_ret;

    
    uint64_t ret = 0;
    memset_pattern4( arg_0,  arg_1, arg_2 );
    
    return (uint64_t)ret;
    
}

//src:void memset_pattern8 ( void * __b , const void * __pattern8 , size_t __len ) __attribute__ ( ( availability ( macosx , introduced = 10.5 ) ) ) ; 
//fmt:void memset_pattern8(void*,const void*,size_t)
uint64_t wrap_memset_pattern8(uint64_t* args)
{
    typedef void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const void* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef void _T_ret;

    
    uint64_t ret = 0;
    memset_pattern8( arg_0,  arg_1, arg_2 );
    
    return (uint64_t)ret;
    
}

//src:void memset_pattern16 ( void * __b , const void * __pattern16 , size_t __len ) __attribute__ ( ( availability ( macosx , introduced = 10.5 ) ) ) ; 
//fmt:void memset_pattern16(void*,const void*,size_t)
uint64_t wrap_memset_pattern16(uint64_t* args)
{
    typedef void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const void* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef void _T_ret;

    
    uint64_t ret = 0;
    memset_pattern16( arg_0,  arg_1, arg_2 );
    
    return (uint64_t)ret;
    
}

//src:char * strcasestr ( const char * __big , const char * __little ) ; 
//fmt:char* strcasestr(const char*,const char*)
uint64_t wrap_strcasestr(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef char* _T_ret;

    auto ret = strcasestr( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:char * strnstr ( const char * __big , const char * __little , size_t __len ) ; 
//fmt:char* strnstr(const char*,const char*,size_t)
uint64_t wrap_strnstr(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef char* _T_ret;

    auto ret = strnstr( arg_0,  arg_1, arg_2 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:size_t strlcat ( char * __dst , const char * __source , size_t __size ) ; 
//fmt:size_t strlcat(char*,const char*,size_t)
uint64_t wrap_strlcat(uint64_t* args)
{
    typedef char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef size_t _T_ret;

    auto ret = strlcat( arg_0,  arg_1, arg_2 );
    
    
    return (uint64_t)ret;
    
}

//src:size_t strlcpy ( char * __dst , const char * __source , size_t __size ) ; 
//fmt:size_t strlcpy(char*,const char*,size_t)
uint64_t wrap_strlcpy(uint64_t* args)
{
    typedef char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef size_t _T_ret;

    auto ret = strlcpy( arg_0,  arg_1, arg_2 );
    
    
    return (uint64_t)ret;
    
}

//src:void strmode ( int __mode , char * __bp ) ; 
//fmt:void strmode(int,char*)
uint64_t wrap_strmode(uint64_t* args)
{
    typedef int _T_0;
    auto arg_0 = (_T_0)(args[0]);
    
    typedef char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef void _T_ret;

    
    uint64_t ret = 0;
    strmode( arg_0, arg_1 );
    
    return (uint64_t)ret;
    
}

//src:char * strsep ( char * * __stringp , const char * __delim ) ; 
//fmt:char* strsep(char**,const char*)
uint64_t wrap_strsep(uint64_t* args)
{
    typedef char** _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef char* _T_ret;

    auto ret = strsep( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:void swab ( const void * restrict , void * restrict , ssize_t ) ; 
//fmt:void swab(const void*,void*,ssize_t)
uint64_t wrap_swab(uint64_t* args)
{
    typedef const void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef void* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef ssize_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef void _T_ret;

    
    uint64_t ret = 0;
    swab( arg_0,  arg_1, arg_2 );
    
    return (uint64_t)ret;
    
}

//src:int timingsafe_bcmp ( const void * __b1 , const void * __b2 , size_t __len ) ; 
//fmt:int timingsafe_bcmp(const void*,const void*,size_t)
uint64_t wrap_timingsafe_bcmp(uint64_t* args)
{
    typedef const void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const void* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef int _T_ret;

    auto ret = timingsafe_bcmp( arg_0,  arg_1, arg_2 );
    
    
    return (uint64_t)ret;
    
}

//src:int bcmp ( const void * , const void * , size_t ) ; 
//fmt:int bcmp(const void*,const void*,size_t)
uint64_t wrap_bcmp(uint64_t* args)
{
    typedef const void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const void* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef int _T_ret;

    auto ret = bcmp( arg_0,  arg_1, arg_2 );
    
    
    return (uint64_t)ret;
    
}

//src:void bcopy ( const void * , void * , size_t ) ; 
//fmt:void bcopy(const void*,void*,size_t)
uint64_t wrap_bcopy(uint64_t* args)
{
    typedef const void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef void* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef void _T_ret;

    
    uint64_t ret = 0;
    bcopy( arg_0,  arg_1, arg_2 );
    
    return (uint64_t)ret;
    
}

//src:void bzero ( void * , size_t ) ; 
//fmt:void bzero(void*,size_t)
uint64_t wrap_bzero(uint64_t* args)
{
    typedef void* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef size_t _T_1;
    auto arg_1 = (_T_1)(args[1]);
    
    typedef void _T_ret;

    
    uint64_t ret = 0;
    bzero( arg_0, arg_1 );
    
    return (uint64_t)ret;
    
}

//src:char * index ( const char * , int ) ; 
//fmt:char* index(const char*,int)
uint64_t wrap_index(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef int _T_1;
    auto arg_1 = (_T_1)(args[1]);
    
    typedef char* _T_ret;

    auto ret = index( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:char * rindex ( const char * , int ) ; 
//fmt:char* rindex(const char*,int)
uint64_t wrap_rindex(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef int _T_1;
    auto arg_1 = (_T_1)(args[1]);
    
    typedef char* _T_ret;

    auto ret = rindex( arg_0, arg_1 );
    ret = (_T_ret)getGuestAddr(bx_phy_address(ret));
    
    return (uint64_t)ret;
    
}

//src:int ffs ( int ) ; 
//fmt:int ffs(int)
uint64_t wrap_ffs(uint64_t* args)
{
    typedef int _T_0;
    auto arg_0 = (_T_0)(args[0]);
    
    typedef int _T_ret;

    auto ret = ffs(arg_0 );
    
    
    return (uint64_t)ret;
    
}

//src:int strcasecmp ( const char * , const char * ) ; 
//fmt:int strcasecmp(const char*,const char*)
uint64_t wrap_strcasecmp(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef int _T_ret;

    auto ret = strcasecmp( arg_0, arg_1 );
    
    
    return (uint64_t)ret;
    
}

//src:int strncasecmp ( const char * , const char * , size_t ) ; 
//fmt:int strncasecmp(const char*,const char*,size_t)
uint64_t wrap_strncasecmp(uint64_t* args)
{
    typedef const char* _T_0;
    auto arg_0 = (_T_0)(args[0]);
    arg_0 = (_T_0)getMemAddr((bx_phy_address)arg_0);
    typedef const char* _T_1;
    auto arg_1 = (_T_1)(args[1]);
    arg_1 = (_T_1)getMemAddr((bx_phy_address)arg_1);
    typedef size_t _T_2;
    auto arg_2 = (_T_2)(args[2]);
    
    typedef int _T_ret;

    auto ret = strncasecmp( arg_0,  arg_1, arg_2 );
    
    
    return (uint64_t)ret;
    
}

//src:int ffsl ( long ) __attribute__ ( ( availability ( macosx , introduced = 10.5 ) ) ) ; 
//fmt:int ffsl(long)
uint64_t wrap_ffsl(uint64_t* args)
{
    typedef long _T_0;
    auto arg_0 = (_T_0)(args[0]);
    
    typedef int _T_ret;

    auto ret = ffsl(arg_0 );
    
    
    return (uint64_t)ret;
    
}

//src:int ffsll ( long long ) __attribute__ ( ( availability ( macosx , introduced = 10.9 ) ) ) ; 
//fmt:int ffsll(long long)
uint64_t wrap_ffsll(uint64_t* args)
{
    typedef long long _T_0;
    auto arg_0 = (_T_0)(args[0]);
    
    typedef int _T_ret;

    auto ret = ffsll(arg_0 );
    
    
    return (uint64_t)ret;
    
}

//src:int fls ( int ) __attribute__ ( ( availability ( macosx , introduced = 10.5 ) ) ) ; 
//fmt:int fls(int)
uint64_t wrap_fls(uint64_t* args)
{
    typedef int _T_0;
    auto arg_0 = (_T_0)(args[0]);
    
    typedef int _T_ret;

    auto ret = fls(arg_0 );
    
    
    return (uint64_t)ret;
    
}

//src:int flsl ( long ) __attribute__ ( ( availability ( macosx , introduced = 10.5 ) ) ) ; 
//fmt:int flsl(long)
uint64_t wrap_flsl(uint64_t* args)
{
    typedef long _T_0;
    auto arg_0 = (_T_0)(args[0]);
    
    typedef int _T_ret;

    auto ret = flsl(arg_0 );
    
    
    return (uint64_t)ret;
    
}

//src:int flsll ( long long ) __attribute__ ( ( availability ( macosx , introduced = 10.9 ) ) ) ; 
//fmt:int flsll(long long)
uint64_t wrap_flsll(uint64_t* args)
{
    typedef long long _T_0;
    auto arg_0 = (_T_0)(args[0]);
    
    typedef int _T_ret;

    auto ret = flsll(arg_0 );
    
    
    return (uint64_t)ret;
    
}
