#ifndef _X86_H_
#define _X86_H_


 #include <stdint.h>

// #define MASK32Bit (0xFFFFFFFF)
// #define MASK16Bit (0xFFFF)



class CPU
{
public:
	CPU(){}
	void init();
	int64_t regs[16];
	int32_t flags;
	
	void run();
	void htl();
};

// extern CPU* g_cpu;

#endif 
