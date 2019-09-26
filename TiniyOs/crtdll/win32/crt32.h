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


#ifdef __cplusplus
}
#endif



#endif  // EXAMPLE_DLL_H