/*++
Copyright (c) 1991-1996,  Microsoft Corporation,
              2010-2011,  ZX Inc. All rights reserved.
Module Name:
    utf.c

Abstract:
    This file contains functions that convert UTF strings to Unicode
    strings and Unicode string to UTF strings.
    
    Unicode 6.0.0:
    UTF-32 range: 00000000 - 0010FFFF
    UTF-16 range: 0000 - DBFF DFFF, no supersurrogates defined right now.
    UTF-8  range: 00 - F4 8F BF BF

Revision History:
    1996-02-06    JulieB      Created.
    2010-12-08    Linda Zhang Edited.
--*/

#include "utf.h"

//  Constant Declarations.

#define ASCII                 0x7f
#define UTF8_2_MAX            0x7ff  // max UTF8 2-byte sequence (32*64=2048)
#define UTF8_3_MAX            0xffff  // max UTF8 3-byte sequence (16*64*64=65536)
#define UTF8_4_MAX            0x1fffff // max UTF8 4-byte sequence (8*64*64*64=2097152)
#define UTF8_5_MAX            0x3ffffff // max UTF8 5-byte sequence (4*64*64*64*64=67108864)
#define UTF8_6_MAX            0x7fffffff // max UTF8 6-byte sequence (maxint)
#define UTF8_1ST_OF_2         0xc0    // 110x xxxx
#define UTF8_1ST_OF_3         0xe0    // 1110 xxxx
#define UTF8_1ST_OF_4         0xf0    // 1111 0xxx
#define UTF8_1ST_OF_5         0xf8    // 1111 10xx
#define UTF8_1ST_OF_6         0xfc    // 1111 110x
#define UTF8_TRAIL            0x80    // 10xx xxxx

#define TOP_6_BIT(u)          ((u) >> 30)
#define LAST5_6_BIT(u)        (((u) & 0x3f000000) >> 24)
#define LAST4_6_BIT(u)        (((u) & 0xfc0000) >> 18)
#define LAST3_6_BIT(u)        (((u) & 0x3f000) >> 12)
#define LAST2_6_BIT(u)        (((u) & 0xfc0) >> 6)
#define LAST1_6_BIT(u)        ((u) & 0x3f)

#define BIT7(a)               ((a) & 0x80)
#define BIT6(a)               ((a) & 0x40)

#define UTF16_2_MAX           0xffff  // max UTF16 2-byte sequence
#define HIGH_SURROGATE_START  0xd800
#define HIGH_SURROGATE_END    0xdbff
#define LOW_SURROGATE_START   0xdc00
#define LOW_SURROGATE_END     0xdfff

#define MAKEUTF32(u1,u2)      (((((u1)&0x03ff)<<10)|((u2)&0x03ff))+0x10000)
#define MAKEHIGHSURROGATE(u)  (0xd800|((((u)-0x10000)>>10)&0x3ff))
#define MAKELOWSURROGATE(u)   (0xdc00|((u)&0x3ff))

#define SWAPBYTEORDER2(u)     ((((u)>>8)&0xff)|(((u)<<8)&0xff00))
#define SWAPBYTEORDER4(u)     ((((u)>>24)&0xff)|(((u)>>8)&0xff00)|(((u)<<8)&0xff0000)|((u)<<24))


//-------------------------------------------------------------------------//
//                           INTERNAL ROUTINES                             //
//-------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////
//
//  cpConvertEncoding
//
//  字符串编码转换是否支持
//
//  2011-06-12  Linda Zhang Created.
////////////////////////////////////////////////////////////////////////////

int WINAPI cpTrCodeSupported(unsigned int nTrCode){
    switch(nTrCode){
        case MAKETRCODE(cpUTF8,cpUTF16LE):
//        case MAKETRCODE(cpUTF8,cpUTF16BE):
        case MAKETRCODE(cpUTF8,cpUTF32LE):
//        case MAKETRCODE(cpUTF8,cpUTF32BE):

        case MAKETRCODE(cpUTF16LE,cpUTF8):
        case MAKETRCODE(cpUTF16LE,cpUTF16BE):
        case MAKETRCODE(cpUTF16LE,cpUTF32LE):
        case MAKETRCODE(cpUTF16LE,cpUTF32BE):

//        case MAKETRCODE(cpUTF16BE,cpUTF8):
        case MAKETRCODE(cpUTF16BE,cpUTF16LE):
//        case MAKETRCODE(cpUTF16BE,cpUTF32LE):
//        case MAKETRCODE(cpUTF16BE,cpUTF32BE):

        case MAKETRCODE(cpUTF32LE,cpUTF8):
        case MAKETRCODE(cpUTF32LE,cpUTF16LE):
//        case MAKETRCODE(cpUTF32LE,cpUTF16BE):
        case MAKETRCODE(cpUTF32LE,cpUTF32BE):

//        case MAKETRCODE(cpUTF32BE,cpUTF8):
        case MAKETRCODE(cpUTF32BE,cpUTF16LE):
//        case MAKETRCODE(cpUTF32BE,cpUTF16BE):
        case MAKETRCODE(cpUTF32BE,cpUTF32LE):
            return -1;

        default:
            return 0;
    }
}

////////////////////////////////////////////////////////////////////////////
//
//  cpConvertEncoding
//
//  转换字符串编码
//
//  2011-06-12  Linda Zhang Created.
////////////////////////////////////////////////////////////////////////////
int WINAPI cpConvertEncoding(
    unsigned int nTrCode,
    LPVOID lpSrcStr,
    int cchSrc,
    LPVOID lpDestStr,
    int cchDest)
{
    switch(nTrCode){
        case MAKETRCODE(cpUTF8,cpUTF16LE):
            return UTF8ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest);
//        case MAKETRCODE(cpUTF8,cpUTF16BE):
        case MAKETRCODE(cpUTF8,cpUTF32LE):
            return UTF8ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest);
//        case MAKETRCODE(cpUTF8,cpUTF32BE):

        case MAKETRCODE(cpUTF16LE,cpUTF8):
            return UTF16ToUTF8(lpSrcStr, cchSrc, lpDestStr, cchDest);
        case MAKETRCODE(cpUTF16LE,cpUTF16BE):
        case MAKETRCODE(cpUTF16BE,cpUTF16LE):
            return UTF16BEToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest);
        case MAKETRCODE(cpUTF16LE,cpUTF32LE):
            return UTF16ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest);
        case MAKETRCODE(cpUTF16LE,cpUTF32BE):
            return UTF16ToUTF32BE(lpSrcStr, cchSrc, lpDestStr, cchDest);

//        case MAKETRCODE(cpUTF16BE,cpUTF8):
            return UTF16BEToUTF8(lpSrcStr, cchSrc, lpDestStr, cchDest);
//        case MAKETRCODE(cpUTF16BE,cpUTF32LE):
//        case MAKETRCODE(cpUTF16BE,cpUTF32BE):

        case MAKETRCODE(cpUTF32LE,cpUTF8):
            return UTF32ToUTF8(lpSrcStr, cchSrc, lpDestStr, cchDest);
        case MAKETRCODE(cpUTF32LE,cpUTF16LE):
            return UTF32ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest);
//        case MAKETRCODE(cpUTF32LE,cpUTF16BE):
        case MAKETRCODE(cpUTF32LE,cpUTF32BE):
        case MAKETRCODE(cpUTF32BE,cpUTF32LE):
            return UTF32BEToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest);

//        case MAKETRCODE(cpUTF32BE,cpUTF8):
        case MAKETRCODE(cpUTF32BE,cpUTF16LE):
            return UTF32BEToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest);
//        case MAKETRCODE(cpUTF32BE,cpUTF16BE):

        default:
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
    }
}

////////////////////////////////////////////////////////////////////////////
//
//  UTF8ToUTF16
//
//  将UTF8字符串转为UTF16格式
//
//  1996-02-06  JulieB      Created.
//  2010-12-08  Linda Zhang Edited.
////////////////////////////////////////////////////////////////////////////

int WINAPI UTF8ToUTF16(
    LPCSTR lpSrcStr,
    int cchSrc,
    LPWSTR lpDestStr,
    int cchDest)
{
    int nTB = 0;                   // # trail bytes to follow
    int cchWC = 0;                 // # of Unicode code points generated
    LPCSTR pUTF8 = lpSrcStr;
    char UTF8;
    int utf32;

    if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
         ((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPSTR)lpDestStr))) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (0);
    }

    while ((cchSrc--) && ((cchDest == 0) || (cchWC < cchDest)))
    {
        //  See if there are any trail bytes.
        if (BIT7(*pUTF8) == 0)
        {
            //  Found ASCII.
            if (cchDest)
            {
                lpDestStr[cchWC] = (WCHAR)*pUTF8;
            }
            cchWC++;
        }
        else if (BIT6(*pUTF8) == 0)
        {
            //  Found a trail byte.
            //  Note : Ignore the trail byte if there was no lead byte.
            if (nTB != 0)
            {
                //  Decrement the trail byte counter.
                nTB--;

                //  Make room for the trail byte and add the trail byte value.
                utf32 <<= 6;
                utf32 |= LAST1_6_BIT(*pUTF8);

                if (nTB == 0)
                {
                    //  End of sequence.  Advance the output counter.
                    if(utf32<=UTF16_2_MAX)
                    {
                        if (cchDest)
                        {
                            lpDestStr[cchWC]=utf32;
                        }
                        cchWC++;
                    }
                    else
                    {
                        //  Out of BMP, output surrogates.
                        if ((cchWC + 1) < cchDest)
                        {
                            if (cchDest)
                            {
                                lpDestStr[cchWC]=MAKEHIGHSURROGATE(utf32);
                                lpDestStr[cchWC+1]=MAKELOWSURROGATE(utf32);
                            }
                            cchWC+=2;
                        }
                        else
                        {
                            cchSrc++;
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            //  Found a lead byte.
            if (nTB > 0)
            {
                //  Error - previous sequence not finished.
                nTB = 0;
                cchWC++;
            }
            else
            {
                //  Calculate the number of bytes to follow.
                //  Look for the first 0 from left to right.
                UTF8 = *pUTF8;
                while (BIT7(UTF8) != 0)
                {
                    UTF8 <<= 1;
                    nTB++;
                }

                //  Store the value from the first byte and decrement
                //  the number of bytes to follow.
                utf32 = UTF8 >> nTB;
                nTB--;
            }
        }

        pUTF8++;
    }

    //  Make sure the destination buffer was large enough.
    if (cchDest && (cchSrc >= 0))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return (0);
    }

    //  Return the number of Unicode characters written.
    return (cchWC);
}


////////////////////////////////////////////////////////////////////////////
//
//  UTF16ToUTF8
//
//  将UTF16字符串转为UTF8格式
//
//  1996-02-06  JulieB      Created.
//  2010-12-08  Linda Zhang Edited.
////////////////////////////////////////////////////////////////////////////

int WINAPI UTF16ToUTF8(
    LPCWSTR lpSrcStr,
    int cchSrc,
    LPSTR lpDestStr,
    int cchDest)
{
    LPCWSTR lpWC = lpSrcStr;
    int utf32;
    int cchU8 = 0;                // # of UTF8 chars generated

    if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
         ((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPWSTR)lpDestStr))) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (0);
    }

    while ((cchSrc--) && ((cchDest == 0) || (cchU8 < cchDest)))
    {
        if (*lpWC <= ASCII)
        {
            //  Found ASCII.
            if (cchDest)
            {
                lpDestStr[cchU8] = (char)*lpWC;
            }
            cchU8++;
        }
        else if (*lpWC <= UTF8_2_MAX)
        {
            //  Found 2 byte sequence if < 0x07ff (11 bits).
            if (cchDest)
            {
                if ((cchU8 + 1) < cchDest)
                {
                    //  Use upper 5 bits in first byte.
                    //  Use lower 6 bits in second byte.
                    lpDestStr[cchU8++] = UTF8_1ST_OF_2 | (*lpWC >> 6);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST1_6_BIT(*lpWC);
                }
                else
                {
                    //  Error - buffer too small.
                    cchSrc++;
                    break;
                }
            }
            else
            {
                cchU8 += 2;
            }
        }
        else if ((*lpWC >= HIGH_SURROGATE_START) && (*lpWC <= HIGH_SURROGATE_END))
        {
            //  Found 4 byte sequence.
            if ((cchSrc>=2) && (*(lpWC+1) >= LOW_SURROGATE_START) && (*(lpWC+1) <= LOW_SURROGATE_END))
            {
                if (cchDest)
                {
                    if (cchU8+3<cchDest)
                    {
                        cchSrc--;
                        utf32=MAKEUTF32(*lpWC,*(lpWC+1));
                        lpWC++;
                        //  Use upper  3 bits in first byte.
                        //  Use higher 6 bits in second byte.
                        //  Use middle 6 bits in third byte.
                        //  Use lower  6 bits in fourth byte.
                        lpDestStr[cchU8++] = UTF8_1ST_OF_4 | (utf32 >> 18);
                        lpDestStr[cchU8++] = UTF8_TRAIL    | LAST3_6_BIT(utf32);
                        lpDestStr[cchU8++] = UTF8_TRAIL    | LAST2_6_BIT(utf32);
                        lpDestStr[cchU8++] = UTF8_TRAIL    | LAST1_6_BIT(utf32);
                    }
                    else
                    {
                        //  Error - buffer too small.
                        cchSrc++;
                        break;
                    }
                }
                else
                {
                    cchSrc--;
                    lpWC++;
                    cchU8 += 4;
                }
            }
            else
            {
                goto badsurrogatepair;
            }
        }
        else
        {
            //  Found 3 byte sequence.
badsurrogatepair:
            if (cchDest)
            {
                if ((cchU8 + 2) < cchDest)
                {
                    //  Use upper  4 bits in first byte.
                    //  Use middle 6 bits in second byte.
                    //  Use lower  6 bits in third byte.
                    lpDestStr[cchU8++] = UTF8_1ST_OF_3 | (*lpWC >> 12);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST2_6_BIT(*lpWC);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST1_6_BIT(*lpWC);
                }
                else
                {
                    //  Error - buffer too small.
                    cchSrc++;
                    break;
                }
            }
            else
            {
                cchU8 += 3;
            }
        }

        lpWC++;
    }

    //  Make sure the destination buffer was large enough.
    if (cchDest && (cchSrc >= 0))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return (0);
    }

    //  Return the number of UTF-8 characters written.
    return (cchU8);
}


////////////////////////////////////////////////////////////////////////////
//
//  UTF16BEToUTF16
//
//  倒转UTF16字符串的字节序
//
//  2010-12-08  Linda Zhang Created.
////////////////////////////////////////////////////////////////////////////

int WINAPI UTF16BEToUTF16(
    LPCWSTR lpSrcStr,
    int cchSrc,
    LPWSTR lpDestStr,
    int cchDest)
{
    int cchWC = 0;                 // # of Unicode code points generated

    if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
         ((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == lpDestStr))) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (0);
    }

    while ((cchSrc--) && ((cchDest == 0) || (cchWC < cchDest)))
    {
        if (cchDest)
        {
            lpDestStr[cchWC] = SWAPBYTEORDER2(lpSrcStr[cchWC]);
        }
        cchWC++;
    }

    //  Make sure the destination buffer was large enough.
    if (cchDest && (cchSrc >= 0))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return (0);
    }

    //  Return the number of Unicode characters written.
    return (cchWC);
}


////////////////////////////////////////////////////////////////////////////
//
//  UTF32ToUTF16
//
//  将UTF32字符串转为UTF16格式
//
//  2010-12-08  Linda Zhang Created.
////////////////////////////////////////////////////////////////////////////

int WINAPI UTF32ToUTF16(
    LPCLSTR lpSrcStr,
    int cchSrc,
    LPWSTR lpDestStr,
    int cchDest)
{
    int cchWC = 0;                 // # of Unicode code points generated
    LPCLSTR pUTF32 = lpSrcStr;

    if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
         ((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPLSTR)lpDestStr))) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (0);
    }

    while ((cchSrc--) && ((cchDest == 0) || (cchWC < cchDest)))
    {
        if(*pUTF32<=UTF16_2_MAX)
        {
            if (cchDest)
            {
                lpDestStr[cchWC] = *pUTF32;
            }
            cchWC++;
        }
        else
        {
            //  Out of BMP, output surrogates.
            if(cchWC+1<cchDest)
            {
                if (cchDest)
                {
                    lpDestStr[cchWC] = MAKEHIGHSURROGATE(*pUTF32);
                    lpDestStr[cchWC+1] = MAKELOWSURROGATE(*pUTF32);
                }
                cchWC+=2;
            }
            else
            {
                //  Error - buffer too small.
                cchSrc++;
                break;
            }
        }
        
        pUTF32++;
    }

    //  Make sure the destination buffer was large enough.
    if (cchDest && (cchSrc >= 0))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return (0);
    }

    //  Return the number of Unicode characters written.
    return (cchWC);
}


////////////////////////////////////////////////////////////////////////////
//
//  UTF16ToUTF32
//
//  将UTF16字符串转为UTF32格式
//
//  2010-12-08  Linda Zhang Created.
////////////////////////////////////////////////////////////////////////////

int WINAPI UTF16ToUTF32(
    LPCWSTR lpSrcStr,
    int cchSrc,
    LPLSTR lpDestStr,
    int cchDest)
{
    LPCWSTR lpWC = lpSrcStr;
    int utf32;
    int cchU32 = 0;                // # of UTF32 chars generated

    if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
         ((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPWSTR)lpDestStr))) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (0);
    }

    while ((cchSrc--) && ((cchDest == 0) || (cchU32 < cchDest)))
    {
        if ((*lpWC >= HIGH_SURROGATE_START) && (*lpWC <= HIGH_SURROGATE_END))
        {
            if ((cchSrc>=2) && (*(lpWC+1) >= LOW_SURROGATE_START) && (*(lpWC+1) <= LOW_SURROGATE_END))
            {
                if (cchDest)
                {
                    utf32=MAKEUTF32(*lpWC,*(lpWC+1));
                    lpDestStr[cchU32] = utf32;
                }
                cchSrc--;
                lpWC++;
                cchU32++;
            }
            else
            {
                goto badsurrogatepair;
            }
        }
        else
        {
badsurrogatepair:
            if (cchDest)
            {
                lpDestStr[cchU32] = *lpWC;
            }
            cchU32++;
        }

        lpWC++;
    }

    //  Make sure the destination buffer was large enough.
    if (cchDest && (cchSrc >= 0))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return (0);
    }

    //  Return the number of UTF-32 characters written.
    return (cchU32);
}


////////////////////////////////////////////////////////////////////////////
//
//  UTF32BEToUTF16
//
//  将UTF32BE字符串转为UTF16格式
//
//  2010-12-08  Linda Zhang Created.
////////////////////////////////////////////////////////////////////////////

int WINAPI UTF32BEToUTF16(
    LPCLSTR lpSrcStr,
    int cchSrc,
    LPWSTR lpDestStr,
    int cchDest)
{
    int cchWC = 0;                 // # of Unicode code points generated
    LPCLSTR pUTF32 = lpSrcStr;
    int utf32;

    if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
         ((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPLSTR)lpDestStr))) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (0);
    }

    while ((cchSrc--) && ((cchDest == 0) || (cchWC < cchDest)))
    {
        utf32=SWAPBYTEORDER4(*pUTF32);
        if(utf32<=UTF16_2_MAX)
        {
            if (cchDest)
            {
                lpDestStr[cchWC] = utf32;
            }
            cchWC++;
        }
        else
        {
            if(cchWC+1<cchDest)
            {
                if (cchDest)
                {
                    lpDestStr[cchWC] = MAKEHIGHSURROGATE(utf32);
                    lpDestStr[cchWC+1] = MAKELOWSURROGATE(utf32);
                }
                cchWC+=2;
            }
            else
            {
                //  Error - buffer too small.
                cchSrc++;
                break;
            }
        }

        pUTF32++;
    }

    //  Make sure the destination buffer was large enough.
    if (cchDest && (cchSrc >= 0))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return (0);
    }

    //  Return the number of Unicode characters written.
    return (cchWC);
}


////////////////////////////////////////////////////////////////////////////
//
//  UTF16ToUTF32BE
//
//  将UTF16字符串转为UTF32BE格式
//
//  2010-12-08  Linda Zhang Created.
////////////////////////////////////////////////////////////////////////////

int WINAPI UTF16ToUTF32BE(
    LPCWSTR lpSrcStr,
    int cchSrc,
    LPLSTR lpDestStr,
    int cchDest)
{
    LPCWSTR lpWC = lpSrcStr;
    int utf32;
    int cchU32 = 0;                // # of UTF32 chars generated

    if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
         ((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPWSTR)lpDestStr))) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (0);
    }

    while ((cchSrc--) && ((cchDest == 0) || (cchU32 < cchDest)))
    {
        if ((*lpWC >= HIGH_SURROGATE_START) && (*lpWC <= HIGH_SURROGATE_END))
        {
            if ((cchSrc>=2) && (*(lpWC+1) >= LOW_SURROGATE_START) && (*(lpWC+1) <= LOW_SURROGATE_END))
            {
                if (cchDest)
                {
                    utf32=MAKEUTF32(*lpWC,*(lpWC+1));
                    lpDestStr[cchU32] = SWAPBYTEORDER4(utf32);
                }
                cchSrc--;
                lpWC++;
                cchU32++;
            }
            else
            {
                goto badsurrogatepair;
            }
        }
        else
        {
badsurrogatepair:
            if (cchDest)
            {
                lpDestStr[cchU32] = SWAPBYTEORDER4(*lpWC);
            }
            cchU32++;
        }

        lpWC++;
    }

    //  Make sure the destination buffer was large enough.
    if (cchDest && (cchSrc >= 0))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return (0);
    }

    //  Return the number of UTF-32 characters written.
    return (cchU32);
}


////////////////////////////////////////////////////////////////////////////
//
//  UTF32BEToUTF32
//
//  倒转UTF32字符串的字节序
//
//  2010-12-11  Linda Zhang Created.
////////////////////////////////////////////////////////////////////////////

int WINAPI UTF32BEToUTF32(
    LPCLSTR lpSrcStr,
    int cchSrc,
    LPLSTR lpDestStr,
    int cchDest)
{
    int cchLC = 0;                 // # of Unicode code points generated

    if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
         ((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == lpDestStr))) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (0);
    }

    while ((cchSrc--) && ((cchDest == 0) || (cchLC < cchDest)))
    {
        if (cchDest)
        {
            lpDestStr[cchLC] = SWAPBYTEORDER4(lpSrcStr[cchLC]);
        }
        cchLC++;
    }

    //  Make sure the destination buffer was large enough.
    if (cchDest && (cchSrc >= 0))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return (0);
    }

    //  Return the number of Unicode characters written.
    return (cchLC);
}


////////////////////////////////////////////////////////////////////////////
//
//  UTF8ToUTF32
//
//  将UTF8字符串转为UTF32格式
//
//  2010-12-11  Linda Zhang Created.
////////////////////////////////////////////////////////////////////////////

int WINAPI UTF8ToUTF32(
    LPCSTR lpSrcStr,
    int cchSrc,
    LPLSTR lpDestStr,
    int cchDest)
{
    int nTB = 0;                   // # trail bytes to follow
    int cchLC = 0;                 // # of Unicode code points generated
    LPCSTR pUTF8 = lpSrcStr;
    char UTF8;
    int utf32;

    if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
         ((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPSTR)lpDestStr))) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (0);
    }

    while ((cchSrc--) && ((cchDest == 0) || (cchLC < cchDest)))
    {
        //  See if there are any trail bytes.
        if (BIT7(*pUTF8) == 0)
        {
            //  Found ASCII.
            if (cchDest)
            {
                lpDestStr[cchLC] = (WCHAR)*pUTF8;
            }
            cchLC++;
        }
        else if (BIT6(*pUTF8) == 0)
        {
            //  Found a trail byte.
            //  Note : Ignore the trail byte if there was no lead byte.
            if (nTB != 0)
            {
                //  Decrement the trail byte counter.
                nTB--;

                //  Make room for the trail byte and add the trail byte value.
                utf32 <<= 6;
                utf32 |= LAST1_6_BIT(*pUTF8);

                if (nTB == 0)
                {
                    //  End of sequence.  Advance the output counter.
                    if (cchDest)
                    {
                        lpDestStr[cchLC]=utf32;
                    }
                    cchLC++;
                }
            }
        }
        else
        {
            //  Found a lead byte.
            if (nTB > 0)
            {
                //  Error - previous sequence not finished.
                nTB = 0;
                cchLC++;
            }
            else
            {
                //  Calculate the number of bytes to follow.
                //  Look for the first 0 from left to right.
                UTF8 = *pUTF8;
                while (BIT7(UTF8) != 0)
                {
                    UTF8 <<= 1;
                    nTB++;
                }

                //  Store the value from the first byte and decrement
                //  the number of bytes to follow.
                utf32 = UTF8 >> nTB;
                nTB--;
            }
        }

        pUTF8++;
    }

    //  Make sure the destination buffer was large enough.
    if (cchDest && (cchSrc >= 0))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return (0);
    }

    //  Return the number of Unicode characters written.
    return (cchLC);
}


////////////////////////////////////////////////////////////////////////////
//
//  UTF32ToUTF8
//
//  将UTF32字符串转为UTF8格式
//
//  2010-12-11  Linda Zhang Created.
////////////////////////////////////////////////////////////////////////////

int WINAPI UTF32ToUTF8(
    LPCLSTR lpSrcStr,
    int cchSrc,
    LPSTR lpDestStr,
    int cchDest)
{
    LPCLSTR lpLC = lpSrcStr;
    int utf32;
    int cchU8 = 0;                // # of UTF8 chars generated

    if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
         ((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPLSTR)lpDestStr))) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (0);
    }

    while ((cchSrc--) && ((cchDest == 0) || (cchU8 < cchDest)))
    {
        if (*lpLC <= ASCII)
        {
            //  Found ASCII.
            if (cchDest)
            {
                lpDestStr[cchU8] = (char)*lpLC;
            }
            cchU8++;
        }
        else if (*lpLC <= UTF8_2_MAX)
        {
            //  Found 2 byte sequence if < 0x07ff (11 bits).
            if (cchDest)
            {
                if ((cchU8 + 1) < cchDest)
                {
                    cchSrc--;
                    //  Use upper 5 bits in first byte.
                    //  Use lower 6 bits in second byte.
                    lpDestStr[cchU8++] = UTF8_1ST_OF_2 | (*lpLC >> 6);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST1_6_BIT(*lpLC);
                }
                else
                {
                    //  Error - buffer too small.
                    cchSrc++;
                    break;
                }
            }
            else
            {
                cchSrc--;
                cchU8 += 2;
            }
        }
        else if (*lpLC <= UTF8_3_MAX)
        {
            //  Found 3 byte sequence.
            if (cchDest)
            {
                if ((cchU8 + 2) < cchDest)
                {
                    //  Use upper  4 bits in first byte.
                    //  Use middle 6 bits in second byte.
                    //  Use lower  6 bits in third byte.
                    lpDestStr[cchU8++] = UTF8_1ST_OF_3 | (*lpLC >> 12);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST2_6_BIT(*lpLC);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST1_6_BIT(*lpLC);
                }
                else
                {
                    //  Error - buffer too small.
                    cchSrc++;
                    break;
                }
            }
            else
            {
                cchU8 += 3;
            }
        }
        else if (*lpLC <= UTF8_4_MAX)
        {
            //  Found 4 byte sequence.
            if (cchDest)
            {
                if (cchU8+3<cchDest)
                {
                    //  Use upper  3 bits in first byte.
                    //  Use higher 6 bits in second byte.
                    //  Use middle 6 bits in third byte.
                    //  Use lower  6 bits in fourth byte.
                    lpDestStr[cchU8++] = UTF8_1ST_OF_4 | (*lpLC >> 18);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST3_6_BIT(*lpLC);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST2_6_BIT(*lpLC);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST1_6_BIT(*lpLC);
                }
                else
                {
                    //  Error - buffer too small.
                    cchSrc++;
                    break;
                }
            }
            else
            {
                cchU8 += 4;
            }
        }
        else if (*lpLC <= UTF8_5_MAX)
        {
            //  Found 5 byte sequence.
            if (cchDest)
            {
                if (cchU8+4<cchDest)
                {
                    //  Use upper  2 bits in first byte.
                    //  Use next   6 bits in second byte.
                    //  Use higher 6 bits in third byte.
                    //  Use middle 6 bits in fourth byte.
                    //  Use lower  6 bits in fifth byte.
                    lpDestStr[cchU8++] = UTF8_1ST_OF_5 | (*lpLC >> 24);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST4_6_BIT(*lpLC);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST3_6_BIT(*lpLC);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST2_6_BIT(*lpLC);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST1_6_BIT(*lpLC);
                }
                else
                {
                    //  Error - buffer too small.
                    cchSrc++;
                    break;
                }
            }
            else
            {
                cchU8 += 5;
            }
        }
        else
        {
            //  Found 6 byte sequence.
            if (cchDest)
            {
                if (cchU8+5<cchDest)
                {
                    //  Use top    1 bits in first byte.
                    //  Use upper  6 bits in second byte.
                    //  Use next   6 bits in third byte.
                    //  Use higher 6 bits in fourth byte.
                    //  Use middle 6 bits in fifth byte.
                    //  Use lower  6 bits in sixth byte.
                    lpDestStr[cchU8++] = UTF8_1ST_OF_6 | (*lpLC >> 30);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST5_6_BIT(*lpLC);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST4_6_BIT(*lpLC);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST3_6_BIT(*lpLC);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST2_6_BIT(*lpLC);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LAST1_6_BIT(*lpLC);
                }
                else
                {
                    //  Error - buffer too small.
                    cchSrc++;
                    break;
                }
            }
            else
            {
                cchU8 += 6;
            }
        }

        lpLC++;
    }

    //  Make sure the destination buffer was large enough.
    if (cchDest && (cchSrc >= 0))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return (0);
    }

    //  Return the number of UTF-8 characters written.
    return (cchU8);
}
