#define NEED_CPU_REG_SHORTCUTS 1
#include "cpu/x86.h"
#include "param_names.h"
#include "elf.h"
#include <stdio.h>
#include "mmu.hpp"

#define LOG_THIS this->


void XE_MEM_C::load_RAM_from_data(Bit8u* data, Bit32u len, bx_phy_address ramaddress)
{
//    Bit32u size, offset;
//    offset = ramaddress;
    //load every 128k block 0x20000
//    assert((ramaddress & (BX_MEM_BLOCK_LEN-1))==0);
#define BLCOK_MASK (BX_MEM_BLOCK_LEN-1)
    bx_phy_address start = ramaddress;
    bx_phy_address end = ramaddress + len;
    
    Bit8u* pdst = this->get_vector(start);
    int load_size = 0;
    int remain_size = len;
    //check high bits
    if((start & ~BLCOK_MASK) == ((end-1) & ~BLCOK_MASK))
    {
        //in one block!
        assert(remain_size<=BX_MEM_BLOCK_LEN);
        load_size = remain_size;
        memcpy(pdst, data,load_size);
        return;
    }
    else
    {
        assert(remain_size >= BX_MEM_BLOCK_LEN);
        bx_phy_address next_addr = ((start/BX_MEM_BLOCK_LEN)+1)*BX_MEM_BLOCK_LEN;
        load_size = (next_addr - start);
        memcpy(pdst, data, load_size);
        remain_size -= load_size;
//        pdst  += load_size;
        start += load_size;
        data  += load_size;
    }
   assert((start & BLCOK_MASK)==0);
    
    while(remain_size>0)
    {
        pdst = this->get_vector(start);
        load_size = remain_size > BX_MEM_BLOCK_LEN?BX_MEM_BLOCK_LEN:remain_size;
        memcpy(pdst,data,load_size);
        remain_size -= load_size;
//        pdst  += load_size;
        data += load_size;
        start += load_size;
    }
    
//    BX_INFO(("ram at 0x%05x/%u ('%s')",(unsigned) ramaddress,(unsigned) len));
    
    BX_INFO(("rom at 0x%05x/%u ",ramaddress,len));
}

inline bx_bool XE_CPU_C::is_host_call(Bit64u addr)
{
    addr &= 0xFFFFFFFF;
    return (addr == 0x1FFFFFFF || addr==0x1FFFFFFE);
}

//guest fun build in gcc!! 
//f (a, b, c, d, e, f);
//a->%rdi, b->%rsi, c->%rdx, d->%rcx, e->%r8, f->%r9

//HOST_CALL_4ARGS

extern Bit64u do_call_host_func(Bit32u idx,HOST_CALL_5ARGS& args);


Bit64u XE_CPU_C::call_host_func(bxInstruction_c* i)
{
    Bit32u idx = EDI;
//    Bit64u data_addr = RSI;
//    Bit32u len = EDX;
    HOST_CALL_5ARGS arg5 = {RSI,RDX,RCX,R8,R9};
//    Bit8u* paddr = BX_MEM(0)->getHostMemAddr(this, data_addr, BX_RW);
//    printf("idx:%0x,data:%0x,len:%u\n",idx,data_addr,len);
    
    return do_call_host_func(idx,arg5);
}

Bit64u XE_CPU_C::call_win_host_func(bxInstruction_c* i)
{
    Bit32u idx = ECX;
    //    Bit64u data_addr = RSI;
    //    Bit32u len = EDX;
    HOST_CALL_5ARGS arg5 = {RDX,R8,R9,0,0};
    //    Bit8u* paddr = BX_MEM(0)->getHostMemAddr(this, data_addr, BX_RW);
    //    printf("idx:%0x,data:%0x,len:%u\n",idx,data_addr,len);
    
    return do_call_host_func(idx,arg5);
}


void XE_CPU_C::cpu_loop()
{
    this->is_exit = false;
    if (setjmp(BX_CPU_THIS_PTR jmp_buf_env)) {
        // can get here only from exception function or VMEXIT
        BX_CPU_THIS_PTR icount++;
        BX_SYNC_TIME_IF_SINGLE_PROCESSOR(0);
    }
    
    
    // We get here either by a normal function call, or by a longjmp
    // back from an exception() call.  In either case, commit the
    // new EIP/ESP, and set up other environmental fields.  This code
    // mirrors similar code below, after the interrupt() call.
    BX_CPU_THIS_PTR prev_rip = RIP; // commit new EIP
    BX_CPU_THIS_PTR speculative_rsp = 0;
    
    while (1) {
        
        // check on events which occurred for previous instructions (traps)
        // and ones which are asynchronous to the CPU (hardware interrupts)
        if (BX_CPU_THIS_PTR async_event) {
            if (handleAsyncEvent()) {
                // If request to return to caller ASAP.
                return;
            }
        }
        
        bxICacheEntry_c *entry = getICacheEntry();
        bxInstruction_c *i = entry->i;
        
//#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS
        for(;;) {
            // want to allow changing of the instruction inside instrumentation callback
            BX_INSTR_BEFORE_EXECUTION(BX_CPU_ID, i);
//            if(RIP == 0x400aee)
//            {
//                ;
//            }
            RIP += i->ilen();
            // when handlers chaining is enabled this single call will execute entire trace
            BX_CPU_CALL_METHOD(i->execute1, (i)); // might iterate repeat instruction
            //check exist
            if(is_exit) return;
            
            BX_SYNC_TIME_IF_SINGLE_PROCESSOR(0);
            
            if (BX_CPU_THIS_PTR async_event) break;
            entry = getICacheEntry();
            i = entry->i;
//            i = getICacheEntry()->i;
        }
        
        // clear stop trace magic indication that probably was set by repeat or branch32/64
        BX_CPU_THIS_PTR async_event &= ~BX_ASYNC_EVENT_STOP_TRACE;
        
    }  // while (1)
}

