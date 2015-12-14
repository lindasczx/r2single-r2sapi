#include <openssl/crypto.h>
#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/mdc2.h>
#include <openssl/ripemd.h>
#include <openssl/sha.h>
#include <openssl/whrlpool.h>
#include "r2sapi.h"

#ifdef  __cplusplus
extern "C" {
#endif

unsigned int OPENSSL_ia32cap_P[2];
void OPENSSL_cpuid_setup(void){};

//unsigned long API stdc_SSLeay(void)
//{
//	return SSLeay();
//}
//
//const char * API stdc_SSLeay_version(int t)
//{
//	return SSLeay_version(t);
//}

////////////////MD4
int API stdc_MD4_Init(MD4_CTX *c)
{
	MD4_Init(c);
	return 0;
}
int API stdc_MD4_Update(MD4_CTX *c, const void *data, size_t len)
{
	MD4_Update(c, data, len);
	return 0;
}
int API stdc_MD4_Final(MD4_CTX *c, unsigned char *md)
{
	MD4_Final(md, c);
	return 0;
}
unsigned char * API stdc_MD4(const void *d, size_t n, unsigned char *md)
{
	MD4(d, n, md);
	return 0;
}

////////////////MD5
int API stdc_MD5_Init(MD5_CTX *c)
{
	MD5_Init(c);
	return 0;
}
int API stdc_MD5_Update(MD5_CTX *c, const void *data, size_t len)
{
	MD5_Update(c, data, len);
	return 0;
}
int API stdc_MD5_Final(MD5_CTX *c, unsigned char *md)
{
	MD5_Final(md, c);
	return 0;
}
unsigned char * API stdc_MD5(const void *d, size_t n, unsigned char *md)
{
	MD5(d, n, md);
	return 0;
}

////////////////MDC-2
int API stdc_MDC2_Init(MDC2_CTX *c)
{
	MDC2_Init(c);
	return 0;
}
int API stdc_MDC2_Update(MDC2_CTX *c, const void *data, size_t len)
{
	MDC2_Update(c, data, len);
	return 0;
}
int API stdc_MDC2_Final(MDC2_CTX *c, unsigned char *md)
{
	MDC2_Final(md, c);
	return 0;
}
unsigned char * API stdc_MDC2(const void *d, size_t n, unsigned char *md)
{
	MDC2(d, n, md);
	return 0;
}

////////////////RIPEMD160
int API stdc_RIPEMD160_Init(RIPEMD160_CTX *c)
{
	RIPEMD160_Init(c);
	return 0;
}
int API stdc_RIPEMD160_Update(RIPEMD160_CTX *c, const void *data, size_t len)
{
	RIPEMD160_Update(c, data, len);
	return 0;
}
int API stdc_RIPEMD160_Final(RIPEMD160_CTX *c, unsigned char *md)
{
	RIPEMD160_Final(md, c);
	return 0;
}
unsigned char * API stdc_RIPEMD160(const void *d, size_t n, unsigned char *md)
{
	RIPEMD160(d, n, md);
	return 0;
}

////////////////SHA-1
int API stdc_SHA1_Init(SHA_CTX *c)
{
	SHA1_Init(c);
	return 0;
}
int API stdc_SHA1_Update(SHA_CTX *c, const void *data, size_t len)
{
	SHA1_Update(c, data, len);
	return 0;
}
int API stdc_SHA1_Final(SHA_CTX *c, unsigned char *md)
{
	SHA1_Final(md, c);
	return 0;
}
unsigned char * API stdc_SHA1(const void *d, size_t n, unsigned char *md)
{
	SHA1(d, n, md);
	return 0;
}

////////////////SHA-224
int API stdc_SHA224_Init(SHA256_CTX *c)
{
	SHA224_Init(c);
	return 0;
}
unsigned char * API stdc_SHA224(const void *d, size_t n, unsigned char *md)
{
	SHA224(d, n, md);
	return 0;
}

////////////////SHA-256
int API stdc_SHA256_Init(SHA256_CTX *c)
{
	SHA256_Init(c);
	return 0;
}
int API stdc_SHA256_Update(SHA256_CTX *c, const void *data, size_t len)
{
	SHA256_Update(c, data, len);
	return 0;
}
int API stdc_SHA256_Final(SHA256_CTX *c, unsigned char *md)
{
	SHA256_Final(md, c);
	return 0;
}
unsigned char * API stdc_SHA256(const void *d, size_t n, unsigned char *md)
{
	SHA256(d, n, md);
	return 0;
}

////////////////SHA-384
int API stdc_SHA384_Init(SHA512_CTX *c)
{
	SHA384_Init(c);
	return 0;
}
unsigned char * API stdc_SHA384(const void *d, size_t n, unsigned char *md)
{
	SHA384(d, n, md);
	return 0;
}

////////////////SHA-512
int API stdc_SHA512_Init(SHA512_CTX *c)
{
	SHA512_Init(c);
	return 0;
}
int API stdc_SHA512_Update(SHA512_CTX *c, const void *data, size_t len)
{
	SHA512_Update(c, data, len);
	return 0;
}
int API stdc_SHA512_Final(SHA512_CTX *c, unsigned char *md)
{
	SHA512_Final(md, c);
	return 0;
}
unsigned char * API stdc_SHA512(const void *d, size_t n, unsigned char *md)
{
	SHA512(d, n, md);
	return 0;
}

////////////////WHIRLPOOL
int API stdc_WHIRLPOOL_Init(WHIRLPOOL_CTX *c)
{
	WHIRLPOOL_Init(c);
	return 0;
}
int API stdc_WHIRLPOOL_Update(WHIRLPOOL_CTX *c, const void *data, size_t len)
{
	WHIRLPOOL_Update(c, data, len);
	return 0;
}
int API stdc_WHIRLPOOL_Final(WHIRLPOOL_CTX *c, unsigned char *md)
{
	WHIRLPOOL_Final(md, c);
	return 0;
}
unsigned char * API stdc_WHIRLPOOL(const void *d, size_t n, unsigned char *md)
{
	WHIRLPOOL(d, n, md);
	return 0;
}

#ifdef  __cplusplus
}
#endif
