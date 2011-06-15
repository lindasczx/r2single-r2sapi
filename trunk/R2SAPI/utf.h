#ifndef UTF_H_
#define UTF_H_

#include <windows.h>

#ifdef __cplusplus
extern "C"{
#endif

#define cpINVALID (-1)
#define cpUTF8    1
#define cpUTF16LE 2
#define cpUTF16BE 3
#define cpUTF16   cpUTF16LE
#define cpUTF32LE 4
#define cpUTF32BE 5
#define cpUTF32   cpUTF32LE
#define cpGB18030 0x100

#define CP_GB18030 54936

#define MAKETRCODE(a,b) (((a)<<16)|(b))

typedef int * LPLSTR;
typedef const int * LPCLSTR;

int WINAPI cpConvertEncoding(unsigned int nTrCode, LPVOID lpSrcStr, int cchSrc, LPVOID lpDestStr, int cchDest);
int WINAPI cpTrCodeSupported(unsigned int nTrCode);
int WINAPI GB18030ToUTF16(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int WINAPI UTF8ToUTF16(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int WINAPI UTF8ToUTF32(LPCSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
int WINAPI UTF16ToGB18030(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int WINAPI UTF16ToUTF8(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int WINAPI UTF16ToUTF32(LPCWSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
int WINAPI UTF16ToUTF32BE(LPCWSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
int WINAPI UTF16BEToUTF16(LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int WINAPI UTF32ToUTF8(LPCLSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int WINAPI UTF32ToUTF16(LPCLSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int WINAPI UTF32BEToUTF16(LPCLSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int WINAPI UTF32BEToUTF32(LPCLSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);

#ifdef __cplusplus
}
#endif

#endif

