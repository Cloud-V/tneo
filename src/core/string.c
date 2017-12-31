#include "string.h"

int __clzsi2 (unsigned int a)
{
	if(a == 0) 
		return -1;
    
	int i = 0;
	while(1){
        if(a & 1) return i;
        a = a>>1;
		++i;
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
	const char *begin = (char *)ptr1;
	const char *end = ptr1 + num;
	while(begin < end)
	{
		if(*begin < *end) return -1;
		if(*begin > *end) return 1;
		++begin;
	}
	return 0;
}