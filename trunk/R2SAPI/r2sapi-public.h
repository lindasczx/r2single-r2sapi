/*++
Module Name:
	r2sapi.h

Version:
	1.34.0.129
--*/

#ifndef R2SAPI_H_
#define R2SAPI_H_

#ifdef __cplusplus
extern "C"{
#endif

#define EXPORT extern "C"
#define API __stdcall

#include <sys/types.h>
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
EXPORT unsigned char * API MD5(const unsigned char *d, size_t n, unsigned char *md);
EXPORT unsigned char * API SHA1(const unsigned char *d, size_t n, unsigned char *md);
//EXPORT unsigned char * API SHA224(const unsigned char *d, size_t n, unsigned char *md);
EXPORT unsigned char * API SHA256(const unsigned char *d, size_t n, unsigned char *md);
//EXPORT unsigned char * API SHA384(const unsigned char *d, size_t n, unsigned char *md);
EXPORT unsigned char * API SHA512(const unsigned char *d, size_t n, unsigned char *md);

////westpak
EXPORT long API GetFileFromPakA(void* pBuf, long ulBufLen, LPCSTR pszFn, LPCSTR pszFnWant);
EXPORT long API GetFileFromPakW(void* pBuf, long ulBufLen, LPCWSTR pszFn, LPCWSTR pszFnWant);

////xmlparser
EXPORT LPCWSTR API __p__STR_ERROR_W();
EXPORT bool    API InQuoteW(LPCWSTR, long, long);
EXPORT LPCWSTR API InStrChrOutQuoteW(LPCWSTR, wchar_t);
EXPORT void    API KOCDataW(LPWSTR, LPCWSTR);
EXPORT void    API KOQuoteW(LPWSTR, LPCWSTR);
EXPORT void    API MyTrimW(LPWSTR, LPCWSTR);
EXPORT int     API XMLIsChar(wchar_t);
EXPORT int     API XMLPickAttribW(LPWSTR, LPCWSTR, LPCWSTR, long, LPCWSTR);
EXPORT int     API XMLPickFullTagW(LPWSTR, LPCWSTR, LPCWSTR, long&, long);
EXPORT int     API XMLPickTagW(LPWSTR, LPCWSTR, LPCWSTR, long);
EXPORT int     API XMLPickTag2W(LPWSTR, LPCWSTR, LPCWSTR, long&, long);

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

