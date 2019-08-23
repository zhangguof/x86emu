#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bochs.h"
#include "cpu/x86.h"
//#include "plugin.h"
#include <memory>

#include "elf.h"
#include "engine.hpp"

extern void load_elf_bin(const char*path, Bit8u** pdata);
extern void print_elf_info(Elf64_Ehdr* p_elfh);


extern void bx_init_options();
extern void bx_init_siminterface();
extern void bx_init_hardware();
//extern void plugin_startup();

void env_init()
{
//    plugin_startup();
//    pluginlog = new logfunctions();
//    pluginlog->put("PLUGIN");

    SAFE_GET_IOFUNC();  // never freed
    SAFE_GET_GENLOG();  // never freed
    genlog->setonoff(LOGLEV_DEBUG, ACT_REPORT);
    
    bx_init_siminterface();
    bx_init_options();
//    bx_init_hardware();
    
    
}

int main()
{
	printf("x86 simualte start!\n");
//    const char* test="=====test!!!!!!\n";
//    write(1,test,4);

//    Engine x86emu;
    auto p_x86emu = std::make_shared<Engine>();

    env_init();
    
    p_x86emu->init();
    p_x86emu->run();
    
    
	return 0;
}
