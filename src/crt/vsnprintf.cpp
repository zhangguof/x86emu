#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
//#include <stdlib.h>
#include <string.h>
#include "crt/dietstdio.h"
#include "wrap_crt.hpp"

#undef vsnprintf

struct str_data {
  char* str;
  size_t len;
  size_t size;
};

static int swrite(const void*ptr, size_t nmemb, void* cookie) {
  struct str_data* sd=(struct str_data*)cookie;
  size_t tmp=sd->size-sd->len;
  if (tmp>0) {
    size_t len=nmemb;
    if (len>tmp) len=tmp;
    if (sd->str) {
      memcpy(sd->str+sd->len,ptr,len);
      sd->str[sd->len+len]=0;
    }
    sd->len+=len;
  }
  return nmemb;
}

int win_vsnprintf(char* str, size_t size, const char *format,struct va_args* arg_ptr) {
  int n;
  struct str_data sd = { str, 0, size?size-1:0 };
  struct arg_printf ap = { &sd, swrite };
  n=__v_printf(&ap,format,arg_ptr);
  if (str && size && n>=0) {
    if (size!=(size_t)-1 && ((size_t)n>=size)) str[size-1]=0;
    else str[n]=0;
  }
  return n;
}
int __stdio_outs(const void *s,size_t len,void* cookie) {
    (void)cookie;
    return (write(1,s,len)==(ssize_t)len)?1:0;
}
int win_vprintf( const char *format,struct va_args* arg_ptr) {
    int n;
//    struct str_data sd = { str, 0, size?size-1:0 };
    struct arg_printf ap = { 0, __stdio_outs };
    n=__v_printf(&ap,format,arg_ptr);
    return n;
}
