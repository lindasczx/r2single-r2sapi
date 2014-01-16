#ifndef UTF_H_
#define UTF_H_

#include <inttypes.h>
#include <windows.h>

#ifdef __cplusplus
extern "C"{
#endif

#define cpINVALID   (-1)
// UCS-4 full compliance, stateless
#define cpUTF8      1
#define cpUTF32LE   4
#define cpUTF32BE   5
#define cpUTF32     cpUTF32LE
#define cpUTFEBCDIC 0x40
#define cpUTF1      0x41
// UTF-16 related, stateless
#define cpUTF16LE   2
#define cpUTF16BE   3
#define cpUTF16     cpUTF16LE
#define cpCESU8     0x80
// UTF-16 related, stateful
#define cpUTF7      0xa0
// national standard, UTF-16 related, stateless
#define cpGB18030   0x100

#define CP_GB18030  54936

#define MAKETRCODE(a,b) ( (uint32_t)( (((uint32_t)(uint16_t)a)<<16) | ((uint32_t)(uint16_t)b) ) )

#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))

typedef int * LPLSTR;
typedef const int * LPCLSTR;

// API有具体实现的函数
unsigned int WINAPI cpMakeTrCode(unsigned short cpfrom, unsigned short cpto);
int WINAPI cpConvertEncoding(unsigned int nTrCode, LPVOID lpSrcStr, int cchSrc, LPVOID lpDestStr, int cchDest);
int WINAPI cpTrCodeSupported(unsigned int nTrCode);
int WINAPI GB18030ToUTF16(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int WINAPI UTF16ToGB18030(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int WINAPI UTF7ToUTF16(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int WINAPI UTF16ToUTF7(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
// API包装函数
int WINAPI UTF16BEToUTF16(LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int WINAPI UTF32BEToUTF32(LPCLSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
int WINAPI UTF8ToUTF16LE(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int WINAPI UTF8ToUTF32LE(LPCSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
int WINAPI UTF16LEToUTF8(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int WINAPI UTF16LEToUTF32LE(LPCWSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
int WINAPI UTF16LEToUTF32BE(LPCWSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest);
int WINAPI UTF32LEToUTF8(LPCLSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int WINAPI UTF32LEToUTF16(LPCLSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
int WINAPI UTF32BEToUTF16LE(LPCLSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest);
// 内部函数
size_t lcslen(LPCLSTR lpSrcStr);
int CopyString(int nSize, LPCVOID lpSrcStr, int cchSrc, LPVOID lpDestStr, int cchDest);
int SwapByte(int nSize, LPCVOID lpSrcStr, int cchSrc, LPVOID lpDestStr, int cchDest);
/*inline*/ int UTF8ToScalar(LPCSTR lpSrcStr, int cchSrc, int* scalar);
/*inline*/ int ScalarToUTF8(LPSTR lpDestStr, int cchDest, int scalar);
/*inline*/ int UTF16ToScalar(LPCWSTR lpSrcStr, int cchSrc, int* scalar, int BigEndian16);
/*inline*/ int ScalarToUTF16(LPWSTR lpDestStr, int cchDest, int scalar);
/*inline*/ int UTFEBCDICToScalar(LPCSTR lpSrcStr, int cchSrc, int* scalar);
/*inline*/ int ScalarToUTFEBCDIC(LPSTR lpDestStr, int cchDest, int scalar);
/*inline*/ int UTF1ToScalar(LPCSTR lpSrcStr, int cchSrc, int* scalar);
/*inline*/ int ScalarToUTF1(LPSTR lpDestStr, int cchDest, int scalar);
int UTF8ToUTF16(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest, int BigEndian16);
int UTF8ToUTF32(LPCSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest, int BigEndian32);
int UTF8ToUTFEBCDIC(LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int UTF8ToUTF1(LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int UTF16ToUTF8(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest, int BigEndian16);
int UTF16ToUTF32(LPCWSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest, int BigEndian16, int BigEndian32);
int UTF16ToUTFEBCDIC(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest, int BigEndian16);
int UTF16ToUTF1(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest, int BigEndian16);
int UTF16ToCESU8(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest, int BigEndian16);
int UTF32ToUTF8(LPCLSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest, int BigEndian32);
int UTF32ToUTF16(LPCLSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest, int BigEndian32, int BigEndian16);
int UTF32ToUTFEBCDIC(LPCLSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest, int BigEndian32);
int UTF32ToUTF1(LPCLSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest, int BigEndian32);
int UTFEBCDICToUTF8(LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int UTFEBCDICToUTF16(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest, int BigEndian16);
int UTFEBCDICToUTF32(LPCSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest, int BigEndian32);
int UTFEBCDICToUTF1(LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int UTF1ToUTF8(LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int UTF1ToUTF16(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest, int BigEndian16);
int UTF1ToUTF32(LPCSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest, int BigEndian32);
int UTF1ToUTFEBCDIC(LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest);
int CESU8ToUTF16(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest, int BigEndian16);

#ifdef __cplusplus
}
#endif

#endif

