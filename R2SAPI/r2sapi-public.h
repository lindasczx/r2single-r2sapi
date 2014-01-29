/*++
Module Name:
	r2sapi.h

Version:
	1.36.0.259
--*/

#ifndef R2SAPI_H_
#define R2SAPI_H_

#ifdef __cplusplus
extern "C"{
#endif

#define API __stdcall

#include <sys/types.h>
#include <inttypes.h>
typedef void * LPVOID;
typedef const void * LPCVOID;
typedef char * LPSTR;
typedef const char * LPCSTR;
typedef wchar_t * LPWSTR;
typedef const wchar_t * LPCWSTR;
typedef int * LPLSTR;
typedef const int * LPCLSTR;

#define cpINVALID   (-1)
#define cpUTF8      1
#define cpUTF16LE   2
#define cpUTF16BE   3
#define cpUTF16     cpUTF16LE
#define cpUTF32LE   4
#define cpUTF32BE   5
#define cpUTF32     cpUTF32LE
#define cpUTFEBCDIC 0x40
#define cpUTF1      0x41
#define cpCESU8     0x80
#define cpUTF7      0xa0
#define cpGB18030   0x100

#define MAKETRCODE(a,b) ( (uint32_t)( (((uint32_t)(uint16_t)a)<<16) | ((uint32_t)(uint16_t)b) ) )
//unsigned int WINAPI cpMakeTrCode(unsigned short cpfrom, unsigned short cpto);

#define MD2_BLOCK 16
typedef struct MD2state_st
{
	unsigned int num;
	unsigned char data[MD2_BLOCK];
	uint32_t cksm[MD2_BLOCK];
	uint32_t state[MD2_BLOCK];
} MD2_CTX;
#define MD4_LBLOCK 16
typedef struct MD4state_st
{
	uint32_t A,B,C,D;
	uint32_t Nl,Nh;
	uint32_t data[MD4_LBLOCK];
	unsigned int num;
} MD4_CTX, MD5_CTX;
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
#define BLAKE2S_BLOCKBYTES 64
#define BLAKE2S_OUTBYTES 32
#define BLAKE_ALIGNMENT 64
#define BLAKE_DATA_SIZE (48 + 2 * BLAKE2S_BLOCKBYTES)
typedef struct blake2s_state
{
	unsigned char ubuf[BLAKE_DATA_SIZE + BLAKE_ALIGNMENT];
	unsigned char *buf;       // byte   buf[2 * BLAKE2S_BLOCKBYTES].
	unsigned int *h, *t, *f; // uint32 h[8], t[2], f[2].
	size_t   buflen;
	unsigned char last_node;
} BLAKE2S_CTX;
typedef struct blake2sp_state
{
	BLAKE2S_CTX S[8];
	BLAKE2S_CTX R;
	unsigned char buf[8 * BLAKE2S_BLOCKBYTES];
	size_t buflen;
} BLAKE2SP_CTX;
typedef struct sm3_context
{
	unsigned int total[2]; /*!< number of bytes processed */
	unsigned int state[8]; /*!< intermediate digest state */
	unsigned char buffer[64]; /*!< data block being processed */
} SM3_CTX;

int API _();
#define R2SAPIVer (_())

///////////////////////////////////////////
//////// 1. R2Beat PAK文件格式相关
///////////////////////////////////////////

////westpak
long API GetFileFromPakA(void* pBuf, long ulBufLen, LPCSTR pszFn, LPCSTR pszFnWant);
long API GetFileFromPakW(void* pBuf, long ulBufLen, LPCWSTR pszFn, LPCWSTR pszFnWant);

///////////////////////////////////////////
//////// 2. 解析XML
///////////////////////////////////////////

////xmlparser
int API XMLPickAttribW(LPWSTR, LPCWSTR, LPCWSTR, long, LPCWSTR);
int API XMLPickAttribA(LPSTR, LPCSTR, LPCSTR, long, LPCSTR);
int API XMLPickAttrib2W(LPWSTR, long, LPCWSTR, LPCWSTR, long, LPCWSTR);
int API XMLPickAttrib2A(LPSTR, long, LPCSTR, LPCSTR, long, LPCSTR);
int API XMLPickAttribPosW(long*, long*, LPCWSTR, LPCWSTR, long, LPCWSTR);
int API XMLPickAttribPosA(long*, long*, LPCSTR, LPCSTR, long, LPCSTR);
int API XMLPickFullTagW(LPWSTR, LPCWSTR, LPCWSTR, long*, long);
int API XMLPickFullTagA(LPSTR, LPCSTR, LPCSTR, long*, long);
int API XMLPickFullTag2W(LPWSTR, long, LPCWSTR, LPCWSTR, long*, long);
int API XMLPickFullTag2A(LPSTR, long, LPCSTR, LPCSTR, long*, long);
int API XMLPickFullTagPosW(long*, long*, LPCWSTR, LPCWSTR, long*, long);
int API XMLPickFullTagPosA(long*, long*, LPCSTR, LPCSTR, long*, long);
int API XMLPickTagW(LPWSTR, LPCWSTR, LPCWSTR, long);
int API XMLPickTagA(LPSTR, LPCSTR, LPCSTR, long);
int API XMLPickTag2W(LPWSTR, LPCWSTR, LPCWSTR, long*, long);
int API XMLPickTag2A(LPSTR, LPCSTR, LPCSTR, long*, long);
int API XMLPickTag3W(LPWSTR, long, LPCWSTR, LPCWSTR, long*, long);
int API XMLPickTag3A(LPSTR, long, LPCSTR, LPCSTR, long*, long);
int API XMLPickTagPosW(long*, long*, LPCWSTR, LPCWSTR, long*, long);
int API XMLPickTagPosA(long*, long*, LPCSTR, LPCSTR, long*, long);

///////////////////////////////////////////
//////// 3. Hash函数
///////////////////////////////////////////

////libcrypto
int API MD2_Init(MD2_CTX *c);
int API MD2_Update(MD2_CTX *c, const void *data, size_t len);
int API MD2_Final(unsigned char *md, MD2_CTX *c);
unsigned char * API MD2(const unsigned char *d, size_t n, unsigned char *md);
int API MD4_Init(MD4_CTX *c);
int API MD4_Update(MD4_CTX *c, const void *data, size_t len);
int API MD4_Final(unsigned char *md, MD4_CTX *c);
unsigned char * API MD4(const unsigned char *d, size_t n, unsigned char *md);
int API MD5_Init(MD5_CTX *c);
int API MD5_Update(MD5_CTX *c, const void *data, size_t len);
int API MD5_Final(unsigned char *md, MD5_CTX *c);
unsigned char * API MD5(const unsigned char *d, size_t n, unsigned char *md);
int API MDC2_Init(MDC2_CTX *c);
int API MDC2_Update(MDC2_CTX *c, const void *data, size_t len);
int API MDC2_Final(unsigned char *md, MDC2_CTX *c);
unsigned char * API MDC2(const unsigned char *d, size_t n, unsigned char *md);
int API RIPEMD160_Init(RIPEMD160_CTX *c);
int API RIPEMD160_Update(RIPEMD160_CTX *c, const void *data, size_t len);
int API RIPEMD160_Final(unsigned char *md, RIPEMD160_CTX *c);
unsigned char * API RIPEMD160(const unsigned char *d, size_t n, unsigned char *md);
int API SHA1_Init(SHA_CTX *c);
int API SHA1_Update(SHA_CTX *c, const void *data, size_t len);
int API SHA1_Final(unsigned char *md, SHA_CTX *c);
unsigned char * API SHA1(const unsigned char *d, size_t n, unsigned char *md);
int API SHA224_Init(SHA256_CTX *c);
int API SHA224_Update(SHA256_CTX *c, const void *data, size_t len);
int API SHA224_Final(unsigned char *md, SHA256_CTX *c);
unsigned char * API SHA224(const unsigned char *d, size_t n, unsigned char *md);
int API SHA256_Init(SHA256_CTX *c);
int API SHA256_Update(SHA256_CTX *c, const void *data, size_t len);
int API SHA256_Final(unsigned char *md, SHA256_CTX *c);
unsigned char * API SHA256(const unsigned char *d, size_t n, unsigned char *md);
int API SHA384_Init(SHA512_CTX *c);
int API SHA384_Update(SHA512_CTX *c, const void *data, size_t len);
int API SHA384_Final(unsigned char *md, SHA512_CTX *c);
unsigned char * API SHA384(const unsigned char *d, size_t n, unsigned char *md);
int API SHA512_Init(SHA512_CTX *c);
int API SHA512_Update(SHA512_CTX *c, const void *data, size_t len);
int API SHA512_Final(unsigned char *md, SHA512_CTX *c);
unsigned char * API SHA512(const unsigned char *d, size_t n, unsigned char *md);
////nist sha-2 addition
int API SHA512_224_Init(SHA512_CTX *c);
int API SHA512_224_Update(SHA512_CTX *c, const void *data, size_t len);
int API SHA512_224_Final(unsigned char *md, SHA512_CTX *c);
unsigned char * API SHA512_224(const unsigned char *d, size_t n, unsigned char *md);
int API SHA512_256_Init(SHA512_CTX *c);
int API SHA512_256_Update(SHA512_CTX *c, const void *data, size_t len);
int API SHA512_256_Final(unsigned char *md, SHA512_CTX *c);
unsigned char * API SHA512_256(const unsigned char *d, size_t n, unsigned char *md);

////md6
int API MD6_Init(MD6_CTX *c);
int API MD6_Init_Len(MD6_CTX *c, int mdlen);
int API MD6_Update(MD6_CTX *c, const void *data, size_t len);
int API MD6_Final(unsigned char *md, MD6_CTX *c);
unsigned char * API MD6(const unsigned char *d, size_t n, unsigned char *md);
unsigned char * API MD6_Len(const unsigned char *d, size_t n, unsigned char *md, int mdlen);

////blake2sp
int API BLAKE2SP_Init(BLAKE2SP_CTX *c);
int API BLAKE2SP_Update(BLAKE2SP_CTX *c, const void *data, size_t len);
int API BLAKE2SP_Final(unsigned char *md, BLAKE2SP_CTX *c);
unsigned char * API BLAKE2SP(const unsigned char *d, size_t n, unsigned char *md);

////国密SM3
int API SM3_Init(SM3_CTX *c);
int API SM3_Update(SM3_CTX *c, const void *data, size_t len);
int API SM3_Final(unsigned char *md, SM3_CTX *c);
unsigned char * API SM3(const unsigned char *d, size_t n, unsigned char *md);

///////////////////////////////////////////
//////// 4. 压缩解压
///////////////////////////////////////////

////zlib
int API compress(char *dest, unsigned long *destLen, const char *source, unsigned long sourceLen);
int API compress2(char *dest, unsigned long *destLen, const char *source, unsigned long sourceLen, int level);
unsigned long API compressBound(unsigned long sourceLen);
unsigned long API crc32(unsigned long crc, const char *buf, unsigned int len);
unsigned long API crc32_combine(unsigned long, unsigned long, long);
//unsigned long API crc32_combine64(unsigned long, unsigned long, long long);
const unsigned long * API get_crc_table(void);
int API uncompress(char *dest, unsigned long *destLen, const char *source, unsigned long sourceLen);
const char * API zlibVersion(void);

///////////////////////////////////////////
//////// 5. 字符编码转换
///////////////////////////////////////////

////utf
int API cpConvertEncoding(unsigned int nTrCode, LPCVOID lpSrcStr, int cchSrc, LPVOID lpDestStr, int cchDest);
int API cpTrCodeSupported(unsigned int nTrCode);
int API UTF8ToUTF16(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int API UTF8ToUTF32(LPCSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
int API UTF16ToUTF8(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int API UTF16ToUTF16BE(LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int API UTF16ToUTF32(LPCWSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
int API UTF16ToUTF32BE(LPCWSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
int API UTF16BEToUTF16(LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int API UTF32ToUTF8(LPCLSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int API UTF32ToUTF16(LPCLSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int API UTF32ToUTF32BE(LPCLSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
int API UTF32BEToUTF16(LPCLSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int API UTF32BEToUTF32(LPCLSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);

#ifdef __cplusplus
}
#endif

#endif

