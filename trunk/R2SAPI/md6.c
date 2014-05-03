#include <stddef.h>
#include "md6.h"
#include "r2sapi.h"

typedef md6_state MD6_CTX;

int API MD6_Init_Len(MD6_CTX *c, int l)
{
	return md6_init(c, l);
}

int API MD6_Update(MD6_CTX *c, const void *data, size_t len)
{
	return md6_update(c, (unsigned char *)data, len*8);
}

int API MD6_Final(unsigned char *md, MD6_CTX *c)
{
	return md6_final(c, md);
}

unsigned char * API MD6_Len(const void *d, size_t n, unsigned char *md, int l)
{
	if(MD6_SUCCESS != md6_hash(l, (unsigned char *)d, n*8, md))
		return NULL;
	return md;
}

int API MD6_Init(MD6_CTX *c)
{
	return MD6_Init_Len(c, 128);
}

unsigned char * API MD6(const void *d, size_t n, unsigned char *md)
{
	return MD6_Len(d, n, md, 128);
}

