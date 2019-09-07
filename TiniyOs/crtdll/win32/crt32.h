#ifndef EXAMPLE_DLL_H
#define EXAMPLE_DLL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILDING_EXAMPLE_DLL
#define DLLAPI __declspec(dllexport)
#else
// #define IMPORT __declspec(dllimport)
#define DLLAPI __declspec(dllimport)
#endif

// void __stdcall EXAMPLE_DLL hello(const char *s);

DLLAPI int  Double(int x);
DLLAPI int  test_dll(const char* name,int a1,int a2,
						int a3,int a4,int a5,double f1);
DLLAPI int  test_dll2();

DLLAPI int64_t test_64b(int a,int b,int64_t c);
DLLAPI int test_dll3(int a,const char* name,uint64_t a64);
DLLAPI void call_host_ret();

// #define malloc host_malloc
// #define free host_free

void* malloc(size_t size);
void free(void* ptr);	

// extern void call_host_ret1();

extern void test_f1(int a1,unsigned int a2, const char* s1,
char c1,size_t st1,uint16_t u1,uint64_t u2);


#ifdef __cplusplus
}
#endif



#endif  // EXAMPLE_DLL_H