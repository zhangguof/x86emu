#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bochs.h"
#include "cpu/x86.h"
//#include "plugin.h"
#include <memory>

#include "elf.h"
#include "elf-ext.hpp"
#include "engine.hpp"


int main()
{
	printf("x86 simualte start!\n");
//    const char* test="=====test!!!!!!\n";
//    write(1,test,4);

//    Engine x86emu;
    auto p_x86emu = std::make_shared<Engine>();
    g_engine = p_x86emu;

    
    p_x86emu->init();
    
    p_x86emu->run();
    
    
	return 0;
}
