#ifndef EXAMPLE_DLL_H
#define EXAMPLE_DLL_H

#ifdef __cplusplus
extern "C" {
#endif

// #ifdef BUILDING_EXAMPLE_DLL
#define EXPORT __declspec(dllexport)
// #else
#define IMPORT __declspec(dllimport)
// #endif

// void __stdcall EXAMPLE_DLL hello(const char *s);

EXPORT int  Double(int x);
EXPORT int  test_dll(const char* name,int a1,int a2,
						int a3,int a4,int a5,double f1);
EXPORT int  test_dll2();

EXPORT int64_t test_64b(int a,int b,int64_t c);

// #define malloc host_malloc
// #define free host_free

void* malloc(size_t size);
void free(void* ptr);	


#ifdef __cplusplus
}
#endif



#endif  // EXAMPLE_DLL_H