#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bochs.h"
#include "cpu/x86.h"
//#include "plugin.h"


extern void bx_init_options();
extern void bx_init_siminterface();
extern void bx_init_hardware();
//extern void plugin_startup();

void init()
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
    CPU x86cpu;

    init();
    
    x86cpu.init();
    x86cpu.run();
    
    
	return 0;
}
