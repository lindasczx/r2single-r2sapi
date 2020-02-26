/*++
Module Name:
	r2sapi.h

Version:
	1.50.0.470
--*/

#ifndef R2SAPI_H_
#define R2SAPI_H_

#ifdef __cplusplus
extern "C"{
#endif

#define API __stdcall

#include <sys/types.h>
#if defined(_MSC_VER) && _MSC_VER < 1600
#include "stdint-vc6.h"
#else
#include <stdint.h>
#endif
typedef void * LPVOID;
typedef const void * LPCVOID;
typedef char * LPSTR;
typedef const char * LPCSTR;
typedef wchar_t * LPWSTR;
typedef const wchar_t * LPCWSTR;
typedef int * LPLSTR;
typedef const int * LPCLSTR;
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

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

#define ALGO_NO		0
#define ALGO_LZSS	1
#define ALGO_DIR	2
#define ALGO_LZSS_XOR	3

typedef struct {
	unsigned int Type;
	size_t Offset;
	size_t CompressedSize;
	size_t Size;
	wchar_t FileName[MAX_PATH];
} ENUMPAKFILEW;
typedef struct {
	unsigned int Type;
	size_t Offset;
	size_t CompressedSize;
	size_t Size;
	char FileName[MAX_PATH];
} ENUMPAKFILEA;
typedef int (API ENUMPAKPROCW)(LPCWSTR, const ENUMPAKFILEW *, void *);
typedef int (API ENUMPAKPROCA)(LPCSTR, const ENUMPAKFILEA *, void *);

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
#define SHA3_STATE_LENGTH 25
#define SHA3_MAX_BLOCK_SIZE 168
typedef struct sha3_ctx
{
	uint64_t state[SHA3_STATE_LENGTH];
	unsigned int index;
	uint8_t block[SHA3_MAX_BLOCK_SIZE];
} SHA3_CTX;
#define WHIRLPOOL_DIGEST_LENGTH	(512/8)
#define WHIRLPOOL_BBLOCK	512
#define WHIRLPOOL_COUNTER	(256/8)
typedef struct
{
	union {
		unsigned char c[WHIRLPOOL_DIGEST_LENGTH];
		/* double q is here to ensure 64-bit alignment */
		double q[WHIRLPOOL_DIGEST_LENGTH/sizeof(double)];
	} H;
	unsigned char data[WHIRLPOOL_BBLOCK/8];
	unsigned int bitoff;
	size_t bitlen[WHIRLPOOL_COUNTER/sizeof(size_t)];
} WHIRLPOOL_CTX;
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
#define SM3_LBLOCK	(64/4)
typedef struct SM3state_st
{
	unsigned int A, B, C, D, E, F, G, H;
	unsigned int Nl, Nh;
	unsigned int data[SM3_LBLOCK];
	unsigned int num;
} SM3_CTX;

int API _();
#define R2SAPIVer (_())

///////////////////////////////////////////
//////// 1. R2Beat PAK文件格式相关
///////////////////////////////////////////

////westpak
#define EnumFileFromPakA EnumFileFromPak2A
#define EnumFileFromPakW EnumFileFromPak2W
size_t API EnumFileFromPak2A(LPCSTR PakFileName, ENUMPAKPROCA lpEnumFunc, void* Param);
size_t API EnumFileFromPak2W(LPCWSTR PakFileName, ENUMPAKPROCW lpEnumFunc, void* Param);
size_t API GetFileFromPakA(void* pBuf, size_t BufLen, LPCSTR PakFileName, LPCSTR FileWant);
size_t API GetFileFromPakW(void* pBuf, size_t BufLen, LPCWSTR PakFileName, LPCWSTR FileWant);
#define GetFileFromPakOffsetA GetFileFromPakOffset2A
#define GetFileFromPakOffsetW GetFileFromPakOffset2W
size_t API GetFileFromPakOffset2A(void* pBuf, size_t BufLen, LPCSTR PakFileName, size_t Offset, size_t CompressedSize, int algo);
size_t API GetFileFromPakOffset2W(void* pBuf, size_t BufLen, LPCWSTR PakFileName, size_t Offset, size_t CompressedSize, int algo);
void API LzssCompress(const void* InBuf, size_t InLen, void* OutBuf, size_t* pOutLen);
void API LzssCompress2(const void* InBuf, size_t InLen, void* OutBuf, size_t* pOutLen, int CompressLevel);
void API LzssCompressXor(const void* InBuf, size_t InLen, void* OutBuf, size_t* pOutLen);
void API LzssCompressXor2(const void* InBuf, size_t InLen, void* OutBuf, size_t OutLen, int CompressLevel);
size_t API LzssCompressBound(size_t DataLen);
size_t API LzssDecompress(const void* InBuf, size_t InLen, void* OutBuf, size_t OutLen);
size_t API LzssDecompressXor(const void* InBuf, size_t InLen, void* OutBuf, size_t OutLen);

///////////////////////////////////////////
//////// 2. R2Beat 游戏内容相关
///////////////////////////////////////////

////game
typedef struct{
	char Enabled;	// 0 - 1
	char Slot;	// 0 - 5
	char Team;	// 0 - 2
	char Rank;	// 0 - 5
	int Frame;
	float FramePos;
	float FrameRate;	// 0, 0.9, 1.0, 1.1
} PlayerList;
int API gmGetRank(int UserCount, int MySlot, int TotalFrame, PlayerList* UsersIn, PlayerList* UsersOut, bool AllowSameRank);
int API gmGetRandomItemID(int IsTeamMode, int UserCount, int MySlot, int TotalFrame, PlayerList* Users, int MusicLevel);
int API gmGetAttackSlot(int IsTeamMode, int UserCount, int MySlot, int TotalFrame, PlayerList* Users, int ItemID);

///////////////////////////////////////////
//////// 3. 解析XML
///////////////////////////////////////////

////xmlparser
int API XMLPickAttribW(LPWSTR, LPCWSTR, LPCWSTR, size_t, LPCWSTR);
int API XMLPickAttribA(LPSTR, LPCSTR, LPCSTR, size_t, LPCSTR);
int API XMLPickAttrib2W(LPWSTR, size_t, LPCWSTR, LPCWSTR, size_t, LPCWSTR);
int API XMLPickAttrib2A(LPSTR, size_t, LPCSTR, LPCSTR, size_t, LPCSTR);
int API XMLPickAttribPosW(size_t*, size_t*, LPCWSTR, LPCWSTR, size_t, LPCWSTR);
int API XMLPickAttribPosA(size_t*, size_t*, LPCSTR, LPCSTR, size_t, LPCSTR);
int API XMLPickFullTagW(LPWSTR, LPCWSTR, LPCWSTR, size_t*, size_t);
int API XMLPickFullTagA(LPSTR, LPCSTR, LPCSTR, size_t*, size_t);
int API XMLPickFullTag2W(LPWSTR, size_t, LPCWSTR, LPCWSTR, size_t*, size_t);
int API XMLPickFullTag2A(LPSTR, size_t, LPCSTR, LPCSTR, size_t*, size_t);
int API XMLPickFullTagPosW(size_t*, size_t*, LPCWSTR, LPCWSTR, size_t*, size_t);
int API XMLPickFullTagPosA(size_t*, size_t*, LPCSTR, LPCSTR, size_t*, size_t);
int API XMLPickTagW(LPWSTR, LPCWSTR, LPCWSTR, size_t);
int API XMLPickTagA(LPSTR, LPCSTR, LPCSTR, size_t);
int API XMLPickTag2W(LPWSTR, LPCWSTR, LPCWSTR, size_t*, size_t);
int API XMLPickTag2A(LPSTR, LPCSTR, LPCSTR, size_t*, size_t);
int API XMLPickTag3W(LPWSTR, size_t, LPCWSTR, LPCWSTR, size_t*, size_t);
int API XMLPickTag3A(LPSTR, size_t, LPCSTR, LPCSTR, size_t*, size_t);
int API XMLPickTagPosW(size_t*, size_t*, LPCWSTR, LPCWSTR, size_t*, size_t);
int API XMLPickTagPosA(size_t*, size_t*, LPCSTR, LPCSTR, size_t*, size_t);

///////////////////////////////////////////
//////// 4. Hash函数
///////////////////////////////////////////

////libcrypto
int API MD4_Init(MD4_CTX *c);
int API MD4_Update(MD4_CTX *c, const void *data, size_t len);
int API MD4_Final(MD4_CTX *c, unsigned char *md);
unsigned char * API MD4(const void *d, size_t n, unsigned char *md);
int API MD5_Init(MD5_CTX *c);
int API MD5_Update(MD5_CTX *c, const void *data, size_t len);
int API MD5_Final(MD5_CTX *c, unsigned char *md);
unsigned char * API MD5(const void *d, size_t n, unsigned char *md);
int API MDC2_Init(MDC2_CTX *c);
int API MDC2_Update(MDC2_CTX *c, const void *data, size_t len);
int API MDC2_Final(MDC2_CTX *c, unsigned char *md);
unsigned char * API MDC2(const void *d, size_t n, unsigned char *md);
int API RIPEMD160_Init(RIPEMD160_CTX *c);
int API RIPEMD160_Update(RIPEMD160_CTX *c, const void *data, size_t len);
int API RIPEMD160_Final(RIPEMD160_CTX *c, unsigned char *md);
unsigned char * API RIPEMD160(const void *d, size_t n, unsigned char *md);
int API SHA1_Init(SHA_CTX *c);
int API SHA1_Update(SHA_CTX *c, const void *data, size_t len);
int API SHA1_Final(SHA_CTX *c, unsigned char *md);
unsigned char * API SHA1(const void *d, size_t n, unsigned char *md);
int API SHA224_Init(SHA256_CTX *c);
int API SHA224_Update(SHA256_CTX *c, const void *data, size_t len);
int API SHA224_Final(SHA256_CTX *c, unsigned char *md);
unsigned char * API SHA224(const void *d, size_t n, unsigned char *md);
int API SHA256_Init(SHA256_CTX *c);
int API SHA256_Update(SHA256_CTX *c, const void *data, size_t len);
int API SHA256_Final(SHA256_CTX *c, unsigned char *md);
unsigned char * API SHA256(const void *d, size_t n, unsigned char *md);
int API SHA384_Init(SHA512_CTX *c);
int API SHA384_Update(SHA512_CTX *c, const void *data, size_t len);
int API SHA384_Final(SHA512_CTX *c, unsigned char *md);
unsigned char * API SHA384(const void *d, size_t n, unsigned char *md);
int API SHA512_Init(SHA512_CTX *c);
int API SHA512_Update(SHA512_CTX *c, const void *data, size_t len);
int API SHA512_Final(SHA512_CTX *c, unsigned char *md);
unsigned char * API SHA512(const void *d, size_t n, unsigned char *md);
int API SHA512_224_Init(SHA512_CTX *c);
int API SHA512_224_Update(SHA512_CTX *c, const void *data, size_t len);
int API SHA512_224_Final(SHA512_CTX *c, unsigned char *md);
unsigned char * API SHA512_224(const void *d, size_t n, unsigned char *md);
int API SHA512_256_Init(SHA512_CTX *c);
int API SHA512_256_Update(SHA512_CTX *c, const void *data, size_t len);
int API SHA512_256_Final(SHA512_CTX *c, unsigned char *md);
unsigned char * API SHA512_256(const void *d, size_t n, unsigned char *md);
int API SM3_Init(SM3_CTX *c);
int API SM3_Update(SM3_CTX *c, const void *data, size_t len);
int API SM3_Final(SM3_CTX *c, unsigned char *md);
unsigned char * API SM3(const void *d, size_t n, unsigned char *md);
int API WHIRLPOOL_Init(WHIRLPOOL_CTX *c);
int API WHIRLPOOL_Update(WHIRLPOOL_CTX *c, const void *data, size_t len);
int API WHIRLPOOL_Final(WHIRLPOOL_CTX *c, unsigned char *md);
unsigned char * API WHIRLPOOL(const void *d, size_t n, unsigned char *md);

////nettle
int API SHA3_224_Init(SHA3_CTX *c);
int API SHA3_224_Update(SHA3_CTX *c, const void *data, size_t len);
int API SHA3_224_Final(SHA3_CTX *c, unsigned char *md);
unsigned char * API SHA3_224(const void *d, size_t n, unsigned char *md);
int API SHA3_256_Init(SHA3_CTX *c);
int API SHA3_256_Update(SHA3_CTX *c, const void *data, size_t len);
int API SHA3_256_Final(SHA3_CTX *c, unsigned char *md);
unsigned char * API SHA3_256(const void *d, size_t n, unsigned char *md);
int API SHA3_384_Init(SHA3_CTX *c);
int API SHA3_384_Update(SHA3_CTX *c, const void *data, size_t len);
int API SHA3_384_Final(SHA3_CTX *c, unsigned char *md);
unsigned char * API SHA3_384(const void *d, size_t n, unsigned char *md);
int API SHA3_512_Init(SHA3_CTX *c);
int API SHA3_512_Update(SHA3_CTX *c, const void *data, size_t len);
int API SHA3_512_Final(SHA3_CTX *c, unsigned char *md);
unsigned char * API SHA3_512(const void *d, size_t n, unsigned char *md);
int API SHAKE128_Init(SHA3_CTX *c);
int API SHAKE128_Update(SHA3_CTX *c, const void *data, size_t len);
int API SHAKE128_Final(SHA3_CTX *c, unsigned char *md, size_t mdlen);
unsigned char * API SHAKE128(const void *d, size_t n, unsigned char *md, size_t mdlen);
int API SHAKE256_Init(SHA3_CTX *c);
int API SHAKE256_Update(SHA3_CTX *c, const void *data, size_t len);
int API SHAKE256_Final(SHA3_CTX *c, unsigned char *md, size_t mdlen);
unsigned char * API SHAKE256(const void *d, size_t n, unsigned char *md, size_t mdlen);

////md6
int API MD6_Init(MD6_CTX *c);
int API MD6_Init_Len(MD6_CTX *c, int mdlen);
int API MD6_Update(MD6_CTX *c, const void *data, size_t len);
int API MD6_Final(MD6_CTX *c, unsigned char *md);
unsigned char * API MD6(const void *d, size_t n, unsigned char *md);
unsigned char * API MD6_Len(const void *d, size_t n, unsigned char *md, int mdlen);

////unrar
int API BLAKE2SP_Init(BLAKE2SP_CTX *c);
int API BLAKE2SP_Update(BLAKE2SP_CTX *c, const void *data, size_t len);
int API BLAKE2SP_Final(BLAKE2SP_CTX *c, unsigned char *md);
unsigned char * API BLAKE2SP(const void *d, size_t n, unsigned char *md);

///////////////////////////////////////////
//////// 5. 压缩解压
///////////////////////////////////////////

////zlib
uint32_t API CRC32(uint32_t crc, const void *buf, size_t len);
uint32_t API CRC32_Combine(uint32_t crc1, uint32_t crc2, int32_t len2);
uint32_t API CRC32_Combine64(uint32_t crc1, uint32_t crc2, int64_t len2);
int API ZlibCompress(void *dest, unsigned int *destLen, const void *source, unsigned int sourceLen);
int API ZlibCompress2(void *dest, unsigned int *destLen, const void *source, unsigned int sourceLen, int level);
unsigned int API ZlibCompressBound(unsigned int sourceLen);
int API ZlibUncompress(void *dest, unsigned int *destLen, const void *source, unsigned int sourceLen);

////crc64
uint64_t API CRC64(uint64_t crc, const void *buf, size_t len);
uint64_t API CRC64_Combine(uint64_t crc1, uint64_t crc2, int64_t len2);

///////////////////////////////////////////
//////// 6. 字符编码转换
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

///////////////////////////////////////////
//////// 7. 大整数操作辅助函数
///////////////////////////////////////////
////本模块很多函数C/C++等语言原生支持，因此不列

////i4helper
int API I4Rol(int, int);
int API I4Ror(int, int);

////i8helper
int64_t API I8Rol(int64_t, int);
int64_t API I8Ror(int64_t, int);
#ifndef _WIN64
int64_t API I8RolR(int64_t *, int);
int64_t API I8RorR(int64_t *, int);
#endif

///////////////////////////////////////////
//////// 8. 排序函数
///////////////////////////////////////////
////本模块很多函数C/C++等语言原生支持，因此不列

////sort
void API MergeSort(void* base, size_t num, size_t size, int (API *stdcallcompare)(const void *a, const void *b));

///////////////////////////////////////////
//////// 9. 随机函数
///////////////////////////////////////////
////本模块很多函数C++11原生支持

////mt19937
void API MT_SRand(uint64_t seed);
void API MT_SRand_Array(void* seed_array, uint64_t array_length);
uint32_t API MT_Rand();
uint64_t API MT_Rand64();
double API MT_DRand();

///////////////////////////////////////////
//////// 10. 老系统兼容函数
///////////////////////////////////////////

#ifndef _WIN64
const char* WINAPI inet_ntop(int af, void const *src, char *dst, size_t size);
int WINAPI inet_pton(int af, const char *src, void *dst);
#endif

///////////////////////////////////////////
//////// 11. 不再使用的旧函数
///////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif

