// #include <stdio.h>
#include <stdint.h>
// #include <string.h>
#include "crt32.h"
// #include <time.h>
// #include <stdlib.h>


extern "C"
{
	#include "loaddll/load_dll.hpp"
	#include "gdt.h"

	void* _stdios[3];
	extern void init_stdio(void* s[3]);
	// __attribute__((weak)) FILE* __acrt_iob_func(unsigned int);

}


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


void * _iob[3] = {(void*)0,(void*)1,(void*)2};
void set_stdio()
{
	_stdios[0] = _iob[0];
	_stdios[1] = _iob[1];
	_stdios[2] = _iob[2];
}

int _initterm(_PVFV* pfbegin, _PVFV* pfend)
{
        /* 
         * walk the table of function pointers from the bottom up, until 
         * the end is encountered.  Do not skip the first entry.  The initial 
         * value of pfbegin points to the first valid entry.  Do not try to 
         * execute what pfend points to.  Only entries before pfend are valid. 
         */  
        while ( pfbegin < pfend )  
        {  
            /* 
             * if current table entry is non-NULL, call thru it. 
             */  
            if ( *pfbegin != NULL )  
                (**pfbegin)();  
            ++pfbegin;  
        }
        return 0; 
}

int DLLMain()
{
	printf("DLLMain start!\n");

	init_gdt();
    setup_nt_threadinfo(ExceptionHandler);

	set_stdio();
	init_stdio(_stdios);
	
	return 1;
}

