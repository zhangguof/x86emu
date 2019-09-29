// #include <stdio.h>
#include <stdint.h>
// #include <string.h>
#include <stdarg.h>
#include "crt32.h"
// #include <time.h>
// #include <stdlib.h>



extern "C"
{
	#include "loaddll/load_dll.hpp"
	#include "gdt.h"

	void* _stdios[3];
	extern void init_stdio(void* s[3]);

}

const char* regist_func_names[]=
{
	#undef DEF_HOST_FUNC
	#undef DEF_HOST_STD_FUNC

	#define DEF_HOST_FUNC(func,idx) \
	#func,

	#define DEF_HOST_STD_FUNC(func,idx,args) \
	#func,

	#include "cpu/host_call.hpp"
	#include "gen_code/wrap_gen_code.h"
	#include "winapi/wrap_winapi.h"

	#undef DEF_HOST_FUNC
	#undef DEF_HOST_STD_FUNC

};

extern uint32_t regist_func_addrs;
uint32_t* regist_func_addrs_ptr = &regist_func_addrs;

// extern void call_host_regist_funcs(const char** names,uint32_t addrs[],int size);
void init_funcs()
{
	uint32_t size = sizeof(regist_func_names) / sizeof(const char*);

	call_host_regist_funcs(regist_func_names,regist_func_addrs_ptr,size);
}


bool setup_nt_threadinfo(PEXCEPTION_HANDLER ExceptionHandler)
{
   static EXCEPTION_FRAME ExceptionFrame;
   static NT_TIB ThreadInfo = {
   	   .StackBase  = (void*)0x1efffff,
   	   .StackLimit = (void*)0x1d00000,
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



FILE _iob[3];
void set_stdio()
{
	_stdios[0] = &_iob[0];
	_stdios[1] = &_iob[1];
	_stdios[2] = &_iob[2];
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

int DllMain()
{
	printf("DllMain start!\n");

	set_stdio();
	init_stdio(_stdios);
	printf("stdio:%p,%p,%p\n",_stdios[0],_stdios[1],_stdios[2]);
	
	return 1;
}

void _crt32_pre_init()
{
	init_funcs();
	init_gdt();
	setup_nt_threadinfo(ExceptionHandler);
	DllMain();
}



