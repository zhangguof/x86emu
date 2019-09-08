//#include <stdarg.h>
#include <sys/types.h>
#include <stdlib.h>
//#include <stdio.h>
//#include "dietwarning.h"
#include "crt/wrap_crt.hpp"

int win_vsprintf(char *dest,const char *format, struct va_args* arg_ptr)
{
  return win_vsnprintf(dest,(size_t)-1-(uintptr_t)dest,format,arg_ptr);
}

//link_warning("vsprintf","warning: Avoid *sprintf; use *snprintf. It is more secure.")
