//
//  wrap_crt.hpp
//  x86emu
//
//  Created by tony on 2019/9/4.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef wrap_crt_hpp
#define wrap_crt_hpp

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
struct va_args
{
    uint8_t* ptr;
};
typedef uint32_t WIN32_PTR;
extern int win_vsnprintf(char* str, size_t size, const char *format,struct va_args* arg_ptr);
extern int win_vsprintf(char *dest,const char *format, struct va_args* arg_ptr);
extern int win_vprintf( const char *format,struct va_args* arg_ptr);
//int fprintf( std::FILE* stream, const char* format, ... );
    extern int win_fprintf(FILE* f,const char* format, ...);
    
#ifdef __cplusplus
}
#endif

#endif /* wrap_crt_hpp */
