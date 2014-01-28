#include <openssl/crypto.h>
#include <openssl/md2.h>
#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/mdc2.h>
#include <openssl/ripemd.h>
#include <openssl/sha.h>

#ifdef  __cplusplus
extern "C" {
#endif

unsigned long __stdcall stdc_SSLeay(void)
{
	return SSLeay();
}

const char * __stdcall stdc_SSLeay_version(int t)
{
	return SSLeay_version(t);
}

////////////////MD2
int __stdcall stdc_MD2_Init(MD2_CTX *c)
{
	return MD2_Init(c);
}
int __stdcall stdc_MD2_Update(MD2_CTX *c, const void *data, size_t len)
{
	return MD2_Update(c, data, len);
}
int __stdcall stdc_MD2_Final(unsigned char *md, MD2_CTX *c)
{
	return MD2_Final(md, c);
}
unsigned char * __stdcall stdc_MD2(const unsigned char *d, size_t n, unsigned char *md)
{
	return MD2(d, n, md);
}

////////////////MD4
int __stdcall stdc_MD4_Init(MD4_CTX *c)
{
	return MD4_Init(c);
}
int __stdcall stdc_MD4_Update(MD4_CTX *c, const void *data, size_t len)
{
	return MD4_Update(c, data, len);
}
int __stdcall stdc_MD4_Final(unsigned char *md, MD4_CTX *c)
{
	return MD4_Final(md, c);
}
unsigned char * __stdcall stdc_MD4(const unsigned char *d, size_t n, unsigned char *md)
{
	return MD4(d, n, md);
}

////////////////MD5
int __stdcall stdc_MD5_Init(MD5_CTX *c)
{
	return MD5_Init(c);
}
int __stdcall stdc_MD5_Update(MD5_CTX *c, const void *data, size_t len)
{
	return MD5_Update(c, data, len);
}
int __stdcall stdc_MD5_Final(unsigned char *md, MD5_CTX *c)
{
	return MD5_Final(md, c);
}
unsigned char * __stdcall stdc_MD5(const unsigned char *d, size_t n, unsigned char *md)
{
	return MD5(d, n, md);
}

////////////////MDC-2
int __stdcall stdc_MDC2_Init(MDC2_CTX *c)
{
	return MDC2_Init(c);
}
int __stdcall stdc_MDC2_Update(MDC2_CTX *c, const void *data, size_t len)
{
	return MDC2_Update(c, data, len);
}
int __stdcall stdc_MDC2_Final(unsigned char *md, MDC2_CTX *c)
{
	return MDC2_Final(md, c);
}
unsigned char * __stdcall stdc_MDC2(const unsigned char *d, size_t n, unsigned char *md)
{
	return MDC2(d, n, md);
}

////////////////RIPEMD160
int __stdcall stdc_RIPEMD160_Init(RIPEMD160_CTX *c)
{
	return RIPEMD160_Init(c);
}
int __stdcall stdc_RIPEMD160_Update(RIPEMD160_CTX *c, const void *data, size_t len)
{
	return RIPEMD160_Update(c, data, len);
}
int __stdcall stdc_RIPEMD160_Final(unsigned char *md, RIPEMD160_CTX *c)
{
	return RIPEMD160_Final(md, c);
}
unsigned char * __stdcall stdc_RIPEMD160(const unsigned char *d, size_t n, unsigned char *md)
{
	return RIPEMD160(d, n, md);
}

////////////////SHA-1
int __stdcall stdc_SHA1_Init(SHA_CTX *c)
{
	return SHA1_Init(c);
}
int __stdcall stdc_SHA1_Update(SHA_CTX *c, const void *data, size_t len)
{
	return SHA1_Update(c, data, len);
}
int __stdcall stdc_SHA1_Final(unsigned char *md, SHA_CTX *c)
{
	return SHA1_Final(md, c);
}
unsigned char * __stdcall stdc_SHA1(const unsigned char *d, size_t n, unsigned char *md)
{
	return SHA1(d, n, md);
}

////////////////SHA-224
int __stdcall stdc_SHA224_Init(SHA256_CTX *c)
{
	return SHA224_Init(c);
}
int __stdcall stdc_SHA224_Update(SHA256_CTX *c, const void *data, size_t len)
{
	return SHA224_Update(c, data, len);
}
int __stdcall stdc_SHA224_Final(unsigned char *md, SHA256_CTX *c)
{
	return SHA224_Final(md, c);
}
unsigned char * __stdcall stdc_SHA224(const unsigned char *d, size_t n, unsigned char *md)
{
	return SHA224(d, n, md);
}

////////////////SHA-256
int __stdcall stdc_SHA256_Init(SHA256_CTX *c)
{
	return SHA256_Init(c);
}
int __stdcall stdc_SHA256_Update(SHA256_CTX *c, const void *data, size_t len)
{
	return SHA256_Update(c, data, len);
}
int __stdcall stdc_SHA256_Final(unsigned char *md, SHA256_CTX *c)
{
	return SHA256_Final(md, c);
}
unsigned char * __stdcall stdc_SHA256(const unsigned char *d, size_t n, unsigned char *md)
{
	return SHA256(d, n, md);
}

////////////////SHA-384
int __stdcall stdc_SHA384_Init(SHA512_CTX *c)
{
	return SHA384_Init(c);
}
int __stdcall stdc_SHA384_Update(SHA512_CTX *c, const void *data, size_t len)
{
	return SHA384_Update(c, data, len);
}
int __stdcall stdc_SHA384_Final(unsigned char *md, SHA512_CTX *c)
{
	return SHA384_Final(md, c);
}
unsigned char * __stdcall stdc_SHA384(const unsigned char *d, size_t n, unsigned char *md)
{
	return SHA384(d, n, md);
}

////////////////SHA-512
int __stdcall stdc_SHA512_Init(SHA512_CTX *c)
{
	return SHA512_Init(c);
}
int __stdcall stdc_SHA512_Update(SHA512_CTX *c, const void *data, size_t len)
{
	return SHA512_Update(c, data, len);
}
int __stdcall stdc_SHA512_Final(unsigned char *md, SHA512_CTX *c)
{
	return SHA512_Final(md, c);
}
unsigned char * __stdcall stdc_SHA512(const unsigned char *d, size_t n, unsigned char *md)
{
	return SHA512(d, n, md);
}

#ifdef  __cplusplus
}
#endif
