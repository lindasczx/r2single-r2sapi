#include <stddef.h>
#include "blake2s.h"
#include "r2sapi.h"

typedef blake2sp_state BLAKE2SP_CTX;

int API_ALIGN16 BLAKE2SP_Init(BLAKE2SP_CTX *c)
{
	blake2sp_init(c);
	return 0;
}

int API_ALIGN16 BLAKE2SP_Update(BLAKE2SP_CTX *c, const void *data, size_t len)
{
	blake2sp_update(c, (unsigned char *)data, len);
	return 0;
}

int API_ALIGN16 BLAKE2SP_Final(BLAKE2SP_CTX *c, unsigned char *md)
{
	blake2sp_final(c, md);
	return 0;
}

int API_ALIGN16 BLAKE2SP(const void *data, size_t len, unsigned char *md)
{
	BLAKE2SP_CTX c;
	blake2sp_init(&c);
	blake2sp_update(&c, (unsigned char *)data, len);
	blake2sp_final(&c, md);
	return 0;
}

