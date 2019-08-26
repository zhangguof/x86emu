#ifndef _X86_H_
#define _X86_H_


#include <stdint.h>
#include "bochs/bochs.h"
#include "bochs/cpu/cpu.h"


//guest fun build in gcc!!
//f (a, b, c, d, e, f);
//a->%rdi, b->%rsi, c->%rdx, d->%rcx, e->%r8, f->%r9


//struct HOST_CALL_4ARGS
//{
//    Bit64u args[4];
//};
typedef Bit64u HOST_CALL_5ARGS[5];




class BOCHSAPI XE_CPU_C: public BX_CPU_C
{
public:
    bool is_exit;
    void cpu_loop(void);
    bx_bool is_host_call(Bit64u addr);
    Bit64u call_host_func(bxInstruction_c* i);
    
};

class BOCHSAPI XE_MEM_C: public BX_MEM_C
{
public:
    void load_RAM_from_data(Bit8u* data, Bit32u len, bx_phy_address ramaddress);
    void alloca_pagebit64(Bit64u start_addr);
};




#endif 
