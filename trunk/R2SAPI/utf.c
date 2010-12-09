#include <windows.h>

typedef int * LPLSTR;
typedef const int * LPCLSTR;

//  Constant Declarations.

#define ASCII             0x007f
#define UTF8_2_MAX        0x07ff  // max UTF8 2-byte sequence (32 * 64 = 2048)
#define SURROGATE1_MIN    0xd800
#define SURROGATE1_MAX    0xdbff
#define SURROGATE2_MIN    0xdc00
#define SURROGATE2_MAX    0xdfff
#define UTF8_1ST_OF_2     0xc0    // 110x xxxx
#define UTF8_1ST_OF_3     0xe0    // 1110 xxxx
#define UTF8_1ST_OF_4     0xf0    // 1111 0xxx
#define UTF8_TRAIL        0x80    // 10xx xxxx

#define MAKEUTF32(u1,u2)  (((((u1)&0x03ff)<<10)|((u2)&0x03ff))+0x10000)
#define MAKESURROGATE1(u) (0xd800|((((u)-0x10000)>>10)&0x3ff))
#define MAKESURROGATE2(u) (0xdc00|((u)&0x3ff))

#define SWAPBYTEORDER2(u) ((((u)>>8)&0xff)|(((u)<<8)&0xff00))
#define SWAPBYTEORDER4(u) ((((u)>>24)&0xff)|(((u)>>8)&0xff00)|(((u)<<8)&0xff0000)|((u)<<24))

#define HIGER_6_BIT(u)    ((u) >> 12)
#define HIGHER_6_BIT(u)   (((u) & 0x3f000) >> 12)
#define MIDDLE_6_BIT(u)   (((u) & 0x0fc0) >> 6)
#define LOWER_6_BIT(u)    ((u) & 0x003f)

#define BIT7(a)           ((a) & 0x80)
#define BIT6(a)           ((a) & 0x40)



//-------------------------------------------------------------------------//
//                           INTERNAL ROUTINES                             //
//-------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////
//
//  UTF8ToUnicode
//
//  Maps a UTF-8 character string to its wide character string counterpart.
//
//  1996-02-06  JulieB    Created.
//  2010-12-08  ********** Edited.
////////////////////////////////////////////////////////////////////////////

int WINAPI UTF8ToUnicode(
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
                utf32 |= LOWER_6_BIT(*pUTF8);

                if (nTB == 0)
                {
                    //  End of sequence.  Advance the output counter.
                    if(utf32<0x10000)
                    {
		                if (cchDest)
		                {
	                        lpDestStr[cchWC]=utf32;
						}
						cchWC++;
					}
					else
					{
		                if ((cchWC + 1) < cchDest)
		                {
			                if (cchDest)
			                {
		                        lpDestStr[cchWC]=MAKESURROGATE1(utf32);
		                        lpDestStr[cchWC+1]=MAKESURROGATE2(utf32);
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
//  UnicodeToUTF8
//
//  Maps a Unicode character string to its UTF-8 string counterpart.
//
//  1996-02-06  JulieB    Created.
//  2010-12-08  ********** Edited.
////////////////////////////////////////////////////////////////////////////

int WINAPI UnicodeToUTF8(
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
                    cchSrc--;
                    //  Use upper 5 bits in first byte.
                    //  Use lower 6 bits in second byte.
                    lpDestStr[cchU8++] = UTF8_1ST_OF_2 | (*lpWC >> 6);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LOWER_6_BIT(*lpWC);
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
        else if ((*lpWC >= SURROGATE1_MIN) && (*lpWC <= SURROGATE1_MAX))
        {
            //  Found 4 byte sequence.
            if (cchDest)
            {
                if (cchSrc>=2 && cchU8+3<cchDest)
                {
					cchSrc--;
					utf32=MAKEUTF32(*lpWC,*(lpWC+1));
			        lpWC++;
                    //  Use upper  3 bits in first byte.
                    //  Use higher 6 bits in second byte.
                    //  Use middle 6 bits in third byte.
                    //  Use lower  6 bits in fourth byte.
                    lpDestStr[cchU8++] = UTF8_1ST_OF_4 | (utf32 >> 18);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | HIGHER_6_BIT(utf32);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | MIDDLE_6_BIT(utf32);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LOWER_6_BIT(utf32);
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
            //  Found 3 byte sequence.
            if (cchDest)
            {
                if ((cchU8 + 2) < cchDest)
                {
                    //  Use upper  4 bits in first byte.
                    //  Use middle 6 bits in second byte.
                    //  Use lower  6 bits in third byte.
                    lpDestStr[cchU8++] = UTF8_1ST_OF_3 | (*lpWC >> 12);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | MIDDLE_6_BIT(*lpWC);
                    lpDestStr[cchU8++] = UTF8_TRAIL    | LOWER_6_BIT(*lpWC);
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


int WINAPI UTF16BEToUnicode(
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

int WINAPI UTF32ToUnicode(
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
		if(*pUTF32<0x10000)
		{
	        if (cchDest)
	        {
	            lpDestStr[cchWC] = *pUTF32;
	        }
        	cchWC++;
		}
		else
		{
			if(cchWC+1<cchDest)
			{
		        if (cchDest)
		        {
		            lpDestStr[cchWC] = MAKESURROGATE1(*pUTF32);
		            lpDestStr[cchWC+1] = MAKESURROGATE2(*pUTF32);
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

int WINAPI UnicodeToUTF32(
    LPCWSTR lpSrcStr,
    int cchSrc,
    LPLSTR lpDestStr,
    int cchDest)
{
    LPCWSTR lpWC = lpSrcStr;
    int utf32;
    int cchU32 = 0;                // # of UTF8 chars generated

    if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
         ((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPWSTR)lpDestStr))) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (0);
    }

    while ((cchSrc--) && ((cchDest == 0) || (cchU32 < cchDest)))
    {
        if ((*lpWC >= SURROGATE1_MIN) && (*lpWC <= SURROGATE1_MAX))
        {
            if (cchDest)
            {
                if (cchSrc>=2)
                {
					utf32=MAKEUTF32(*lpWC,*(lpWC+1));
                    lpDestStr[cchU32] = utf32;
                }
                else
                {
                    //  Error - buffer too small.
                    cchSrc++;
                    break;
                }
            }
			cchSrc--;
			lpWC++;
			cchU32++;
		}
		else
		{
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

    //  Return the number of UTF-8 characters written.
    return (cchU32);
}

int WINAPI UTF32BEToUnicode(
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
		if(utf32<0x10000)
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
		            lpDestStr[cchWC] = MAKESURROGATE1(utf32);
		            lpDestStr[cchWC+1] = MAKESURROGATE2(utf32);
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

int WINAPI UnicodeToUTF32BE(
    LPCWSTR lpSrcStr,
    int cchSrc,
    LPLSTR lpDestStr,
    int cchDest)
{
    LPCWSTR lpWC = lpSrcStr;
    int utf32;
    int cchU32 = 0;                // # of UTF8 chars generated

    if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
         ((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPWSTR)lpDestStr))) )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (0);
    }

    while ((cchSrc--) && ((cchDest == 0) || (cchU32 < cchDest)))
    {
        if ((*lpWC >= SURROGATE1_MIN) && (*lpWC <= SURROGATE1_MAX))
        {
            if (cchDest)
            {
                if (cchSrc>=2)
                {
					utf32=MAKEUTF32(*lpWC,*(lpWC+1));
                    lpDestStr[cchU32] = SWAPBYTEORDER4(utf32);
                }
                else
                {
                    //  Error - buffer too small.
                    cchSrc++;
                    break;
                }
            }
			cchSrc--;
	        lpWC++;
            cchU32++;
		}
		else
		{
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

    //  Return the number of UTF-8 characters written.
    return (cchU32);
}

