// #include <iostream>
// #include <string>

#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#define WINAPI __attribute__((__stdcall__))
extern "C" {
	#include <stdio.h>
	
	#include <stdint.h>

	EXPORT int test_cpp();
	WINAPI uint32_t DLLMain(void* hinstDLL, uint32_t fdwReason, uint32_t lpvReserved);
}


WINAPI uint32_t DllMain(void* hinstDLL, uint32_t fdwReason, uint32_t lpvReserved)
{
	printf("test2 DLLMain:%p,%d,%d\n",hinstDLL,fdwReason,lpvReserved);
	return 1;
}


EXPORT int test_cpp()
{
	// std::cout<<"hello world!"<<std::endl;
	// std::string h = "hello world";
	// printf("%s\n",h.c_str());
	printf("hello world\n");
	return 0;
}