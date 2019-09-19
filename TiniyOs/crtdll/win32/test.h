#ifndef EXAMPLE_DLL_H
#define EXAMPLE_DLL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILD_DLL_TEST
#define DLLAPI __declspec(dllexport)
#else
// #define IMPORT __declspec(dllimport)
#define DLLAPI __declspec(dllimport)
#endif

#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)


DLLAPI int  Double(int x);
DLLAPI int  test_dll2();
DLLAPI int64_t test_64b(int a,int b,int64_t c);
DLLAPI uint64_t test_dll3(int a,const char* name,uint64_t a64);


typedef int (*p_func_t)(int);

struct call_funcs
{
	const char* name;
	p_func_t pf;
};

DLLAPI void init_call_funcs(call_funcs* tbl);
DLLAPI void test_call_ptr(int (*ptr)(int,int));




#ifdef __cplusplus
}
#endif



#endif  // EXAMPLE_DLL_H