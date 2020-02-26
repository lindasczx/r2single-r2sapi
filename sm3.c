#include <openssl/sm3.h>
#include "r2sapi.h"

unsigned char * API SM3(const void *d, size_t n, unsigned char *md)
{
	SM3_CTX c;
	static unsigned char m[SM3_DIGEST_LENGTH];

	if (md == NULL) md = m;
	sm3_init(&c);
	sm3_update(&c, d, n);
	sm3_final(md, &c);
	return md;
}

