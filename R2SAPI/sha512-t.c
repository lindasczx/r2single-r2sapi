#include <openssl/sha.h>

int __stdcall SHA512_224_Init(SHA512_CTX *c)
{
	c->h[0] = U64(0x8C3D37C819544DA2);
	c->h[1] = U64(0x73E1996689DCD4D6);
	c->h[2] = U64(0x1DFAB7AE32FF9C82);
	c->h[3] = U64(0x679DD514582F9FCF);
	c->h[4] = U64(0x0F6D2B697BD44DA8);
	c->h[5] = U64(0x77E36F7304C48942);
	c->h[6] = U64(0x3F9D85A86A1D36C8);
	c->h[7] = U64(0x1112E6AD91D692A1);

        c->Nl = 0;      c->Nh = 0;
        c->num = 0;     c->md_len = SHA224_DIGEST_LENGTH;
        return 1;
}

int __stdcall SHA512_256_Init(SHA512_CTX *c)
{
	c->h[0] = U64(0x22312194FC2BF72C);
	c->h[1] = U64(0x9F555FA3C84C64C2);
	c->h[2] = U64(0x2393B86B6F53B151);
	c->h[3] = U64(0x963877195940EABD);
	c->h[4] = U64(0x96283EE2A88EFFE3);
	c->h[5] = U64(0xBE5E1E2553863992);
	c->h[6] = U64(0x2B0199FC2C85B8AA);
	c->h[7] = U64(0x0EB72DDC81C52CA2);

        c->Nl = 0;      c->Nh = 0;
        c->num = 0;     c->md_len = SHA256_DIGEST_LENGTH;
        return 1;
}

int __stdcall SHA512_224_Final (unsigned char *md, SHA512_CTX *c)
{
	return SHA512_Final(md, c);
}

int __stdcall SHA512_256_Final (unsigned char *md, SHA512_CTX *c)
{
	return SHA512_Final(md, c);
}

int __stdcall SHA512_224_Update (SHA512_CTX *c, const void *data, size_t len)
{
	return SHA512_Update(c, data, len);
}

int __stdcall SHA512_256_Update (SHA512_CTX *c, const void *data, size_t len)
{
	return SHA512_Update(c, data, len);
}

unsigned char * __stdcall SHA512_224(const unsigned char *d, size_t n, unsigned char *md)
{
	SHA512_CTX c;
	static unsigned char m[SHA224_DIGEST_LENGTH];

	if (md == NULL) md = m;
	SHA512_224_Init(&c);
	SHA512_Update(&c, d, n);
	SHA512_Final(md, &c);
	return (md);
}

unsigned char * __stdcall SHA512_256(const unsigned char *d, size_t n, unsigned char *md)
{
	SHA512_CTX c;
	static unsigned char m[SHA256_DIGEST_LENGTH];

	if (md == NULL) md = m;
	SHA512_256_Init(&c);
	SHA512_Update(&c, d, n);
	SHA512_Final(md, &c);
	return (md);
}
