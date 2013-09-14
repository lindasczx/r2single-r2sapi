#include <stddef.h>
#include "blake2s.hpp"

extern "C" {

typedef blake2sp_state BLAKE2SP_CTX;

int __stdcall BLAKE2SP_Init(BLAKE2SP_CTX *c)
{
	int i;
	for (i=0; i<8; i++)
		c->S[i].set_pointers();
	c->R.set_pointers();

	blake2sp_init(c);
	return 0;
}

int __stdcall BLAKE2SP_Update(BLAKE2SP_CTX *c, const void *data, size_t len)
{
	blake2sp_update(c, (unsigned char *)data, len);
	return 0;
}

int __stdcall BLAKE2SP_Final(unsigned char *md, BLAKE2SP_CTX *c)
{
	blake2sp_final(c, md);
	return 0;
}

int __stdcall BLAKE2SP(const void *data, size_t len, unsigned char *md)
{
	BLAKE2SP_CTX c;
	blake2sp_init(&c);
	blake2sp_update(&c, (unsigned char *)data, len);
	blake2sp_final(&c, md);
	return 0;
}

}
