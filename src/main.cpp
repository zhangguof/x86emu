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
#include "logger.hpp"


int main()
{
    get_logger(Logger::LV_INFO);
    
    LOG_INFO("x86 simualte start!");
    

//    Engine x86emu;
    auto p_x86emu = std::make_shared<Engine>();
    g_engine = p_x86emu;

    
    p_x86emu->init();
    
    p_x86emu->run();
    
    
	return 0;
}
