#include <nettle/sha3.h>
#include "r2sapi.h"

int API SHA3_224_Init(struct sha3_224_ctx *c)
{
	sha3_224_init(c);
	return 0;
}

int API SHA3_256_Init(struct sha3_256_ctx *c)
{
	sha3_256_init(c);
	return 0;
}

int API SHA3_384_Init(struct sha3_384_ctx *c)
{
	sha3_384_init(c);
	return 0;
}

int API SHA3_512_Init(struct sha3_512_ctx *c)
{
	sha3_512_init(c);
	return 0;
}

int API SHAKE128_Init(struct shake128_ctx *c)
{
	shake128_init(c);
	return 0;
}

int API SHAKE256_Init(struct shake256_ctx *c)
{
	shake256_init(c);
	return 0;
}

int API SHA3_224_Final(struct sha3_224_ctx *c, unsigned char *md)
{
	sha3_224_digest(c, SHA3_224_DIGEST_SIZE, md);
	return 0;
}

int API SHA3_256_Final(struct sha3_256_ctx *c, unsigned char *md)
{
	sha3_256_digest(c, SHA3_256_DIGEST_SIZE, md);
	return 0;
}

int API SHA3_384_Final(struct sha3_384_ctx *c, unsigned char *md)
{
	sha3_384_digest(c, SHA3_384_DIGEST_SIZE, md);
	return 0;
}

int API SHA3_512_Final(struct sha3_512_ctx *c, unsigned char *md)
{
	sha3_512_digest(c, SHA3_512_DIGEST_SIZE, md);
	return 0;
}

int API SHAKE128_Final(struct shake128_ctx *c, unsigned char *md, size_t l)
{
	shake128_digest(c, l/8, md);
	return 0;
}

int API SHAKE256_Final(struct shake256_ctx *c, unsigned char *md, size_t l)
{
	shake256_digest(c, l/8, md);
	return 0;
}

int API SHA3_224_Update(struct sha3_224_ctx *c, const void *data, size_t len)
{
	sha3_224_update(c, len, data);
	return 0;
}

int API SHA3_256_Update(struct sha3_256_ctx *c, const void *data, size_t len)
{
	sha3_256_update(c, len, data);
	return 0;
}

int API SHA3_384_Update(struct sha3_384_ctx *c, const void *data, size_t len)
{
	sha3_384_update(c, len, data);
	return 0;
}

int API SHA3_512_Update(struct sha3_512_ctx *c, const void *data, size_t len)
{
	sha3_512_update(c, len, data);
	return 0;
}

int API SHAKE128_Update(struct shake128_ctx *c, const void *data, size_t len)
{
	shake128_update(c, len, data);
	return 0;
}

int API SHAKE256_Update(struct shake256_ctx *c, const void *data, size_t len)
{
	shake256_update(c, len, data);
	return 0;
}

unsigned char * API SHA3_224(const void *d, size_t n, unsigned char *md)
{
	struct sha3_224_ctx c;
	static unsigned char m[SHA3_224_DIGEST_SIZE];

	if (md == NULL) md = m;
	sha3_224_init(&c);
	sha3_224_update(&c, n, d);
	sha3_224_digest(&c, SHA3_224_DIGEST_SIZE, md);
	return (md);
}

unsigned char * API SHA3_256(const void *d, size_t n, unsigned char *md)
{
	struct sha3_256_ctx c;
	static unsigned char m[SHA3_256_DIGEST_SIZE];

	if (md == NULL) md = m;
	sha3_256_init(&c);
	sha3_256_update(&c, n, d);
	sha3_256_digest(&c, SHA3_256_DIGEST_SIZE, md);
	return (md);
}

unsigned char * API SHA3_384(const void *d, size_t n, unsigned char *md)
{
	struct sha3_384_ctx c;
	static unsigned char m[SHA3_384_DIGEST_SIZE];

	if (md == NULL) md = m;
	sha3_384_init(&c);
	sha3_384_update(&c, n, d);
	sha3_384_digest(&c, SHA3_384_DIGEST_SIZE, md);
	return (md);
}

unsigned char * API SHA3_512(const void *d, size_t n, unsigned char *md)
{
	struct sha3_512_ctx c;
	static unsigned char m[SHA3_512_DIGEST_SIZE];

	if (md == NULL) md = m;
	sha3_512_init(&c);
	sha3_512_update(&c, n, d);
	sha3_512_digest(&c, SHA3_512_DIGEST_SIZE, md);
	return (md);
}

unsigned char * API SHAKE128(const void *d, size_t n, unsigned char *md, size_t l)
{
	struct shake128_ctx c;
	static unsigned char m[SHA3_512_DIGEST_SIZE];

	if (md == NULL) {
		if (l > SHA3_512_DIGEST_SIZE * 8)
			return NULL;
		else
			md = m;
	}
	shake128_init(&c);
	shake128_update(&c, n, d);
	shake128_digest(&c, l/8, md);
	return (md);
}

unsigned char * API SHAKE256(const void *d, size_t n, unsigned char *md, size_t l)
{
	struct shake256_ctx c;
	static unsigned char m[SHA3_512_DIGEST_SIZE];

	if (md == NULL) {
		if (l > SHA3_512_DIGEST_SIZE * 8)
			return NULL;
		else
			md = m;
	}
	shake256_init(&c);
	shake256_update(&c, n, d);
	shake256_digest(&c, l/8, md);
	return (md);
}

