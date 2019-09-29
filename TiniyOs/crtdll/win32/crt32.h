#ifndef EXAMPLE_DLL_H
#define EXAMPLE_DLL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILD_DLL_CRT
#define DLLAPI __declspec(dllexport)
#else
// #define IMPORT __declspec(dllimport)
#define DLLAPI __declspec(dllimport)
#endif

#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)

#define WINAPI __attribute__((__stdcall__))


DLLAPI int DllMain();
DLLAPI void _crt32_pre_init();
typedef void (__cdecl *_PVFV)(void);

DLLAPI int _initterm(_PVFV* a,_PVFV* b);
extern DLLAPI void exit(unsigned int);

struct _iobuf {
    char *_ptr;
    int _cnt;
    char *_base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char *_tmpfname;
  };
typedef struct _iobuf FILE;

extern EXPORT FILE _iob[3];
extern DLLAPI int printf(const char*,...);
extern DLLAPI int vprintf(const char*,va_list ap);
extern DLLAPI void* get_func_by_name(const char* name);

//def host func
// typedef void(*generic_func)(...);
// typedef WINAPI void(*std_generic_func)(...);

// #undef DEF_HOST_FUNC
// #undef DEF_HOST_STD_FUNC

// #define DEF_HOST_FUNC(func,idx) \
// extern generic_func func;

// #define DEF_HOST_STD_FUNC(func,idx,args) \
// extern std_generic_func func;

// #include "cpu/host_call.hpp"
// #include "gen_code/wrap_gen_code.h"
// #include "winapi/wrap_winapi.h"

// #undef DEF_HOST_FUNC
// #undef DEF_HOST_STD_FUNC

extern void call_host_regist_funcs(const char** names,uint32_t addrs[],uint32_t size);

#ifdef __cplusplus
}
#endif



#endif  // EXAMPLE_DLL_H