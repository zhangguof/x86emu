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

class BOCHSAPI XE_MEM_C: public BX_MEM_C
{
public:
    void load_RAM_from_data(Bit8u* data, Bit32u len, bx_phy_address ramaddress);
    void alloca_pagebit64(Bit64u start_addr);
};


class Engine
{
public:
    XE_CPU_C* cpu_ptr;
    XE_MEM_C* mem_ptr;
    Engine(){
        cpu_ptr = nullptr;
        mem_ptr = nullptr;
    }
    void load_elf(const char* path);
    void init();
    
    void run();
};

#endif 
