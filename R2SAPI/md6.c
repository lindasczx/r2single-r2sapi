#include <stddef.h>
#include "md6.h"

typedef md6_state MD6_CTX;

int __stdcall MD6_Init_Len(MD6_CTX *c, int l)
{
	return md6_init(c, l);
}

int __stdcall MD6_Update(MD6_CTX *c, const void *data, size_t len)
{
	return md6_update(c, (const unsigned char *)data, len*sizeof(char));
}

int __stdcall MD6_Final(unsigned char *md, MD6_CTX *c)
{
	return md6_final(c, md);
}

unsigned char * __stdcall MD6_Len(const unsigned char *d, size_t n, unsigned char *md, int l)
{
	if(MD6_SUCCESS != md6_hash(l, d, n*sizeof(char), md))
		return NULL;
	return md;
}

int __stdcall MD6_Init(MD6_CTX *c)
{
	return MD6_Init_Len(c, 128);
}

unsigned char * __stdcall MD6(const unsigned char *d, size_t n, unsigned char *md)
{
	return MD6_Len(d, n, md, 128);
}

