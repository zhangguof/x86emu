#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "crt32.h"

// #include "test.h"

int DLLMain()
{
	printf("DLLMain start!\n");
	return 1;
}
//uint64_t test_dll3(int a,const char* name,uint64_t a64);
// void test_dll()
// {
// 	test_dll3(1234,"crt call test.dll!",0x12345678FEDCBA9);
// }
