#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "crt32.h"
#include <time.h>
#include <stdlib.h>


// #include "test.h"

// struct user_desc {
//     unsigned int  entry_number;
//     unsigned long base_addr;
//     unsigned int  limit;
//     unsigned int  seg_32bit : 1;
//     unsigned int  contents : 2;
//     unsigned int  read_exec_only : 1;
//     unsigned int  limit_in_pages : 1;
//     unsigned int  seg_not_present : 1;
//     unsigned int  useable : 1;
// };

extern "C"
{
	#include "loaddll/load_dll.hpp"
}
#include "gdt.h"

bool setup_nt_threadinfo(PEXCEPTION_HANDLER ExceptionHandler)
{
   static EXCEPTION_FRAME ExceptionFrame;
   static NT_TIB ThreadInfo = {
       .Self = &ThreadInfo,
   };
   struct user_desc pebdescriptor = {
       .entry_number       = (uint32_t)-1,
       .base_addr          = (uintptr_t) &ThreadInfo,
       .limit              = sizeof ThreadInfo,
       .seg_32bit          = 1,
       .contents           = 0,
       .read_exec_only     = 0,
       .limit_in_pages     = 0,
       .seg_not_present    = 0,
       .useable            = 1,
   };

   if (ExceptionHandler) {
       if (ThreadInfo.ExceptionList) {
           printf("Resetting ThreadInfo.ExceptionList");
       }
       ExceptionFrame.handler      = ExceptionHandler;
       ExceptionFrame.prev         = NULL;
       ThreadInfo.ExceptionList    = &ExceptionFrame;
   }

   auto pdes = gdt + used_gdt_des;
   pebdescriptor.entry_number = used_gdt_des++;
   init_descriptor(pdes,pebdescriptor.base_addr,
   pebdescriptor.limit,DA_32|DA_DRW);

   printf("fs base_addr:%p\n", &ThreadInfo);

   // if (syscall(__NR_set_thread_area, &pebdescriptor) != 0) {
   //     // return false;
   // }

   // Install descriptor
   // printf("Install descriptor fs!\n");
   asm("mov %[segment], %%fs" :: [segment] "r"(pebdescriptor.entry_number*8+0));

   return true;
}

EXCEPTION_DISPOSITION ExceptionHandler(struct _EXCEPTION_RECORD *ExceptionRecord,
            struct _EXCEPTION_FRAME *EstablisherFrame,
            struct _CONTEXT *ContextRecord,
            struct _EXCEPTION_FRAME **DispatcherContext)
    {
        printf("Toplevel Exception Handler Caught Exception");
        exit(0);
    }


int DLLMain()
{
	printf("DLLMain start!\n");

	init_gdt();
	printf("try setup_nt_threadinfo!\n");


    setup_nt_threadinfo(ExceptionHandler);
	// setup_nt_threadinfo(nullptr);

	return 1;
}
//uint64_t test_dll3(int a,const char* name,uint64_t a64);
// void test_dll()
// {
// 	test_dll3(1234,"crt call test.dll!",0x12345678FEDCBA9);
// }
