/*++
Module Name:
	r2sapi.h

Version:
	1.35.0.160
--*/

#ifndef R2SAPI_H_
#define R2SAPI_H_

#ifdef __cplusplus
extern "C"{
#endif

#define EXPORT extern "C"
#define API __stdcall

#include <sys/types.h>
#include <inttypes.h>
typedef void * LPVOID;
typedef char * LPSTR;
typedef const char * LPCSTR;
typedef wchar_t * LPWSTR;
typedef const wchar_t * LPCWSTR;
typedef int * LPLSTR;
typedef const int * LPCLSTR;

#define cpINVALID (-1)
#define cpUTF8    1
#define cpUTF16LE 2
#define cpUTF16BE 3
#define cpUTF16   cpUTF16LE
#define cpUTF32LE 4
#define cpUTF32BE 5
#define cpUTF32   cpUTF32LE
#define cpGB18030 0x100

#define MAKETRCODE(a,b) (((a)<<16)|(b))

#define MD4_LBLOCK 16
typedef struct MD4state_st
{
	uint32_t A,B,C,D;
	uint32_t Nl,Nh;
	uint32_t data[MD4_LBLOCK];
	unsigned int num;
} MD4_CTX, MD5_CTX;
#define md6_b 64
#define md6_c 16
#define md6_k 8
#define md6_max_stack_height 29
#define md6_w 64
typedef struct md6_state
{
	int d;
	int hashbitlen;
	unsigned char hashval[ md6_c*(md6_w/8) ];
	unsigned char hexhashval[(md6_c*(md6_w/8))+1];
	int initialized;
	uint64_t bits_processed;
	uint64_t compression_calls;
	int finalized;
	uint64_t K[ md6_k ];
	int keylen;
	int L;
	int r;
	int top;
	uint64_t B[ md6_max_stack_height ][ md6_b ];
	unsigned int bits[ md6_max_stack_height ];
	uint64_t i_for_level[ md6_max_stack_height ];
} MD6_CTX;
#define MDC2_BLOCK 8
typedef unsigned char DES_cblock[8];
typedef struct mdc2_ctx_st
{
	unsigned int num;
	unsigned char data[MDC2_BLOCK];
	DES_cblock h,hh;
	int pad_type;
} MDC2_CTX;
#define RIPEMD160_LBLOCK 16
typedef struct RIPEMD160state_st
{
	uint32_t A,B,C,D,E;
	uint32_t Nl,Nh;
	uint32_t data[RIPEMD160_LBLOCK];
	unsigned int num;
} RIPEMD160_CTX;
#define SHA_LBLOCK 16
typedef struct SHAstate_st
{
	uint32_t h0,h1,h2,h3,h4;
	uint32_t Nl,Nh;
	uint32_t data[SHA_LBLOCK];
	unsigned int num;
} SHA_CTX;
typedef struct SHA256state_st
{
	uint32_t h[8];
	uint32_t Nl,Nh;
	uint32_t data[SHA_LBLOCK];
	unsigned int num,md_len;
} SHA256_CTX;
#define SHA512_CBLOCK (SHA_LBLOCK*8)
typedef struct SHA512state_st
{
	uint64_t h[8];
	uint64_t Nl,Nh;
	union {
		uint64_t d[SHA_LBLOCK];
		unsigned char p[SHA512_CBLOCK];
	} u;
	unsigned int num,md_len;
} SHA512_CTX;

EXPORT int API _();

////utf
EXPORT int API cpConvertEncoding(unsigned int nTrCode, LPVOID lpSrcStr, int cchSrc, LPVOID lpDestStr, int cchDest);
EXPORT int API cpTrCodeSupported(unsigned int nTrCode);
EXPORT int API UTF8ToUTF16(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
EXPORT int API UTF8ToUTF32(LPCSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
EXPORT int API UTF16ToUTF8(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
EXPORT int API UTF16ToUTF16BE(LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
EXPORT int API UTF16ToUTF32(LPCWSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
EXPORT int API UTF16ToUTF32BE(LPCWSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
EXPORT int API UTF16BEToUTF16(LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
EXPORT int API UTF32ToUTF8(LPCLSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
EXPORT int API UTF32ToUTF16(LPCLSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
EXPORT int API UTF32ToUTF32BE(LPCLSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
EXPORT int API UTF32BEToUTF16(LPCLSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
EXPORT int API UTF32BEToUTF32(LPCLSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);

////libcrypto
EXPORT int API MD4_Init(MD4_CTX *c);
EXPORT int API MD4_Update(MD4_CTX *c, const void *data, size_t len);
EXPORT int API MD4_Final(unsigned char *md, MD4_CTX *c);
EXPORT unsigned char * API MD4(const unsigned char *d, size_t n, unsigned char *md);
EXPORT int API MD5_Init(MD5_CTX *c);
EXPORT int API MD5_Update(MD5_CTX *c, const void *data, size_t len);
EXPORT int API MD5_Final(unsigned char *md, MD5_CTX *c);
EXPORT unsigned char * API MD5(const unsigned char *d, size_t n, unsigned char *md);
EXPORT int API MD6_Init(MD6_CTX *c);
EXPORT int API MD6_Init_Len(MD6_CTX *c, int mdlen);
EXPORT int API MD6_Update(MD6_CTX *c, const void *data, size_t len);
EXPORT int API MD6_Final(unsigned char *md, MD6_CTX *c);
EXPORT unsigned char * API MD6(const unsigned char *d, size_t n, unsigned char *md);
EXPORT unsigned char * API MD6_Len(const unsigned char *d, size_t n, unsigned char *md, int mdlen);
EXPORT int API MDC2_Init(MDC2_CTX *c);
EXPORT int API MDC2_Update(MDC2_CTX *c, const void *data, size_t len);
EXPORT int API MDC2_Final(unsigned char *md, MDC2_CTX *c);
EXPORT unsigned char * API MDC2(const unsigned char *d, size_t n, unsigned char *md);
EXPORT int API RIPEMD160_Init(RIPEMD160_CTX *c);
EXPORT int API RIPEMD160_Update(RIPEMD160_CTX *c, const void *data, size_t len);
EXPORT int API RIPEMD160_Final(unsigned char *md, RIPEMD160_CTX *c);
EXPORT unsigned char * API RIPEMD160(const unsigned char *d, size_t n, unsigned char *md);
EXPORT int API SHA1_Init(SHA_CTX *c);
EXPORT int API SHA1_Update(SHA_CTX *c, const void *data, size_t len);
EXPORT int API SHA1_Final(unsigned char *md, SHA_CTX *c);
EXPORT unsigned char * API SHA1(const unsigned char *d, size_t n, unsigned char *md);
//EXPORT int API SHA224_Init(SHA256_CTX *c);
//EXPORT int API SHA224_Update(SHA256_CTX *c, const void *data, size_t len);
//EXPORT int API SHA224_Final(unsigned char *md, SHA256_CTX *c);
//EXPORT unsigned char * API SHA224(const unsigned char *d, size_t n, unsigned char *md);
EXPORT int API SHA256_Init(SHA256_CTX *c);
EXPORT int API SHA256_Update(SHA256_CTX *c, const void *data, size_t len);
EXPORT int API SHA256_Final(unsigned char *md, SHA256_CTX *c);
EXPORT unsigned char * API SHA256(const unsigned char *d, size_t n, unsigned char *md);
//EXPORT int API SHA384_Init(SHA512_CTX *c);
//EXPORT int API SHA384_Update(SHA512_CTX *c, const void *data, size_t len);
//EXPORT int API SHA384_Final(unsigned char *md, SHA512_CTX *c);
//EXPORT unsigned char * API SHA384(const unsigned char *d, size_t n, unsigned char *md);
EXPORT int API SHA512_Init(SHA512_CTX *c);
EXPORT int API SHA512_Update(SHA512_CTX *c, const void *data, size_t len);
EXPORT int API SHA512_Final(unsigned char *md, SHA512_CTX *c);
EXPORT unsigned char * API SHA512(const unsigned char *d, size_t n, unsigned char *md);

////westpak
EXPORT long API GetFileFromPakA(void* pBuf, long ulBufLen, LPCSTR pszFn, LPCSTR pszFnWant);
EXPORT long API GetFileFromPakW(void* pBuf, long ulBufLen, LPCWSTR pszFn, LPCWSTR pszFnWant);

////xmlparser
EXPORT int     API XMLPickAttribW(LPWSTR, LPCWSTR, LPCWSTR, long, LPCWSTR);
EXPORT int     API XMLPickAttribA(LPSTR, LPCSTR, LPCSTR, long, LPCSTR);
EXPORT int     API XMLPickAttribPosW(long&, long&, LPCWSTR, LPCWSTR, long, LPCWSTR);
EXPORT int     API XMLPickAttribPosA(long&, long&, LPCSTR, LPCSTR, long, LPCSTR);
EXPORT int     API XMLPickFullTagW(LPWSTR, LPCWSTR, LPCWSTR, long&, long);
EXPORT int     API XMLPickFullTagA(LPSTR, LPCSTR, LPCSTR, long&, long);
EXPORT int     API XMLPickFullTagPosW(long&, long&, LPCWSTR, LPCWSTR, long&, long);
EXPORT int     API XMLPickFullTagPosA(long&, long&, LPCSTR, LPCSTR, long&, long);
EXPORT int     API XMLPickTagW(LPWSTR, LPCWSTR, LPCWSTR, long);
EXPORT int     API XMLPickTagA(LPSTR, LPCSTR, LPCSTR, long);
EXPORT int     API XMLPickTag2W(LPWSTR, LPCWSTR, LPCWSTR, long&, long);
EXPORT int     API XMLPickTag2A(LPSTR, LPCSTR, LPCSTR, long&, long);
EXPORT int     API XMLPickTagPosW(long&, long&, LPCWSTR, LPCWSTR, long);
EXPORT int     API XMLPickTagPosA(long&, long&, LPCSTR, LPCSTR, long);

////zlib
EXPORT int API compress(char *dest, unsigned long *destLen, const char *source, unsigned long sourceLen);
EXPORT int API compress2(char *dest, unsigned long *destLen, const char *source, unsigned long sourceLen, int level);
EXPORT unsigned long API compressBound(unsigned long sourceLen);
EXPORT unsigned long API crc32(unsigned long crc, const char *buf, unsigned int len);
EXPORT unsigned long API crc32_combine(unsigned long, unsigned long, long);
//EXPORT unsigned long API crc32_combine64(unsigned long, unsigned long, long long);
EXPORT const unsigned long * API get_crc_table(void);
EXPORT int API uncompress(char *dest, unsigned long *destLen, const char *source, unsigned long sourceLen);
EXPORT const char * API zlibVersion(void);

#ifdef __cplusplus
}
#endif

#endif

