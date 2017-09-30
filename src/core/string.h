#ifndef __STRING_H__
#define __STRING_H__

#define OUT_LOC 0x80000000
#define MAXHEAP 1024
#define NULL    0
#define PREC 10
typedef unsigned int size_t;


int __clzsi2 (unsigned int a);
void *memset(void * ptr, int value, size_t num );
int memcmp ( const void * ptr1, const void * ptr2, size_t num );

#endif