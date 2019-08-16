#ifndef _X86_H_
#define _X86_H_


#include <stdint.h>
#include "bochs/bochs.h"
#include "bochs/cpu/cpu.h"



class BOCHSAPI XE_CPU_C: public BX_CPU_C
{
public:
    void cpu_loop(void);
    
};


class CPU
{
public:
    XE_CPU_C* cpu_ptr;
    BX_MEM_C* bx_mem_ptr;
    CPU(){
        cpu_ptr = nullptr;
        bx_mem_ptr = nullptr;
    }
    void init();
    
    void run();
};

#endif 
