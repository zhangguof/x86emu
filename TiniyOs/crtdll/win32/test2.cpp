// #include <iostream>
#include <string>

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
	// printf("test2 DLLMain:%p,%d,%d\n",hinstDLL,fdwReason,lpvReserved);
	puts("test2 DllMain");
	return 1;
}

int main()
{
	std::string h = "hhh";
	return 0;
}


EXPORT int test_cpp()
{
	// std::cout<<"hello world!"<<std::endl;
	// std::string h = "xxhello world";
	// printf("%s\n",h.c_str());
	// puts(h.c_str());
	printf("hello world\n");
	// fputc('X',stdout);
	return 0;
}