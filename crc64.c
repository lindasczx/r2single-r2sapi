/* crc64.c -- compute the CRC-64/XZ of a data stream
 * Copyright (C) 1995-2006, 2010, 2011, 2012, 2016 Mark Adler
 * Copyright (C) 2007-2009 Lasse Collin
 * Copyright (C) 2014, 2017 Linda Zhang
 *
 * Thanks to Lasse Collin <lasse.collin@tukaani.org> for his contribution of faster CRC methods:
 * using the slice-by-four algorithm instead of slice-by-eight to avoid increasing CPU cache usage.
 * This results in about 1.1x increase in speed on a Intel Core 2 Duo using gcc -O3.
 */

/* @(#) $Id$ */

/*
  Note on the use of DYNAMIC_CRC_TABLE: there is no mutex or semaphore
  protection on the static variables used to control the first-use generation
  of the crc tables.  Therefore, if you #define DYNAMIC_CRC_TABLE, you should
  first call get_crc64_table() to initialize the tables before allowing more than
  one thread to use crc64().

  DYNAMIC_CRC_TABLE and MAKECRCH can be #defined to write out crc64.h.
 */

#ifdef MAKECRCH
#  include <stdio.h>
#  ifndef DYNAMIC_CRC_TABLE
#    define DYNAMIC_CRC_TABLE
#  endif /* !DYNAMIC_CRC_TABLE */
#endif /* MAKECRCH */

//#include "zutil.h"      /* for STDC and FAR definitions */
#include <stddef.h>
#include <windows.h>
#include "crc64.h"
#define OF(args) args
#define Z_NULL NULL
#define ZEXPORT API
#define ZSWAP64(q) (((q) >> 56 & 0xff) | ((q) >> 40 & 0xff00) | ((q) >> 24 & 0xff0000) | ((q) >> 8 & 0xff000000) | \
                   (((q) & 0xff000000) << 8) | (((q) & 0xff0000) << 24) | (((q) & 0xff00) << 40) | (((q) & 0xff) << 56))
//#define ZSWAP64(q) (((q) >> 56 & 0xff) | (((q) >> 48 & 0xff) << 8) | (((q) >> 40 & 0xff) << 16) | (((q) >> 32 & 0xff) << 24) | \
//                   (((q) >> 24 & 0xff) << 32) | (((q) >> 16 & 0xff) << 40) | (((q) >> 8 & 0xff) << 48) | (((q) & 0xff) << 56))
typedef unsigned __int64 z_crc_t;
typedef __int64 z_off64_t;
typedef size_t z_size_t;
#ifndef local
#  define local static
#endif
/* since "static" is used to mean two completely different things in C, we
   define "local" for the non-static meaning of "static", for readability
   (compile with -Dlocal if your debugger can't find static symbols) */

/* Definitions for doing the crc 8 data bytes at a time. */
#if !defined(NOBYFOUR)
#  define BYFOUR
#endif
#ifdef BYFOUR
   local z_crc_t crc64_little OF((z_crc_t, const unsigned char FAR *, unsigned __int64));
   //local z_crc_t crc64_big OF((z_crc_t, const unsigned char FAR *, unsigned __int64));
#  if defined(BIGENDIAN) || defined(MAKECRCH)
#    define TBLS 8
#  else
#    define TBLS 4
#  endif /* BIGENDIAN */
#else
#  define TBLS 1
#endif /* BYFOUR */

/* Local functions for crc concatenation */
local z_crc_t gf2_matrix_times OF((z_crc_t *mat, z_crc_t vec));
local void gf2_matrix_square OF((z_crc_t *square, z_crc_t *mat));
local z_crc_t crc64_combine_ OF((z_crc_t crc1, z_crc_t crc2, z_off64_t len2));


#ifdef DYNAMIC_CRC_TABLE

local volatile int crc_table_empty = 1;
local z_crc_t FAR crc_table[TBLS][256];
local void make_crc_table OF((void));
#ifdef MAKECRCH
   local void write_table OF((FILE *, const z_crc_t FAR *));
#endif /* MAKECRCH */
/*
  Generate tables for a byte-wise 64-bit CRC calculation on the polynomial:
  x^64+x^62+x^57+x^55+x^54+x^53+x^52+x^47+x^46+x^45+x^40+x^39+x^38+x^37+x^35+x^33+x^32+x^31+x^29+x^27+x^24+x^23+x^22+x^21+x^19+x^17+x^13+x^12+x^10+x^9+x^7+x^4+x+1.

  Polynomials over GF(2) are represented in binary, one bit per coefficient,
  with the lowest powers in the most significant bit.  Then adding polynomials
  is just exclusive-or, and multiplying a polynomial by x is a right shift by
  one.  If we call the above polynomial p, and represent a byte as the
  polynomial q, also with the lowest power in the most significant bit (so the
  byte 0xb1 is the polynomial x^7+x^3+x+1), then the CRC is (q*x^64) mod p,
  where a mod b means the remainder after dividing a by b.

  This calculation is done using the shift-register method of multiplying and
  taking the remainder.  The register is initialized to zero, and for each
  incoming bit, x^64 is added mod p to the register if the bit is a one (where
  x^64 mod p is p+x^64 = x^62+...+1), and the register is multiplied mod p by
  x (which is shifting right by one and adding x^64 mod p if the bit shifted
  out is a one).  We start with the highest power (least significant bit) of
  q and repeat for all eight bits of q.

  The first table is simply the CRC of all possible eight bit values.  This is
  all the information needed to generate CRCs on data a byte at a time for all
  combinations of CRC register values and incoming bytes.  The remaining tables
  allow for word-at-a-time CRC calculation for both big-endian and little-
  endian machines, where a word is four bytes.
*/
local void make_crc_table()
{
    z_crc_t c;
    int n, k;
    z_crc_t poly;                       /* polynomial exclusive-or pattern */
    /* terms of polynomial defining this crc (except x^64): */
    static volatile int first = 1;      /* flag to limit concurrent making */
    static const unsigned char p[] = {62,57,55,54,53,52,47,46,45,40,39,38,37,35,33,32,31,29,27,24,23,22,21,19,17,13,12,10,9,7,4,1,0};

    /* See if another task is already doing this (not thread-safe, but better
       than nothing -- significantly reduces duration of vulnerability in
       case the advice about DYNAMIC_CRC_TABLE is ignored) */
    if (first) {
        first = 0;

        /* make exclusive-or pattern from polynomial (0x42f0e1eba9ea3693ULL) */
        poly = 0;
        for (n = 0; n < (int)(sizeof(p)/sizeof(unsigned char)); n++)
            poly |= (z_crc_t)1 << (63 - p[n]);

        /* generate a crc for every 8-bit value */
        for (n = 0; n < 256; n++) {
            c = (z_crc_t)n;
            for (k = 0; k < 8; k++)
                c = c & 1 ? poly ^ (c >> 1) : c >> 1;
            crc_table[0][n] = c;
        }

#ifdef BYFOUR
        /* generate crc for each value followed by one, two, and three zeros,
           and then the byte reversal of those as well as the first table */
        for (n = 0; n < 256; n++) {
            c = crc_table[0][n];
            crc_table[4][n] = ZSWAP64(c);
            for (k = 1; k < 4; k++) {
                c = crc_table[0][c & 0xff] ^ (c >> 8);
                crc_table[k][n] = c;
                crc_table[k + 4][n] = ZSWAP64(c);
            }
        }
#endif /* BYFOUR */

        crc_table_empty = 0;
    }
    else {      /* not first */
        /* wait for the other guy to finish (not efficient, but rare) */
        while (crc_table_empty)
            ;
    }

#ifdef MAKECRCH
    /* write out CRC tables to crc64.h */
    {
        FILE *out;

        out = fopen("crc64_table.h", "w");
        if (out == NULL) return;
        fprintf(out, "/* crc64_table.h -- tables for rapid CRC-64/XZ calculation\n");
        fprintf(out, " * Generated automatically by crc64.c\n */\n\n");
        fprintf(out, "local const z_crc_t FAR ");
        fprintf(out, "crc_table[TBLS][256] =\n{\n  {\n");
        write_table(out, crc_table[0]);
#  ifdef BYFOUR
        fprintf(out, "#ifdef BYFOUR\n");
        for (k = 1; k < 4; k++) {
            fprintf(out, "  },\n  {\n");
            write_table(out, crc_table[k]);
        }
        fprintf(out, "#ifdef BIGENDIAN\n");
        for (k = 4; k < 8; k++) {
            fprintf(out, "  },\n  {\n");
            write_table(out, crc_table[k]);
        }
        fprintf(out, "#endif\n");
        fprintf(out, "#endif\n");
#  endif /* BYFOUR */
        fprintf(out, "  }\n};\n");
        fclose(out);
    }
#endif /* MAKECRCH */
}

#ifdef MAKECRCH
local void write_table(out, table)
    FILE *out;
    const z_crc_t FAR *table;
{
    int n;

    for (n = 0; n < 256; n++)
        fprintf(out, "%s0x%016I64xULL%s", n % 4 ? "" : "    ",
                (z_crc_t)(table[n]),
                n == 255 ? "\n" : (n % 4 == 3 ? ",\n" : ", "));
}
#endif /* MAKECRCH */

#else /* !DYNAMIC_CRC_TABLE */
/* ========================================================================
 * Tables of CRC-64s of all single-byte values, made by make_crc_table().
 */
#include "crc64_table.h"
#endif /* DYNAMIC_CRC_TABLE */

/* =========================================================================
 * This function can be used by asm versions of crc64()
 */
const z_crc_t FAR * ZEXPORT get_crc64_table()
{
#ifdef DYNAMIC_CRC_TABLE
    if (crc_table_empty)
        make_crc_table();
#endif /* DYNAMIC_CRC_TABLE */
    return (const z_crc_t FAR *)crc_table;
}

/* ========================================================================= */
#define DO1 crc = crc_table[0][((z_crc_t)crc ^ (*buf++)) & 0xff] ^ (crc >> 8)
#define DO8 DO1; DO1; DO1; DO1; DO1; DO1; DO1; DO1

/* ========================================================================= */
z_crc_t ZEXPORT crc64_z(crc, buf, len)
    z_crc_t crc;
    const unsigned char FAR *buf;
    z_size_t len;
{
    if (buf == Z_NULL) return 0ULL;

#ifdef DYNAMIC_CRC_TABLE
    if (crc_table_empty)
        make_crc_table();
#endif /* DYNAMIC_CRC_TABLE */

#ifdef BYFOUR
    if (sizeof(void *) == sizeof(ptrdiff_t)) {
        //z_crc_t endian;

        //endian = 1;
        //if (*((unsigned char *)(&endian)))
            return crc64_little(crc, buf, len);
        //else
        //    return crc64_big(crc, buf, len);
    }
#endif /* BYFOUR */
    crc = crc ^ 0xffffffffffffffffULL;
    while (len >= 8) {
        DO8;
        len -= 8;
    }
    if (len) do {
        DO1;
    } while (--len);
    crc = crc ^ 0xffffffffffffffffULL;
    return crc;
}

/* ========================================================================= */
unsigned int ZEXPORT crc64(crc, buf, len)
    z_crc_t crc;
    const unsigned char FAR *buf;
    unsigned __int64 len;
{
    return crc64_z(crc, buf, len);
}

#ifdef BYFOUR


/*
   This BYFOUR code accesses the passed unsigned char * buffer with a 64-bit
   integer pointer type. This violates the strict aliasing rule, where a
   compiler can assume, for optimization purposes, that two pointers to
   fundamentally different types won't ever point to the same memory. This can
   manifest as a problem only if one of the pointers is written to. This code
   only reads from those pointers. So long as this code remains isolated in
   this compilation unit, there won't be a problem. For this reason, this code
   should not be copied and pasted into a compilation unit in which other code
   writes to the buffer that is passed to these routines.
 */

/* ========================================================================= */
#define DOLIT4 { \
	register unsigned int t = c ^ *(unsigned int *)(buf); \
	buf += 4; \
        c = crc_table[3][t & 0xff] ^ crc_table[2][t >> 8 & 0xff] ^ \
            c >> 32 ^ \
	    crc_table[1][t >> 16 & 0xff] ^ crc_table[0][t >> 24]; }
#define DOLIT32 DOLIT4; DOLIT4; DOLIT4; DOLIT4; DOLIT4; DOLIT4; DOLIT4; DOLIT4

/* ========================================================================= */
local z_crc_t crc64_little(crc, buf, len)
    z_crc_t crc;
    const unsigned char FAR *buf;
    unsigned __int64 len;
{
    register z_crc_t c;

    c = (z_crc_t)crc;
    c = ~c;
    while (len && ((ptrdiff_t)buf & 3)) {
        c = crc_table[0][(c ^ *buf++) & 0xff] ^ (c >> 8);
        len--;
    }

    while (len >= 32) {
        DOLIT32;
        len -= 32;
    }
    while (len >= 4) {
        DOLIT4;
        len -= 4;
    }

    if (len) do {
        c = crc_table[0][(c ^ *buf++) & 0xff] ^ (c >> 8);
    } while (--len);
    c = ~c;
    return (z_crc_t)c;
}

/* ========================================================================= */
#define DOBIG4 { \
	register unsigned int t = c >> 32 ^ *(unsigned int *)(buf); \
	buf += 4; \
        c = crc_table[4][t & 0xff] ^ crc_table[5][t >> 8 & 0xff] ^ \
            c << 32 ^ \
	    crc_table[6][t >> 16 & 0xff] ^ crc_table[7][t >> 24]; }
#define DOBIG32 DOBIG4; DOBIG4; DOBIG4; DOBIG4; DOBIG4; DOBIG4; DOBIG4; DOBIG4

/* ========================================================================= */
/*
local z_crc_t crc64_big(crc, buf, len)
    z_crc_t crc;
    const unsigned char FAR *buf;
    unsigned __int64 len;
{
    register z_crc_t c;

    c = ZSWAP64((z_crc_t)crc);
    c = ~c;
    while (len && ((ptrdiff_t)buf & 3)) {
        c = crc_table[4][(c >> 56) ^ *buf++] ^ (c << 8);
        len--;
    }

    while (len >= 32) {
        DOBIG32;
        len -= 32;
    }
    while (len >= 4) {
        DOBIG4;
        len -= 4;
    }
    if (len) do {
        c = crc_table[4][(c >> 56) ^ *buf++] ^ (c << 8);
    } while (--len);
    c = ~c;
    return (z_crc_t)(ZSWAP64(c));
}
*/
#endif /* BYFOUR */

#define GF2_DIM 64      /* dimension of GF(2) vectors (length of CRC) */

/* ========================================================================= */
local z_crc_t gf2_matrix_times(mat, vec)
    z_crc_t *mat;
    z_crc_t vec;
{
    z_crc_t sum;

    sum = 0;
    while (vec) {
        if (vec & 1)
            sum ^= *mat;
        vec >>= 1;
        mat++;
    }
    return sum;
}

/* ========================================================================= */
local void gf2_matrix_square(square, mat)
    z_crc_t *square;
    z_crc_t *mat;
{
    int n;

    for (n = 0; n < GF2_DIM; n++)
        square[n] = gf2_matrix_times(mat, mat[n]);
}

/* ========================================================================= */
local z_crc_t crc64_combine_(crc1, crc2, len2)
    z_crc_t crc1;
    z_crc_t crc2;
    z_off64_t len2;
{
    int n;
    z_crc_t row;
    z_crc_t even[GF2_DIM];    /* even-power-of-two zeros operator */
    z_crc_t odd[GF2_DIM];     /* odd-power-of-two zeros operator */

    /* degenerate case (also disallow negative lengths) */
    if (len2 <= 0)
        return crc1;

    /* put operator for one zero bit in odd */
    odd[0] = 0xC96C5795D7870F42ULL; //0x42f0e1eba9ea3693ULL;          /* CRC-64 polynomial */
    row = 1;
    for (n = 1; n < GF2_DIM; n++) {
        odd[n] = row;
        row <<= 1;
    }

    /* put operator for two zero bits in even */
    gf2_matrix_square(even, odd);

    /* put operator for four zero bits in odd */
    gf2_matrix_square(odd, even);

    /* apply len2 zeros to crc1 (first square will put the operator for one
       zero byte, eight zero bits, in even) */
    do {
        /* apply zeros operator for this bit of len2 */
        gf2_matrix_square(even, odd);
        if (len2 & 1)
            crc1 = gf2_matrix_times(even, crc1);
        len2 >>= 1;

        /* if no more bits set, then done */
        if (len2 == 0)
            break;

        /* another iteration of the loop with odd and even swapped */
        gf2_matrix_square(odd, even);
        if (len2 & 1)
            crc1 = gf2_matrix_times(odd, crc1);
        len2 >>= 1;

        /* if no more bits set, then done */
    } while (len2 != 0);

    /* return combined crc */
    crc1 ^= crc2;
    return crc1;
}

/* ========================================================================= */
z_crc_t ZEXPORT crc64_combine(crc1, crc2, len2)
    z_crc_t crc1;
    z_crc_t crc2;
    z_off64_t len2;
{
    return crc64_combine_(crc1, crc2, len2);
}

