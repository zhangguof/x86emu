//
//  logger.cpp
//  x86emu
//
//  Created by tony on 2019/8/27.
//  Copyright © 2019 tony. All rights reserved.
//

#include "logger.hpp"
#include <string>
#include <stdarg.h>

const uint32_t max_buf_size = 2048;


//#define LOG_FMT(buf,tag,msg) \
//snprintf(buf,sizeof(buf),"[%s]%s(%d)-<%s>:%s\n",tag,__FILE__,__LINE__,__FUNCTION__,msg);
Logger* logger = nullptr;

Logger* get_logger(Logger::LEVEL lv)
{
    if(logger==nullptr)
    {
        logger = new Logger(lv);
    }
    return logger;
}

Logger::Logger(LEVEL lv)
{
    level = lv;
}
void Logger::stdout(const char* s)
{
    fprintf(::stdout, s);
}
void Logger::stderr(const char* s)
{
    fprintf(::stderr,s);
}

void Logger::vfmt(char* buf,const char* tag,const char* filename,int lineno,
         const char* func_name,const char* fmt,va_list args)
{
    char msg[max_buf_size];
//    char buf[2048];
    
    vsnprintf(msg,max_buf_size,fmt,args);
    snprintf(buf, max_buf_size,"[%s]%s(%d)-<%s>:\n%s\n",tag,filename,lineno,func_name,msg);
//    stdout(buf);
}

void Logger::info(const char* filename,int lineno, const char* func_name,
                  const char* fmt,...)
{
    if(level>LV_INFO) return;
    char buf[max_buf_size];
    
    va_list args;
    va_start(args, fmt);
    this->vfmt(buf,"INFO",filename,lineno,func_name,fmt,args);
    
    va_end(args);
    stdout(buf);

    
//    snprintf(buf, sizeof(buf), fmt,)
    
    
}
void Logger::debug(const char* filename,int lineno, const char* func_name,
                   const char* fmt,...)
{
    if(level>LV_DEBUG) return;
    char buf[max_buf_size];
    
    va_list args;
    va_start(args, fmt);
    this->vfmt(buf,"DEBUG",filename,lineno,func_name,fmt,args);
    
    va_end(args);
    
    stdout(buf);

    
}

void Logger::warn(const char* filename,int lineno, const char* func_name,
                  const char* fmt,...)
{
    if(level>LV_WARN) return;
    char buf[max_buf_size];
    
    va_list args;
    va_start(args, fmt);
    this->vfmt(buf,"WARN",filename,lineno,func_name,fmt,args);
    
    va_end(args);
    
    stdout(buf);
}

void Logger::error(const char* filename,int lineno, const char* func_name,
                   const char* fmt,...)
{
    if(level>LV_ERROR) return;
    char buf[max_buf_size];
    
    va_list args;
    va_start(args, fmt);
    this->vfmt(buf,"ERROR",filename,lineno,func_name,fmt,args);
    
    va_end(args);
    stderr(buf);

}

//C api

void log_info(const char* filename,int lineno, const char* func_name,
              const char* fmt,...)
{
    auto log = get_logger();
    if(log->level>Logger::LV_INFO) return;
    char buf[max_buf_size];
    
    va_list args;
    va_start(args, fmt);
    log->vfmt(buf,"INFO",filename,lineno,func_name,fmt,args);
    
    va_end(args);
    log->stdout(buf);
}
void log_debug(const char* filename,int lineno, const char* func_name,
               const char* fmt,...)
{
    auto log = get_logger();
    if(log->level>Logger::LV_DEBUG) return;
    char buf[max_buf_size];
    
    va_list args;
    va_start(args, fmt);
    log->vfmt(buf,"INFO",filename,lineno,func_name,fmt,args);
    
    va_end(args);
    log->stdout(buf);
}
void log_warn(const char* filename,int lineno, const char* func_name,
              const char* fmt,...)
{
    auto log = get_logger();
    if(log->level>Logger::LV_WARN) return;
    char buf[max_buf_size];
    
    va_list args;
    va_start(args, fmt);
    log->vfmt(buf,"INFO",filename,lineno,func_name,fmt,args);
    
    va_end(args);
    log->stdout(buf);
}
void log_error(const char* filename,int lineno, const char* func_name,
               const char* fmt,...)
{
    auto log = get_logger();
    if(log->level>Logger::LV_ERROR) return;
    char buf[max_buf_size];
    
    va_list args;
    va_start(args, fmt);
    log->vfmt(buf,"INFO",filename,lineno,func_name,fmt,args);
    
    va_end(args);
    log->stdout(buf);
}
