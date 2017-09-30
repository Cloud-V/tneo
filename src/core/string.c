#include "string.h"

int __clzsi2 (unsigned int a)
{
    if(!a) return -1;
    for(int i = 0; i < 32; ++i){
        if(a & 1) return i;
        i = i>>1; 
    }
}

void *memset(void * ptr, int value, size_t num )
{
	void *end = ptr + num;
	while(ptr < end){
		*((unsigned char*)ptr) = value;
		++ptr;
	}
	
	return (void*)(end - ptr);
}
int memcmp ( const void * ptr1, const void * ptr2, size_t num )
{
	char *begin = (char *)ptr1;
	char *end = ptr1 + num;
	while(begin < end)
	{
		if(*begin < *end) return -1;
		if(*begin > *end) return 1;
		++begin;
	}
	return 0;
}