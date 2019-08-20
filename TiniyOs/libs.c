#include <stdint.h>

#include "libs.h"
#include "host_func.h"
#include <stdarg.h>

// int printf(const char* fmt,...)
// {
// 	va_list ap;
// 	char tmpbuf[1024];
// 	va_start(ap,fmt);
// 	int ret = vsnprintf(tmpbuf, sizeof(tmpbuf), fmt, ap);
// 	tmpbuf[ret] = '\0';
// 	puts(tmpbuf);
	
// 	va_end(ap);
// 	return ret;
// }