/*++
Module Name:
	utf7.c

Abstract:
	This file contains functions that convert UTF-7 strings to UTF-16
	strings and UTF-16 string to UTF-7 strings.

--*/

#include "utf.h"

#define SHIFT_IN              '+'     // beginning of a shift sequence
#define SHIFT_OUT             '-'     // end       of a shift sequence
#define ASCII                 0x7f

/////////////////////////
//                     //
//  Unicode -> UTF-7   //
//                     //
/////////////////////////

//
//  Convert one Unicode to 2 2/3 Base64 chars in a shifted sequence.
//  Each char represents a 6-bit portion of the 16-bit Unicode char.
//
CONST char cBase64[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"  // A : 000000 .... 011001  ( 0 - 25)
	"abcdefghijklmnopqrstuvwxyz"  // a : 011010 .... 110011  (26 - 51)
	"0123456789"                  // 0 : 110100 .... 111101  (52 - 61)
	"+/";                         // + : 111110, / : 111111  (62 - 63)

//
//  To determine if an ASCII char needs to be shifted.
//    1 :     to be shifted
//    0 : not to be shifted
//
CONST BOOLEAN fShiftChar[] = {
	0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1,    // Null, Tab, LF, CR
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0,    // Space '() +,-./
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,    // 0123456789:    ?
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    //  ABCDEFGHIJKLMNO
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,    // PQRSTUVWXYZ
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    //  abcdefghijklmno
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1     // pqrstuvwxyz
};

/////////////////////////
//                     //
//  UTF-7 -> Unicode   //
//                     //
/////////////////////////

//
//  Convert a Base64 char in a shifted sequence to a 6-bit portion of a
//  Unicode char.
//  -1 means it is not a Base64
//
CONST char nBitBase64[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,   //            +   /
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,   // 0123456789
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,   //  ABCDEFGHIJKLMNO
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,   // PQRSTUVWXYZ
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,   //  abcdefghijklmno
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1    // pqrstuvwxyz
};

//-------------------------------------------------------------------------//
//                           INTERNAL ROUTINES                             //
//-------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////
//
//  UTF7ToUnicode
//
//  Maps a UTF-7 character string to its wide character string counterpart.
//
////////////////////////////////////////////////////////////////////////////

int API UTF7ToUTF16(
    LPCSTR lpSrcStr,
    int cchSrc,
    LPWSTR lpDestStr,
    int cchDest)
{
    const unsigned char *pUTF7 = (const unsigned char *)lpSrcStr;
    BOOL fShift = FALSE;
    DWORD dwBit = 0;              // 32-bit buffer to hold temporary bits
    int iPos = 0;                 // 6-bit position pointer in the buffer
    int cchWC = 0;                // # of Unicode code points generated

	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPSTR)lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	if (cchSrc <= -1)
	{
		cchSrc = strlen(lpSrcStr) + 1;
	}

    while ((cchSrc--) && ((cchDest == 0) || (cchWC < cchDest)))
    {
        if (*pUTF7 > ASCII)
        {
            //
            //  Error - non ASCII char, so zero extend it.
            //
            if (cchDest)
            {
                lpDestStr[cchWC] = (WCHAR)*pUTF7;
            }
            cchWC++;
        }
        else if (!fShift)
        {
            //
            //  Not in shifted sequence.
            //
            if (*pUTF7 == SHIFT_IN)
            {
                if (cchSrc && (pUTF7[1] == SHIFT_OUT))
                {
                    //
                    //  "+-" means "+"
                    //
                    if (cchDest)
                    {
                        lpDestStr[cchWC] = (WCHAR)*pUTF7;
                    }
                    pUTF7++;
                    cchSrc--;
                    cchWC++;
                }
                else
                {
                    //
                    //  Start a new shift sequence.
                    //
                    fShift = TRUE;
                }
            }
            else
            {
                //
                //  No need to shift.
                //
                if (cchDest)
                {
                    lpDestStr[cchWC] = (WCHAR)*pUTF7;
                }
                cchWC++;
            }
        }
        else
        {
            //
            //  Already in shifted sequence.
            //
            if (nBitBase64[*pUTF7] == -1)
            {
                //
                //  Any non Base64 char also ends shift state.
                //
                if (*pUTF7 != SHIFT_OUT)
                {
                    //
                    //  Not "-", so write it to the buffer.
                    //
                    if (cchDest)
                    {
                        lpDestStr[cchWC] = (WCHAR)*pUTF7;
                    }
                    cchWC++;
                }

                //
                //  Reset bits.
                //
                fShift = FALSE;
                dwBit = 0;
                iPos = 0;
            }
            else
            {
                //
                //  Store the bits in the 6-bit buffer and adjust the
                //  position pointer.
                //
                dwBit |= ((DWORD)nBitBase64[*pUTF7]) << (26 - iPos);
                iPos += 6;
            }

            //
            //  Output the 16-bit Unicode value.
            //
            while (iPos >= 16)
            {
                if (cchDest)
                {
                    if (cchWC < cchDest)
                    {
                        lpDestStr[cchWC] = (WCHAR)(dwBit >> 16);
                    }
                    else
                    {
                        break;
                    }
                }
                cchWC++;

                dwBit <<= 16;
                iPos -= 16;
            }
            if (iPos >= 16)
            {
                //
                //  Error - buffer too small.
                //
                cchSrc++;
                break;
            }
        }

        pUTF7++;
    }

    //
    //  Make sure the destination buffer was large enough.
    //
    if (cchDest && (cchSrc >= 0))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return (0);
    }

    //
    //  Return the number of Unicode characters written.
    //
    return (cchWC);
}


////////////////////////////////////////////////////////////////////////////
//
//  UnicodeToUTF7
//
//  Maps a Unicode character string to its UTF-7 string counterpart.
//
////////////////////////////////////////////////////////////////////////////

int API UTF16ToUTF7(
    LPCWSTR lpSrcStr,
    int cchSrc,
    LPSTR lpDestStr,
    int cchDest)
{
    LPCWSTR lpWC = lpSrcStr;
    BOOL fShift = FALSE;
    DWORD dwBit = 0;              // 32-bit buffer
    int iPos = 0;                 // 6-bit position in buffer
    int cchU7 = 0;                // # of UTF7 chars generated

	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPWSTR)lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	if (cchSrc <= -1)
	{
		cchSrc = wcslen(lpSrcStr) + 1;
	}

    while ((cchSrc--) && ((cchDest == 0) || (cchU7 < cchDest)))
    {
        if ((*lpWC > ASCII) || (fShiftChar[*lpWC]))
        {
            //
            //  Need shift.  Store 16 bits in buffer.
            //
            dwBit |= ((DWORD)*lpWC) << (16 - iPos);
            iPos += 16;

            if (!fShift)
            {
                //
                //  Not in shift state, so add "+".
                //
                if (cchDest)
                {
                    lpDestStr[cchU7] = SHIFT_IN;
                }
                cchU7++;

                //
                //  Go into shift state.
                //
                fShift = TRUE;
            }

            //
            //  Output 6 bits at a time as Base64 chars.
            //
            while (iPos >= 6)
            {
                if (cchDest)
                {
                    if (cchU7 < cchDest)
                    {
                        //
                        //  26 = 32 - 6
                        //
                        lpDestStr[cchU7] = cBase64[(int)(dwBit >> 26)];
                    }
                    else
                    {
                        break;
                    }
                }

                cchU7++;
                dwBit <<= 6;           // remove from bit buffer
                iPos -= 6;             // adjust position pointer
            }
            if (iPos >= 6)
            {
                //
                //  Error - buffer too small.
                //
                cchSrc++;
                break;
            }
        }
        else
        {
            //
            //  No need to shift.
            //
            if (fShift)
            {
                //
                //  End the shift sequence.
                //
                fShift = FALSE;

                if (iPos != 0)
                {
                    //
                    //  Some bits left in dwBit.
                    //
                    if (cchDest)
                    {
                        if ((cchU7 + 1) < cchDest)
                        {
                            lpDestStr[cchU7++] = cBase64[(int)(dwBit >> 26)];
                            lpDestStr[cchU7++] = SHIFT_OUT;
                        }
                        else
                        {
                            //
                            //  Error - buffer too small.
                            //
                            cchSrc++;
                            break;
                        }
                    }
                    else
                    {
                        cchU7 += 2;
                    }

                    dwBit = 0;         // reset bit buffer
                    iPos  = 0;         // reset postion pointer
                }
                else
                {
                    //
                    //  Simply end the shift sequence.
                    //
                    if (cchDest)
                    {
                        lpDestStr[cchU7++] = SHIFT_OUT;
                    }
                    else
                    {
                        cchU7++;
                    }
                }
            }

            //
            //  Write the character to the buffer.
            //  If the character is "+", then write "+-".
            //
            if (cchDest)
            {
                if (cchU7 < cchDest)
                {
                    lpDestStr[cchU7++] = (char)*lpWC;

                    if (*lpWC == SHIFT_IN)
                    {
                        if (cchU7 < cchDest)
                        {
                            lpDestStr[cchU7++] = SHIFT_OUT;
                        }
                        else
                        {
                            //
                            //  Error - buffer too small.
                            //
                            cchSrc++;
                            break;
                        }
                    }
                }
                else
                {
                    //
                    //  Error - buffer too small.
                    //
                    cchSrc++;
                    break;
                }
            }
            else
            {
                cchU7++;

                if (*lpWC == SHIFT_IN)
                {
                    cchU7++;
                }
            }
        }

        lpWC++;
    }

    //
    //  See if we're still in the shift state.
    //
    if (fShift)
    {
        if (iPos != 0)
        {
            //
            //  Some bits left in dwBit.
            //
            if (cchDest)
            {
                if ((cchU7 + 1) < cchDest)
                {
                    lpDestStr[cchU7++] = cBase64[(int)(dwBit >> 26)];
                    lpDestStr[cchU7++] = SHIFT_OUT;
                }
                else
                {
                    //
                    //  Error - buffer too small.
                    //
                    cchSrc++;
                }
            }
            else
            {
                cchU7 += 2;
            }
        }
        else
        {
            //
            //  Simply end the shift sequence.
            //
            if (cchDest)
            {
                lpDestStr[cchU7++] = SHIFT_OUT;
            }
            else
            {
                cchU7++;
            }
        }
    }

    //
    //  Make sure the destination buffer was large enough.
    //
    if (cchDest && (cchSrc >= 0))
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return (0);
    }

    //
    //  Return the number of UTF-7 characters written.
    //
    return (cchU7);
}

