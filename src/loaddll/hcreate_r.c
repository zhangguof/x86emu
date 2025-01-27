#define _LINUX_SOURCE
//#include <search.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
//#include <sys/auxv.h>
#include <sys/random.h>
#include "loaddll/search.h"


int hcreate_r(size_t count, struct hsearch_data* htab) {
  htab->size=count;
  htab->filled=0;
  htab->table=calloc(count,sizeof(htab->table[0]));
  if (htab->table) {
//    char* rnd=(char*)getauxval(AT_RANDOM);
//    if (rnd)
//      memcpy(htab->key,rnd,16);
//    else
    {
      struct timeval tv;
      struct timeval* x=(struct timeval*)htab->key;
      if (getentropy(&htab->key,sizeof(htab->key)) != 0) {
	gettimeofday(&tv,NULL);
	x->tv_sec += tv.tv_sec;
	x->tv_usec += tv.tv_usec;
      }
    }
  }
  return htab->table ? 1 : 0;
}

