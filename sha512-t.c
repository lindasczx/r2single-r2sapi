#include <openssl/sha.h>
#include <openssl/internal/sha.h>
#include "r2sapi.h"

int API SHA512_224_Init(SHA512_CTX *c)
{
	sha512_224_init(c);
	return 0;
}

int API SHA512_256_Init(SHA512_CTX *c)
{
	sha512_256_init(c);
	return 0;
}

unsigned char * API SHA512_224(const void *d, size_t n, unsigned char *md)
{
	SHA512_CTX c;
	static unsigned char m[SHA224_DIGEST_LENGTH];

	if (md == NULL) md = m;
	sha512_224_init(&c);
	SHA512_Update(&c, d, n);
	SHA512_Final(md, &c);
	return md;
}

unsigned char * API SHA512_256(const void *d, size_t n, unsigned char *md)
{
	SHA512_CTX c;
	static unsigned char m[SHA256_DIGEST_LENGTH];

	if (md == NULL) md = m;
	sha512_256_init(&c);
	SHA512_Update(&c, d, n);
	SHA512_Final(md, &c);
	return md;
}
