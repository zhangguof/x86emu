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
    g_engine = p_x86emu;

    env_init();
    
    p_x86emu->init();
    
    p_x86emu->run();
    
    
	return 0;
}
