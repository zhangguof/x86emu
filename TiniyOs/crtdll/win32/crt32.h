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

DLLAPI int DLLMain();
typedef void (__cdecl *_PVFV)(void);
DLLAPI int _initterm(_PVFV* a,_PVFV* b);
extern DLLAPI void exit(unsigned int);

extern EXPORT void *_iob[3];
extern DLLAPI int printf(const char*,...);


#ifdef __cplusplus
}
#endif



#endif  // EXAMPLE_DLL_H