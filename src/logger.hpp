//
//  logger.hpp
//  x86emu
//
//  Created by tony on 2019/8/27.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef logger_hpp
#define logger_hpp

#include <stdio.h>



class Logger
{
public:
    enum LEVEL
    {
        LV_INFO = 0,
        LV_DEBUG,
        LV_WARN,
        LV_ERROR,
        LV_IGNOR,
    } level;
    Logger(LEVEL lv=LV_DEBUG);
    void info(const char* filename,int lineno, const char* func_name,
              const char* fmt,...);
    void debug(const char* filename,int lineno, const char* func_name,
               const char* fmt,...);
    void warn(const char* filename,int lineno, const char* func_name,
              const char* fmt,...);
    void error(const char* filename,int lineno, const char* func_name,
               const char* fmt,...);
    void vfmt(char* buf,const char* tag,const char* filename,int lineno,
        const char* func_name,const char* fmt,va_list args);
    void stdout(const char* s);
    void stderr(const char* s);
};

extern Logger* logger;

Logger* get_logger(Logger::LEVEL lv= Logger::LV_DEBUG);


#define LOG_INFO(fmt,...) get_logger()->info(__FILE__,__LINE__,__FUNCTION__,fmt,## __VA_ARGS__);
#define LOG_DEBUG(fmt,...) get_logger()->debug(__FILE__,__LINE__,__FUNCTION__,fmt,## __VA_ARGS__);
#define LOG_WARN(fmt,...) get_logger()->warn(__FILE__,__LINE__,__FUNCTION__,fmt,## __VA_ARGS__);
#define LOG_ERROR(fmt,...) get_logger()->error(__FILE__,__LINE__,__FUNCTION__,fmt,## __VA_ARGS__);

#endif /* logger_hpp */
