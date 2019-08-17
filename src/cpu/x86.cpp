#define NEED_CPU_REG_SHORTCUTS 1
#include "cpu/x86.h"
#include "param_names.h"

// g_cpu = NULL;
extern void bx_init_hardware();



void Engine::init()
{
//    bx_cpu_ptr = &::bx_cpu; //global bx cpu!
    if(bx_cpu)
    {
        cpu_ptr = dynamic_cast<XE_CPU_C*>(bx_cpu);
    }
    else
    {
//        bx_cpu_ptr = bx_cpu = new BX_CPU_C();
        cpu_ptr = new XE_CPU_C();
        bx_cpu = cpu_ptr;
    }
    bx_mem_ptr = &::bx_mem;
    
    Bit64u memSize = 32*1024*1024; //32M
    Bit64u hostMemSize = 32*1023*1024;
    const char* rom_path = "/Users/tony/workspace/github/x86emu/tool/BIOS-bochs-latest";
    Bit32u ips = 4000000;
    
    bx_pc_system.setonoff(LOGLEV_DEBUG, ACT_REPORT);
    bx_pc_system.initialize(ips);
    
    bx_mem_ptr->init_memory(memSize, hostMemSize);
    bx_mem_ptr->load_ROM(rom_path, 0, 0);
    
    cpu_ptr->initialize();
    cpu_ptr->sanity_checks();
    cpu_ptr->register_state();
    
    bx_pc_system.register_state();
    // will enable A20 line and reset CPU and devices
    bx_pc_system.Reset(BX_RESET_HARDWARE);
    
    // First load the system BIOS (VGABIOS loading moved to the vga code)
//    SIM->get_param_string(BXPN_ROM_PATH)->set(rom_path);
//    bx_init_hardware();
    
}

void Engine::run()
{
    cpu_ptr->cpu_loop();
}


void XE_CPU_C::cpu_loop()
{
    printf("!!!!!in my loop!\n");
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
            RIP += i->ilen();
            // when handlers chaining is enabled this single call will execute entire trace
            BX_CPU_CALL_METHOD(i->execute1, (i)); // might iterate repeat instruction
            
            BX_SYNC_TIME_IF_SINGLE_PROCESSOR(0);
            
            if (BX_CPU_THIS_PTR async_event) break;
            
            i = getICacheEntry()->i;
        }
//#else // BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS == 0
//
//        bxInstruction_c *last = i + (entry->tlen);
//
//        for(;;) {
//
//            // want to allow changing of the instruction inside instrumentation callback
//            BX_INSTR_BEFORE_EXECUTION(BX_CPU_ID, i);
//            RIP += i->ilen();
//            BX_CPU_CALL_METHOD(i->execute1, (i)); // might iterate repeat instruction
//            BX_CPU_THIS_PTR prev_rip = RIP; // commit new RIP
//            BX_INSTR_AFTER_EXECUTION(BX_CPU_ID, i);
//            BX_CPU_THIS_PTR icount++;
//
//            BX_SYNC_TIME_IF_SINGLE_PROCESSOR(0);
//
//
//            if (BX_CPU_THIS_PTR async_event) break;
//
//            if (++i == last) {
//                entry = getICacheEntry();
//                i = entry->i;
//                last = i + (entry->tlen);
//            }
//        }
//#endif
        
        // clear stop trace magic indication that probably was set by repeat or branch32/64
        BX_CPU_THIS_PTR async_event &= ~BX_ASYNC_EVENT_STOP_TRACE;
        
    }  // while (1)
}

