#ifndef SEARCH_H
#define SEARCH_H

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>

typedef enum { FIND, ENTER } ACTION;
typedef struct entry { char* key; void* data; } ENTRY;

extern ENTRY* hsearch(ENTRY item, ACTION action) ;
extern int hcreate(size_t count) ;
extern void hdestroy(void) ;

#ifdef _GNU_SOURCE
/* This is the internal structure of the hash table buckets.
 * Do not use it or your program will be dietlibc-only!
 * It's here so it's possible to write traversal code. */
struct bucket {
  struct bucket* next;
  struct entry e;
  size_t hv;
};
#else
struct bucket;
#endif

struct hsearch_data {
  struct bucket** table;
  size_t size, filled;
  unsigned char key[16];
};

extern int hsearch_r(ENTRY item, ACTION action, ENTRY** retval, struct hsearch_data* htab) ;
extern int hcreate_r(size_t count, struct hsearch_data* htab) ;
extern void hdestroy_r(struct hsearch_data* htab) ;

extern uint64_t siphash24(const unsigned char key[16], const unsigned char *in, size_t len) ;

#endif
