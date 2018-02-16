/*++
Module Name:
	utf.c

Abstract:
	This file contains functions that convert UTF strings to Unicode
	strings and Unicode string to UTF strings.
	
	Unicode 6.0.0:
	UTF-32 range: 00000000 - 0010FFFF
	UTF-16 range: 0000 - DBFF DFFF, no supersurrogates defined right now.
	UTF-8  range: 00 - F4 8F BF BF
	UTF-EBCDIC I8 range: 00 - F9 A1 BF BF BF
	UTF-1  range: 00 - FC 21 39 6E 6C
--*/

#include "utf.h"
#include "gb18030.h"

//  Constant Declarations.

#define UTF8_1_MAX		0x7f
#define UTF8_2_MAX		0x7ff  // max UTF8 2-byte sequence (32*64=2048)
#define UTF8_3_MAX		0xffff  // max UTF8 3-byte sequence (16*64*64=65536)
#define UTF8_4_MAX		0x1fffff // max UTF8 4-byte sequence (8*64*64*64=2097152)
#define UTF8_5_MAX		0x3ffffff // max UTF8 5-byte sequence (4*64*64*64*64=67108864)
#define UTF8_6_MAX		0x7fffffff // max UTF8 6-byte sequence (maxint)
#define UTF8_1ST_OF_2		0xc0	// 110x xxxx
#define UTF8_1ST_OF_3		0xe0	// 1110 xxxx
#define UTF8_1ST_OF_4		0xf0	// 1111 0xxx
#define UTF8_1ST_OF_5		0xf8	// 1111 10xx
#define UTF8_1ST_OF_6		0xfc	// 1111 110x
#define UTF8_TRAIL		0x80	// 10xx xxxx
#define UTF8_TRAIL_MASK		0xc0	// 11xx xxxx

#define TOP_6_BIT(u)		((u) >> 30)
#define LAST5_6_BIT(u)		(((u) & 0x3f000000) >> 24)
#define LAST4_6_BIT(u)		(((u) & 0xfc0000) >> 18)
#define LAST3_6_BIT(u)		(((u) & 0x3f000) >> 12)
#define LAST2_6_BIT(u)		(((u) & 0xfc0) >> 6)
#define LAST1_6_BIT(u)		((u) & 0x3f)

#define UTFEBCDIC_1_MAX		0x9f
#define UTFEBCDIC_2_MAX		0x3ff  // max UTF-EBCDIC 2-byte sequence (32*32=1024)
#define UTFEBCDIC_3_MAX		0x3fff  // max UTF-EBCDIC 3-byte sequence (16*32*32=16384)
#define UTFEBCDIC_4_MAX		0x3ffff  // max UTF-EBCDIC 4-byte sequence (8*32*32*32=262144)
#define UTFEBCDIC_5_MAX		0x3fffff  // max UTF-EBCDIC 5-byte sequence (4*32*32*32*32=4194304)
#define UTFEBCDIC_6_MAX		0x3ffffff  // max UTF-EBCDIC 6-byte sequence (2*32*32*32*32*32=67108864)
#define UTFEBCDIC_7_MAX		0x7fffffff  // max UTF-EBCDIC 7-byte sequence (maxint)
#define UTFEBCDIC_1ST_OF_2	0xc0	// 110x xxxx
#define UTFEBCDIC_1ST_OF_3	0xe0	// 1110 xxxx
#define UTFEBCDIC_1ST_OF_4	0xf0	// 1111 0xxx
#define UTFEBCDIC_1ST_OF_5	0xf8	// 1111 10xx
#define UTFEBCDIC_1ST_OF_6	0xfc	// 1111 110x
#define UTFEBCDIC_1ST_OF_7	0xfe	// 1111 111x
#define UTFEBCDIC_TRAIL		0xa0	// 101x xxxx
#define UTFEBCDIC_TRAIL_MASK	0xe0	// 111x xxxx

#define TOP_5_BIT(u)		((u) >> 30)
#define LAST6_5_BIT(u)		(((u) & 0x3e000000) >> 25)
#define LAST5_5_BIT(u)		(((u) & 0x1f00000) >> 20)
#define LAST4_5_BIT(u)		(((u) & 0xf8000) >> 15)
#define LAST3_5_BIT(u)		(((u) & 0x7c00) >> 10)
#define LAST2_5_BIT(u)		(((u) & 0x3e0) >> 5)
#define LAST1_5_BIT(u)		((u) & 0x1f)

#define BIT7(a)			((a) & 0x80)
#define BIT6(a)			((a) & 0x40)

#define UTF16_MAX		0x10ffff  // max UTF16 sequence
#define UTF16_2_MAX		0xffff  // max UTF16 2-byte sequence
#define HIGH_SURROGATE_START	0xd800
#define HIGH_SURROGATE_END	0xdbff
#define LOW_SURROGATE_START	0xdc00
#define LOW_SURROGATE_END	0xdfff

#define MAKEUTF32(u1,u2)	(((((u1)&0x03ff)<<10)|((u2)&0x03ff))+0x10000)
#define MAKEHIGHSURROGATE(u)	(0xd800|((((u)-0x10000)>>10)&0x3ff))
#define MAKELOWSURROGATE(u)	(0xdc00|((u)&0x3ff))

#define SWAPBYTEORDER2(u)	((((u)>>8)&0xff)|(((u)<<8)&0xff00))
#define SWAPBYTEORDER4(u)	((((u)>>24)&0xff)|(((u)>>8)&0xff00)|(((u)<<8)&0xff0000)|((u)<<24))

#define UTF1_MODULO		190	// 0xbe
#define UTF1_MODULO_2		(190*190)
#define UTF1_MODULO_3		(190*190*190)
#define UTF1_MODULO_4		(190*190*190*190)
#define UTF1_1_MAX		0x9f
#define UTF1_2_BASE		0x100
#define UTF1_2_MAX		0x4015  // max UTF1 2-byte sequence (96+85*190=16246)
#define UTF1_3_BASE		0x4016
#define UTF1_3_MAX		0x38e2d  // max UTF1 3-byte sequence (6*190*190=216600)
#define UTF1_5_BASE		0x38e2e
#define UTF1_5_MAX		0x7fffffff // max UTF1 5-byte sequence (2*190*190*190*190=2606420000 overflow)
#define UTF1_DUP_OF_2		0xa0	// 0xa0, 0xa0-0xff
#define UTF1_1ST_OF_2		0xa1	// 0xa1-0xf5
#define UTF1_1ST_OF_3		0xf6	// 0xf6-0xfb
#define UTF1_1ST_OF_5		0xfc	// 0xfc-0xfd overflow

static const char I8ToEbcdic[256] = {
	0x00, 0x01, 0x02, 0x03, 0x37, 0x2D, 0x2E, 0x2F, 0x16, 0x05, 0x15, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x3C, 0x3D, 0x32, 0x26, 0x18, 0x19, 0x3F, 0x27, 0x1C, 0x1D, 0x1E, 0x1F,
	0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D, 0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,
	0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,
	0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xAD, 0xE0, 0xBD, 0x5F, 0x6D,
	0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC0, 0x4F, 0xD0, 0xA1, 0x07,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x06, 0x17, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x09, 0x0A, 0x1B,
	0x30, 0x31, 0x1A, 0x33, 0x34, 0x35, 0x36, 0x08, 0x38, 0x39, 0x3A, 0x3B, 0x04, 0x14, 0x3E, 0xFF,
	0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56,
	0x57, 0x58, 0x59, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x70, 0x71, 0x72, 0x73,
	0x74, 0x75, 0x76, 0x77, 0x78, 0x80, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x9A, 0x9B, 0x9C,
	0x9D, 0x9E, 0x9F, 0xA0, 0xAA, 0xAB, 0xAC, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6,
	0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBE, 0xBF, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xDA, 0xDB,
	0xDC, 0xDD, 0xDE, 0xDF, 0xE1, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE,
};
static const unsigned char EbcdicToI8[256] = {
	0x00, 0x01, 0x02, 0x03, 0x9C, 0x09, 0x86, 0x7F, 0x97, 0x8D, 0x8E, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x9D, 0x0A, 0x08, 0x87, 0x18, 0x19, 0x92, 0x8F, 0x1C, 0x1D, 0x1E, 0x1F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x17, 0x1B, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x05, 0x06, 0x07,
	0x90, 0x91, 0x16, 0x93, 0x94, 0x95, 0x96, 0x04, 0x98, 0x99, 0x9A, 0x9B, 0x14, 0x15, 0x9E, 0x1A,
	0x20, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0x2E, 0x3C, 0x28, 0x2B, 0x7C,
	0x26, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0x21, 0x24, 0x2A, 0x29, 0x3B, 0x5E,
	0x2D, 0x2F, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
	0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
	0xC5, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB,
	0xCC, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2,
	0xD3, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0xD4, 0xD5, 0xD6, 0x5B, 0xD7, 0xD8,
	0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0x5D, 0xE6, 0xE7,
	0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED,
	0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3,
	0x5C, 0xF4, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 0x9F,
};

static const char UTF1_T[256] = {
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
	0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
	0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60,
	0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
	0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0xA0, 0xA1,
	0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1,
	0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1,
	0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1,
	0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1,
	0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1,
	0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 0x00, 0x01,
	0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
	0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
};
static const unsigned char UTF1_U[256] = {
	0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD,
	0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD,
	0xDE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
	0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E,
	0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E,
	0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E,
	0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
	0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
	0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
	0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D,
	0x7E, 0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D,
	0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D,
	0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD,
	0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD,
};
//-------------------------------------------------------------------------//
//						   INTERNAL ROUTINES							 //
//-------------------------------------------------------------------------//


////////////////////////////////////////////////////////////////////////////
//
//  cpMakeTrCode
//
//  导出MAKETRCODE宏
//
////////////////////////////////////////////////////////////////////////////

unsigned int API cpMakeTrCode(unsigned short cpfrom, unsigned short cpto) {
	return MAKETRCODE(cpfrom, cpto);
}

////////////////////////////////////////////////////////////////////////////
//
//  cpTrCodeSupported
//
//  字符串编码转换是否支持
//
////////////////////////////////////////////////////////////////////////////

int API cpTrCodeSupported(unsigned int nTrCode){
	switch(nTrCode){
		case MAKETRCODE(cpUTF8, cpUTF16LE):
		case MAKETRCODE(cpUTF8, cpUTF16BE):
		case MAKETRCODE(cpUTF8, cpUTF32LE):
		case MAKETRCODE(cpUTF8, cpUTF32BE):
		case MAKETRCODE(cpUTF8, cpUTFEBCDIC):
		case MAKETRCODE(cpUTF8, cpUTF1):

		case MAKETRCODE(cpUTF16LE, cpUTF8):
		case MAKETRCODE(cpUTF16LE, cpUTF16BE):
		case MAKETRCODE(cpUTF16LE, cpUTF32LE):
		case MAKETRCODE(cpUTF16LE, cpUTF32BE):
		case MAKETRCODE(cpUTF16LE, cpUTFEBCDIC):
		case MAKETRCODE(cpUTF16LE, cpUTF1):
		case MAKETRCODE(cpUTF16LE, cpCESU8):
		case MAKETRCODE(cpUTF16LE, cpUTF7):
		case MAKETRCODE(cpUTF16LE, cpGB18030):

		case MAKETRCODE(cpUTF16BE, cpUTF8):
		case MAKETRCODE(cpUTF16BE, cpUTF16LE):
		case MAKETRCODE(cpUTF16BE, cpUTF32LE):
		case MAKETRCODE(cpUTF16BE, cpUTF32BE):
		case MAKETRCODE(cpUTF16BE, cpUTFEBCDIC):
		case MAKETRCODE(cpUTF16BE, cpUTF1):
		case MAKETRCODE(cpUTF16BE, cpCESU8):

		case MAKETRCODE(cpUTF32LE, cpUTF8):
		case MAKETRCODE(cpUTF32LE, cpUTF16LE):
		case MAKETRCODE(cpUTF32LE, cpUTF16BE):
		case MAKETRCODE(cpUTF32LE, cpUTF32BE):
		case MAKETRCODE(cpUTF32LE, cpUTFEBCDIC):
		case MAKETRCODE(cpUTF32LE, cpUTF1):

		case MAKETRCODE(cpUTF32BE, cpUTF8):
		case MAKETRCODE(cpUTF32BE, cpUTF16LE):
		case MAKETRCODE(cpUTF32BE, cpUTF16BE):
		case MAKETRCODE(cpUTF32BE, cpUTF32LE):
		case MAKETRCODE(cpUTF32BE, cpUTFEBCDIC):
		case MAKETRCODE(cpUTF32BE, cpUTF1):

		case MAKETRCODE(cpUTFEBCDIC, cpUTF8):
		case MAKETRCODE(cpUTFEBCDIC, cpUTF16LE):
		case MAKETRCODE(cpUTFEBCDIC, cpUTF16BE):
		case MAKETRCODE(cpUTFEBCDIC, cpUTF32LE):
		case MAKETRCODE(cpUTFEBCDIC, cpUTF32BE):
		case MAKETRCODE(cpUTFEBCDIC, cpUTF1):

		case MAKETRCODE(cpUTF1, cpUTF8):
		case MAKETRCODE(cpUTF1, cpUTF16LE):
		case MAKETRCODE(cpUTF1, cpUTF16BE):
		case MAKETRCODE(cpUTF1, cpUTF32LE):
		case MAKETRCODE(cpUTF1, cpUTF32BE):
		case MAKETRCODE(cpUTF1, cpUTFEBCDIC):

		case MAKETRCODE(cpCESU8, cpUTF16LE):
		case MAKETRCODE(cpCESU8, cpUTF16BE):

		case MAKETRCODE(cpUTF7, cpUTF16LE):

		case MAKETRCODE(cpGB18030, cpUTF16LE):

		case MAKETRCODE(cpUTF8, cpUTF8):
		case MAKETRCODE(cpUTF16LE, cpUTF16LE):
		case MAKETRCODE(cpUTF16BE, cpUTF16BE):
		case MAKETRCODE(cpUTF32LE, cpUTF32LE):
		case MAKETRCODE(cpUTF32BE, cpUTF32BE):
		case MAKETRCODE(cpUTFEBCDIC, cpUTFEBCDIC):
		case MAKETRCODE(cpUTF1, cpUTF1):
		case MAKETRCODE(cpCESU8, cpCESU8):
		case MAKETRCODE(cpUTF7, cpUTF7):
		case MAKETRCODE(cpGB18030, cpGB18030):
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
////////////////////////////////////////////////////////////////////////////
int API cpConvertEncoding(
	unsigned int nTrCode,
	LPCVOID lpSrcStr,
	int cchSrc,
	LPVOID lpDestStr,
	int cchDest)
{
	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPSTR)lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	switch(nTrCode){
		case MAKETRCODE(cpUTF8, cpUTF16LE):
			return UTF8ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpUTF8, cpUTF16BE):
			return UTF8ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);
		case MAKETRCODE(cpUTF8, cpUTF32LE):
			return UTF8ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpUTF8, cpUTF32BE):
			return UTF8ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);
		case MAKETRCODE(cpUTF8, cpUTFEBCDIC):
			return UTF8ToUTFEBCDIC(lpSrcStr, cchSrc, lpDestStr, cchDest);
		case MAKETRCODE(cpUTF8, cpUTF1):
			return UTF8ToUTF1(lpSrcStr, cchSrc, lpDestStr, cchDest);

		case MAKETRCODE(cpUTF16LE, cpUTF8):
			return UTF16ToUTF8(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpUTF16LE, cpUTF16BE):
		case MAKETRCODE(cpUTF16BE, cpUTF16LE):
			return SwapByte(2, lpSrcStr, cchSrc, lpDestStr, cchDest);
		case MAKETRCODE(cpUTF16LE, cpUTF32LE):
			return UTF16ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE, FALSE);
		case MAKETRCODE(cpUTF16LE, cpUTF32BE):
			return UTF16ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE, TRUE);
		case MAKETRCODE(cpUTF16LE, cpUTFEBCDIC):
			return UTF16ToUTFEBCDIC(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpUTF16LE, cpUTF1):
			return UTF16ToUTF1(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpUTF16LE, cpCESU8):
			return UTF16ToCESU8(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpUTF16LE, cpUTF7):
			return UTF16ToUTF7(lpSrcStr, cchSrc, lpDestStr, cchDest);
		case MAKETRCODE(cpUTF16LE, cpGB18030):
			return UTF16ToGB18030(lpSrcStr, cchSrc, lpDestStr, cchDest);

		case MAKETRCODE(cpUTF16BE, cpUTF8):
			return UTF16ToUTF8(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);
		case MAKETRCODE(cpUTF16BE, cpUTF32LE):
			return UTF16ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE, FALSE);
		case MAKETRCODE(cpUTF16BE, cpUTF32BE):
			return UTF16ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE, TRUE);
		case MAKETRCODE(cpUTF16BE, cpUTFEBCDIC):
			return UTF16ToUTFEBCDIC(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);
		case MAKETRCODE(cpUTF16BE, cpUTF1):
			return UTF16ToUTF1(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);
		case MAKETRCODE(cpUTF16BE, cpCESU8):
			return UTF16ToCESU8(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);

		case MAKETRCODE(cpUTF32LE, cpUTF8):
			return UTF32ToUTF8(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpUTF32LE, cpUTF16LE):
			return UTF32ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE, FALSE);
		case MAKETRCODE(cpUTF32LE, cpUTF16BE):
			return UTF32ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE, TRUE);
		case MAKETRCODE(cpUTF32LE, cpUTF32BE):
		case MAKETRCODE(cpUTF32BE, cpUTF32LE):
			return SwapByte(4, lpSrcStr, cchSrc, lpDestStr, cchDest);
		case MAKETRCODE(cpUTF32LE, cpUTFEBCDIC):
			return UTF32ToUTFEBCDIC(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpUTF32LE, cpUTF1):
			return UTF32ToUTF1(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);

		case MAKETRCODE(cpUTF32BE, cpUTF8):
			return UTF32ToUTF8(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);
		case MAKETRCODE(cpUTF32BE, cpUTF16LE):
			return UTF32ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE, FALSE);
		case MAKETRCODE(cpUTF32BE, cpUTF16BE):
			return UTF32ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE, TRUE);
		case MAKETRCODE(cpUTF32BE, cpUTFEBCDIC):
			return UTF32ToUTFEBCDIC(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);
		case MAKETRCODE(cpUTF32BE, cpUTF1):
			return UTF32ToUTF1(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);

		case MAKETRCODE(cpUTFEBCDIC, cpUTF8):
			return UTFEBCDICToUTF8(lpSrcStr, cchSrc, lpDestStr, cchDest);
		case MAKETRCODE(cpUTFEBCDIC, cpUTF16LE):
			return UTFEBCDICToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpUTFEBCDIC, cpUTF16BE):
			return UTFEBCDICToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);
		case MAKETRCODE(cpUTFEBCDIC, cpUTF32LE):
			return UTFEBCDICToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpUTFEBCDIC, cpUTF32BE):
			return UTFEBCDICToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);
		case MAKETRCODE(cpUTFEBCDIC, cpUTF1):
			return UTFEBCDICToUTF1(lpSrcStr, cchSrc, lpDestStr, cchDest);

		case MAKETRCODE(cpUTF1, cpUTF8):
			return UTF1ToUTF8(lpSrcStr, cchSrc, lpDestStr, cchDest);
		case MAKETRCODE(cpUTF1, cpUTF16LE):
			return UTF1ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpUTF1, cpUTF16BE):
			return UTF1ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);
		case MAKETRCODE(cpUTF1, cpUTF32LE):
			return UTF1ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpUTF1, cpUTF32BE):
			return UTF1ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);
		case MAKETRCODE(cpUTF1, cpUTFEBCDIC):
			return UTF1ToUTFEBCDIC(lpSrcStr, cchSrc, lpDestStr, cchDest);

		case MAKETRCODE(cpCESU8, cpUTF16LE):
			return CESU8ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
		case MAKETRCODE(cpCESU8, cpUTF16BE):
			return CESU8ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE);

		case MAKETRCODE(cpUTF7, cpUTF16LE):
			return UTF7ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest);

		case MAKETRCODE(cpGB18030, cpUTF16LE):
			return GB18030ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest);

		case MAKETRCODE(cpUTF8, cpUTF8):
		case MAKETRCODE(cpUTFEBCDIC, cpUTFEBCDIC):
		case MAKETRCODE(cpUTF1, cpUTF1):
		case MAKETRCODE(cpCESU8, cpCESU8):
		case MAKETRCODE(cpUTF7, cpUTF7):
		case MAKETRCODE(cpGB18030, cpGB18030):
			return CopyString(1, lpSrcStr, cchSrc, lpDestStr, cchDest);
		case MAKETRCODE(cpUTF16LE, cpUTF16LE):
		case MAKETRCODE(cpUTF16BE, cpUTF16BE):
			return CopyString(2, lpSrcStr, cchSrc, lpDestStr, cchDest);
		case MAKETRCODE(cpUTF32LE, cpUTF32LE):
		case MAKETRCODE(cpUTF32BE, cpUTF32BE):
			return CopyString(4, lpSrcStr, cchSrc, lpDestStr, cchDest);

		default:
			SetLastError(ERROR_INVALID_PARAMETER);
			return 0;
	}
}

////////////////////////////////////////////////////////////////////////////
//
//  内部函数
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//  lcslen
//
//  取得字符串长度
//
////////////////////////////////////////////////////////////////////////////

size_t lcslen(LPCLSTR lpSrcStr) {
	LPCLSTR eos = lpSrcStr;
	while( *eos++ ) ;
	return( (size_t)(eos - lpSrcStr - 1) );
}

////////////////////////////////////////////////////////////////////////////
//
//  CopyString
//
//  复制字符串
//
////////////////////////////////////////////////////////////////////////////

int CopyString(
	int nSize,
	LPCVOID lpSrcStr,
	int cchSrc,
	LPVOID lpDestStr,
	int cchDest)
{
	if (cchSrc <= -1) {
		if (nSize == 1) {
			cchSrc = strlen(lpSrcStr) + 1;
		} else if (nSize == 2) {
			cchSrc = wcslen(lpSrcStr) + 1;
		} else if (nSize == 4) {
			cchSrc = lcslen(lpSrcStr) + 1;
		} else {
			SetLastError(ERROR_INVALID_PARAMETER);
			return 0;
		}
	}
	if (cchDest) {
		if (cchDest < cchSrc) {
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return 0;
		} else {
			memcpy(lpDestStr, lpSrcStr, cchSrc * nSize);
			return cchSrc;
		}
	} else {
		return cchSrc;
	}
	// should never reach here
}

////////////////////////////////////////////////////////////////////////////
//
//  SwapByte
//
//  倒转字符串的字节序
//
////////////////////////////////////////////////////////////////////////////

int SwapByte(
	int nSize,
	LPCVOID lpSrcStr,
	int cchSrc,
	LPVOID lpDestStr,
	int cchDest)
{
	int cch;

	if (cchSrc <= -1) {
		if (nSize == 2) {
			cchSrc = wcslen(lpSrcStr) + 1;
		} else if (nSize == 4) {
			cchSrc = lcslen(lpSrcStr) + 1;
		} else {
			SetLastError(ERROR_INVALID_PARAMETER);
			return 0;
		}
	}
	if (cchDest) {
		if (cchDest < cchSrc) {
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return 0;
		} else {
			if (nSize == 2) {
				LPCWSTR pSrc = lpSrcStr;
				LPWSTR pDest = lpDestStr;
				for (cch = 0; cch < cchSrc; cch++) {
					*pDest = SWAPBYTEORDER2(*pSrc);
					pSrc++;
					pDest++;
				}
			} else if (nSize == 4) {
				LPCLSTR pSrc = lpSrcStr;
				LPLSTR pDest = lpDestStr;
				for (cch = 0; cch < cchSrc; cch++) {
					*pDest = SWAPBYTEORDER4(*pSrc);
					pSrc++;
					pDest++;
				}
			} else {
				// should never reach here
				return 0;
			}
			return cchSrc;
		}
	} else {
		return cchSrc;
	}
	// should never reach here
}

////////////////////////////////////////////////////////////////////////////
//
//  UTF8ToScalar
//
//  将单个UTF8字符转为线性编码
//
////////////////////////////////////////////////////////////////////////////

/*inline*/ int UTF8ToScalar(
	LPCSTR lpSrcStr,
	int cchSrc,
	int* scalar)
{
	int nTB = 0;				// # trail bytes to follow
	LPCSTR pUTF8 = lpSrcStr;
	unsigned char I8;
	int utf32 = 0;
	int cchProcessed = 0;

	while (cchSrc--) {
		cchProcessed++;
		//  See if there are any trail bytes.
		I8 = *(unsigned char *)pUTF8;
		if (I8 <= UTF8_1_MAX) {
			//  Found ASCII.
			*scalar = I8;
			return cchProcessed;
		} else if (I8 < UTF8_1ST_OF_2) {
			//  Found a trail byte.
			//  Note : Ignore the trail byte if there was no lead byte.
			if (nTB != 0) {
				//  Decrement the trail byte counter.
				nTB--;
				//  Make room for the trail byte and add the trail byte value.
				utf32 <<= 6;
				utf32 |= LAST1_6_BIT(I8);
				if (nTB == 0) {
					//  End of sequence.  Advance the output counter.
					*scalar = utf32;
					return cchProcessed;
				}
			} else {
				//  Error - trail byte without lead byte.
				*scalar = -1;
				return cchProcessed;
			}
		} else {
			//  Found a lead byte.
			if (nTB > 0) {
				//  Error - previous sequence not finished.
				*scalar = -1;
				return cchProcessed;
			} else {
				//  Calculate the number of bytes to follow.
				//  Look for the first 0 from left to right.
				while (BIT7(I8) != 0) {
					I8 <<= 1;
					nTB++;
				}

				//  Store the value from the first byte and decrement
				//  the number of bytes to follow.
				utf32 = I8 >> nTB;
				nTB--;
			}
		}
		pUTF8++;
	}
	//  Error - incomplete sequence.
	*scalar = -1;
	return cchProcessed;
}

////////////////////////////////////////////////////////////////////////////
//
//  ScalarToUTF8
//
//  将单个线性编码转为UTF8字符
//
////////////////////////////////////////////////////////////////////////////

/*inline*/ int ScalarToUTF8(
	LPSTR lpDestStr,
	int cchDest,
	int scalar)
{
	if (scalar < 0) {
		//  Error
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	} else if (scalar <= UTF8_1_MAX) {
		//  Found ASCII.
		if (cchDest >= 1) {
			lpDestStr[0] = scalar;
			return 1;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else if (scalar <= UTF8_2_MAX) {
		//  Found 2 byte sequence if < 0x07ff (11 bits).
		if (cchDest >= 2) {
			lpDestStr[0] = UTF8_1ST_OF_2 | (scalar >> 6);
			lpDestStr[1] = UTF8_TRAIL | LAST1_6_BIT(scalar);
			return 2;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else if (scalar <= UTF8_3_MAX) {
		//  Found 3 byte sequence.
		if (cchDest >= 3) {
			lpDestStr[0] = UTF8_1ST_OF_3 | (scalar >> 12);
			lpDestStr[1] = UTF8_TRAIL | LAST2_6_BIT(scalar);
			lpDestStr[2] = UTF8_TRAIL | LAST1_6_BIT(scalar);
			return 3;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else if (scalar <= UTF8_4_MAX) {
		//  Found 4 byte sequence.
		if (cchDest >= 4) {
			lpDestStr[0] = UTF8_1ST_OF_4 | (scalar >> 18);
			lpDestStr[1] = UTF8_TRAIL | LAST3_6_BIT(scalar);
			lpDestStr[2] = UTF8_TRAIL | LAST2_6_BIT(scalar);
			lpDestStr[3] = UTF8_TRAIL | LAST1_6_BIT(scalar);
			return 4;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else if (scalar <= UTF8_5_MAX) {
		//  Found 5 byte sequence.
		if (cchDest >= 5) {
			lpDestStr[0] = UTF8_1ST_OF_5 | (scalar >> 24);
			lpDestStr[1] = UTF8_TRAIL | LAST4_6_BIT(scalar);
			lpDestStr[2] = UTF8_TRAIL | LAST3_6_BIT(scalar);
			lpDestStr[3] = UTF8_TRAIL | LAST2_6_BIT(scalar);
			lpDestStr[4] = UTF8_TRAIL | LAST1_6_BIT(scalar);
			return 5;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else {
		//  Found 6 byte sequence.
		if (cchDest >= 6) {
			lpDestStr[0] = UTF8_1ST_OF_6 | (scalar >> 30);
			lpDestStr[1] = UTF8_TRAIL | LAST5_6_BIT(scalar);
			lpDestStr[2] = UTF8_TRAIL | LAST4_6_BIT(scalar);
			lpDestStr[3] = UTF8_TRAIL | LAST3_6_BIT(scalar);
			lpDestStr[4] = UTF8_TRAIL | LAST2_6_BIT(scalar);
			lpDestStr[5] = UTF8_TRAIL | LAST1_6_BIT(scalar);
			return 6;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	}
	//  Should never reach here.
	return -2;
}

////////////////////////////////////////////////////////////////////////////
//
//  UTF16ToScalar
//
//  将单个UTF16字符转为线性编码
//
////////////////////////////////////////////////////////////////////////////

/*inline*/ int UTF16ToScalar(
	LPCWSTR lpSrcStr,
	int cchSrc,
	int* scalar,
	int BigEndian16)
{
	WCHAR c1, c2;
	if (cchSrc >= 1) {
		c1 = *lpSrcStr;
		if (BigEndian16) c1 = SWAPBYTEORDER2(c1);
		if ((c1 >= HIGH_SURROGATE_START) && (c1 <= HIGH_SURROGATE_END)) {
			if (cchSrc >= 2) {
				c2 = *(lpSrcStr + 1);
				if (BigEndian16) c2 = SWAPBYTEORDER2(c2);
				if ((c2 >= LOW_SURROGATE_START) && (c2 <= LOW_SURROGATE_END)) {
					*scalar = MAKEUTF32(c1, c2);
					return 2;
				} else {
					// Error - bad surrogate pair
					*scalar = -1;
					return 0;
				}
			} else {
				// Error - incomplete data
				*scalar = -1;
				return 0;
			}
		} else {
			*scalar = c1;
			return 1;
		}
	}
	//  Should never reach here.
	*scalar = -1;
	return 0;
}

////////////////////////////////////////////////////////////////////////////
//
//  ScalarToUTF16
//
//  将单个线性编码转为UTF16字符
//
////////////////////////////////////////////////////////////////////////////

/*inline*/ int ScalarToUTF16(
	LPWSTR lpDestStr,
	int cchDest,
	int scalar)
{
	if (scalar < 0 || scalar > UTF16_MAX) {
		//  Error
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	} else if (scalar <= UTF16_2_MAX) {
		//  Found BMP character.
		if (cchDest >= 1) {
			lpDestStr[0] = scalar;
			return 1;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else {
		//  Found 2 byte sequence.
		if (cchDest >= 2) {
			lpDestStr[0] = MAKEHIGHSURROGATE(scalar);
			lpDestStr[1] = MAKELOWSURROGATE(scalar);
			return 2;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	}
	//  Should never reach here.
	return -2;
}

////////////////////////////////////////////////////////////////////////////
//
//  UTFEBCDICToScalar
//
//  将单个UTF-EBCDIC字符转为线性编码
//
////////////////////////////////////////////////////////////////////////////

/*inline*/ int UTFEBCDICToScalar(
	LPCSTR lpSrcStr,
	int cchSrc,
	int* scalar)
{
	int nTB = 0;				// # trail bytes to follow
	LPCSTR pUTF8 = lpSrcStr;
	unsigned char I8;
	int utf32 = 0;
	int cchProcessed = 0;

	while (cchSrc--) {
		cchProcessed++;
		//  See if there are any trail bytes.
		I8 = EbcdicToI8[*(unsigned char *)pUTF8];
		//  See if there are any trail bytes.
		if (I8 <= UTFEBCDIC_1_MAX) {
			//  Found ASCII.
			*scalar = I8;
			return cchProcessed;
		} else if (I8 < UTFEBCDIC_1ST_OF_2) {
			//  Found a trail byte.
			//  Note : Ignore the trail byte if there was no lead byte.
			if (nTB != 0) {
				//  Decrement the trail byte counter.
				nTB--;
				//  Make room for the trail byte and add the trail byte value.
				utf32 <<= 5;
				utf32 |= LAST1_5_BIT(I8);
				if (nTB == 0) {
					//  End of sequence.  Advance the output counter.
					*scalar = utf32;
					return cchProcessed;
				}
			} else {
				//  Error - trail byte without lead byte.
				*scalar = -1;
				return cchProcessed;
			}
		} else {
			//  Found a lead byte.
			if (nTB > 0) {
				//  Error - previous sequence not finished.
				*scalar = -1;
				return cchProcessed;
			} else {
				//  Calculate the number of bytes to follow.
				//  Look for the first 0 from left to right.
				while (BIT7(I8) != 0) {
					I8 <<= 1;
					nTB++;
				}
				//  Store the value from the first byte and decrement
				//  the number of bytes to follow.
				utf32 = I8 >> nTB;
				nTB--;
			}
		}
		pUTF8++;
	}
	//  Error - incomplete sequence.
	*scalar = -1;
	return cchProcessed;
}

////////////////////////////////////////////////////////////////////////////
//
//  ScalarToUTFEBCDIC
//
//  将单个线性编码转为UTF-EBCDIC字符
//
////////////////////////////////////////////////////////////////////////////

/*inline*/ int ScalarToUTFEBCDIC(
	LPSTR lpDestStr,
	int cchDest,
	int scalar)
{
	if (scalar < 0) {
		//  Error
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	} else if (scalar <= UTFEBCDIC_1_MAX) {
		//  Found ASCII.
		if (cchDest >= 1) {
			lpDestStr[0] = I8ToEbcdic[ scalar ];
			return 1;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else if (scalar <= UTFEBCDIC_2_MAX) {
		//  Found 2 byte sequence if < 0x07ff (11 bits).
		if (cchDest >= 2) {
			lpDestStr[0] = I8ToEbcdic[ UTFEBCDIC_1ST_OF_2 | (scalar >> 5) ];
			lpDestStr[1] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST1_5_BIT(scalar) ];
			return 2;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else if (scalar <= UTFEBCDIC_3_MAX) {
		//  Found 3 byte sequence.
		if (cchDest >= 3) {
			lpDestStr[0] = I8ToEbcdic[ UTFEBCDIC_1ST_OF_3 | (scalar >> 10) ];
			lpDestStr[1] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST2_5_BIT(scalar) ];
			lpDestStr[2] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST1_5_BIT(scalar) ];
			return 3;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else if (scalar <= UTFEBCDIC_4_MAX) {
		//  Found 4 byte sequence.
		if (cchDest >= 4) {
			lpDestStr[0] = I8ToEbcdic[ UTFEBCDIC_1ST_OF_4 | (scalar >> 15) ];
			lpDestStr[1] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST3_5_BIT(scalar) ];
			lpDestStr[2] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST2_5_BIT(scalar) ];
			lpDestStr[3] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST1_5_BIT(scalar) ];
			return 4;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else if (scalar <= UTFEBCDIC_5_MAX) {
		//  Found 5 byte sequence.
		if (cchDest >= 5) {
			lpDestStr[0] = I8ToEbcdic[ UTFEBCDIC_1ST_OF_5 | (scalar >> 20) ];
			lpDestStr[1] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST4_5_BIT(scalar) ];
			lpDestStr[2] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST3_5_BIT(scalar) ];
			lpDestStr[3] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST2_5_BIT(scalar) ];
			lpDestStr[4] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST1_5_BIT(scalar) ];
			return 5;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else if (scalar <= UTFEBCDIC_6_MAX) {
		//  Found 6 byte sequence.
		if (cchDest >= 6) {
			lpDestStr[0] = I8ToEbcdic[ UTFEBCDIC_1ST_OF_6 | (scalar >> 25) ];
			lpDestStr[1] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST5_5_BIT(scalar) ];
			lpDestStr[2] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST4_5_BIT(scalar) ];
			lpDestStr[3] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST3_5_BIT(scalar) ];
			lpDestStr[4] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST2_5_BIT(scalar) ];
			lpDestStr[5] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST1_5_BIT(scalar) ];
			return 6;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else {
		//  Found 7 byte sequence.
		if (cchDest >= 7) {
			lpDestStr[0] = I8ToEbcdic[ UTFEBCDIC_1ST_OF_7 | (scalar >> 30) ];
			lpDestStr[1] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST6_5_BIT(scalar) ];
			lpDestStr[2] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST5_5_BIT(scalar) ];
			lpDestStr[3] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST4_5_BIT(scalar) ];
			lpDestStr[4] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST3_5_BIT(scalar) ];
			lpDestStr[5] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST2_5_BIT(scalar) ];
			lpDestStr[6] = I8ToEbcdic[ UTFEBCDIC_TRAIL | LAST1_5_BIT(scalar) ];
			return 7;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	}
	//  Should never reach here.
	return -2;
}

////////////////////////////////////////////////////////////////////////////
//
//  UTF1ToScalar
//
//  将单个UTF1字符转为线性编码
//
////////////////////////////////////////////////////////////////////////////

/*inline*/ int UTF1ToScalar(
	LPCSTR lpSrcStr,
	int cchSrc,
	int* scalar)
{
	LPCSTR pUTF8 = lpSrcStr;
	unsigned char I8;
	int utf32;
	int cchProcessed = 0;

	if (cchSrc > 0) {
		I8 = *(unsigned char *)pUTF8;
		if (I8 <= UTF1_1_MAX) {
			//  Found ASCII.
			*scalar = I8;
			return 1;
		} else if (I8 == UTF1_DUP_OF_2) {
			//  Found a 2 byte sequence.
			if (cchSrc >= 2) {
				utf32 = *(unsigned char *)++pUTF8;
				*scalar = utf32;
				return 2;
			} else {
				//  Error - incomplete sequence.
				*scalar = -1;
				return cchSrc;
			}
		} else if (I8 < UTF1_1ST_OF_3) {
			//  Found a 2 byte sequence.
			if (cchSrc >= 2) {
				utf32 = (I8 - UTF1_1ST_OF_2) * UTF1_MODULO;
				utf32 += UTF1_U[*(unsigned char *)++pUTF8];
				utf32 += UTF1_2_BASE;
				*scalar = utf32;
				return 2;
			} else {
				//  Error - incomplete sequence.
				*scalar = -1;
				return cchSrc;
			}
		} else if (I8 < UTF1_1ST_OF_5) {
			//  Found a 3 byte sequence.
			if (cchSrc >= 3) {
				utf32 = (I8 - UTF1_1ST_OF_3) * UTF1_MODULO_2;
				utf32 += UTF1_U[*(unsigned char *)++pUTF8] * UTF1_MODULO;
				utf32 += UTF1_U[*(unsigned char *)++pUTF8];
				utf32 += UTF1_3_BASE;
				*scalar = utf32;
				return 3;
			} else {
				//  Error - incomplete sequence.
				*scalar = -1;
				return cchSrc;
			}
		} else {
			//  Found a 5 byte sequence.
			if (cchSrc >= 5) {
				utf32 = (I8 - UTF1_1ST_OF_5) * UTF1_MODULO_4;
				utf32 += UTF1_U[*(unsigned char *)++pUTF8] * UTF1_MODULO_3;
				utf32 += UTF1_U[*(unsigned char *)++pUTF8] * UTF1_MODULO_2;
				utf32 += UTF1_U[*(unsigned char *)++pUTF8] * UTF1_MODULO;
				utf32 += UTF1_U[*(unsigned char *)++pUTF8];
				utf32 += UTF1_5_BASE;
				*scalar = utf32;
				return 5;
			} else {
				//  Error - incomplete sequence.
				*scalar = -1;
				return cchSrc;
			}
		}
	}
	//  Error - incomplete sequence.
	*scalar = -1;
	return cchProcessed;
}

////////////////////////////////////////////////////////////////////////////
//
//  ScalarToUTF1
//
//  将单个线性编码转为UTF1字符
//
////////////////////////////////////////////////////////////////////////////

/*inline*/ int ScalarToUTF1(
	LPSTR lpDestStr,
	int cchDest,
	int scalar)
{
	if (scalar < 0) {
		//  Error
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	} else if (scalar <= UTF1_1_MAX) {
		//  Found ASCII.
		if (cchDest >= 1) {
			lpDestStr[0] = scalar;
			return 1;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else if (scalar <= UTF1_2_MAX) {
		//  Found 2 byte sequence if < 0x07ff (11 bits).
		if (cchDest >= 2) {
			scalar -= UTF1_2_BASE;
			lpDestStr[0] = UTF1_1ST_OF_2 + scalar / UTF1_MODULO;
			lpDestStr[1] = UTF1_T[ scalar % UTF1_MODULO ];
			return 2;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else if (scalar <= UTF1_3_MAX) {
		//  Found 3 byte sequence.
		if (cchDest >= 3) {
			scalar -= UTF1_3_BASE;
			lpDestStr[0] = UTF1_1ST_OF_3 + scalar / UTF1_MODULO_2;
			lpDestStr[1] = UTF1_T[ scalar / UTF1_MODULO % UTF1_MODULO ];
			lpDestStr[2] = UTF1_T[ scalar % UTF1_MODULO ];
			return 3;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	} else {
		//  Found 5 byte sequence.
		if (cchDest >= 5) {
			scalar -= UTF1_5_BASE;
			lpDestStr[0] = UTF1_1ST_OF_5 + scalar / UTF1_MODULO_4;
			lpDestStr[1] = UTF1_T[ scalar / UTF1_MODULO_3 % UTF1_MODULO ];
			lpDestStr[2] = UTF1_T[ scalar / UTF1_MODULO_2 % UTF1_MODULO ];
			lpDestStr[3] = UTF1_T[ scalar / UTF1_MODULO % UTF1_MODULO ];
			lpDestStr[4] = UTF1_T[ scalar % UTF1_MODULO ];
			return 5;
		} else {
			//  Error - buffer too small.
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return -1;
		}
	}
	//  Should never reach here.
	return -2;
}


////////////////////////////////////////////////////////////////////////////
//
//  导出函数
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//  REGISTER_FUNC_I1_TO_I1
//
//  将8位字符串转为另一个8位字符串
//
////////////////////////////////////////////////////////////////////////////

#define REGISTER_FUNC_I1_TO_I1(FuncName, I1ToScalar, ScalarToI1) \
int FuncName( \
	LPCSTR lpSrcStr, \
	int cchSrc, \
	LPSTR lpDestStr, \
	int cchDest) \
{ \
	int cchU8 = 0; \
	LPCSTR pUTF8 = lpSrcStr; \
	int utf32; \
	int ret, ret2; \
	char buf[8]; \
\
	if (cchSrc <= -1) { \
		cchSrc = strlen(lpSrcStr) + 1; \
	} \
\
	while ((cchSrc > 0) && ((cchDest == 0) || (cchU8 < cchDest))) { \
		ret = I1ToScalar(pUTF8, cchSrc, &utf32); \
		if (utf32 < 0) { \
			/* Error byte sequence */ \
		} else { \
			if (cchDest) { \
				ret2 = ScalarToI1(&lpDestStr[cchU8], cchDest - cchU8, utf32); \
			} else { \
				ret2 = ScalarToI1(buf, ARRAYSIZE(buf), utf32); \
			} \
			if (ret2 < 0) { \
				/* Error - insufficient buffer */ \
				break; \
			} \
			cchU8 += ret2; \
		} \
		pUTF8 += ret; \
		cchSrc -= ret; \
	} \
\
	if (cchDest && (cchSrc > 0)) { \
		SetLastError(ERROR_INSUFFICIENT_BUFFER); \
		return 0; \
	} \
\
	return cchU8; \
}

REGISTER_FUNC_I1_TO_I1(UTF8ToUTFEBCDIC, UTF8ToScalar, ScalarToUTFEBCDIC)
REGISTER_FUNC_I1_TO_I1(UTF8ToUTF1, UTF8ToScalar, ScalarToUTF1)
REGISTER_FUNC_I1_TO_I1(UTFEBCDICToUTF8, UTFEBCDICToScalar, ScalarToUTF8)
REGISTER_FUNC_I1_TO_I1(UTFEBCDICToUTF1, UTFEBCDICToScalar, ScalarToUTF1)
REGISTER_FUNC_I1_TO_I1(UTF1ToUTF8, UTF1ToScalar, ScalarToUTF8)
REGISTER_FUNC_I1_TO_I1(UTF1ToUTFEBCDIC, UTF1ToScalar, ScalarToUTFEBCDIC)

////////////////////////////////////////////////////////////////////////////
//
//  REGISTER_FUNC_I1_TO_I2
//
//  将8位字符串转为16位字符串
//
////////////////////////////////////////////////////////////////////////////

#define REGISTER_FUNC_I1_TO_I2(FuncName, I1ToScalar, ScalarToI2) \
int FuncName( \
	LPCSTR lpSrcStr, \
	int cchSrc, \
	LPWSTR lpDestStr, \
	int cchDest, \
	int BigEndian16) \
{ \
	int cchWC = 0; \
	LPCSTR pUTF8 = lpSrcStr; \
	int utf32; \
	int ret, ret2; \
	WCHAR buf[4]; \
\
	if (cchSrc <= -1) { \
		cchSrc = strlen(lpSrcStr) + 1; \
	} \
\
	while ((cchSrc > 0) && ((cchDest == 0) || (cchWC < cchDest))) { \
		ret = I1ToScalar(pUTF8, cchSrc, &utf32); \
		if (utf32 < 0) { \
			/* Error byte sequence */ \
		} else { \
			if (cchDest) { \
				ret2 = ScalarToI2(&lpDestStr[cchWC], cchDest - cchWC, utf32); \
			} else { \
				ret2 = ScalarToI2(buf, ARRAYSIZE(buf), utf32); \
			} \
			if (ret2 < 0) { \
				/* Error - insufficient buffer */ \
				break; \
			} \
			cchWC += ret2; \
		} \
		pUTF8 += ret; \
		cchSrc -= ret; \
	} \
\
	if (cchDest && (cchSrc > 0)) { \
		SetLastError(ERROR_INSUFFICIENT_BUFFER); \
		return 0; \
	} \
\
	if (cchDest && BigEndian16) { \
		int cch = 0; \
		while (cch++ < cchWC) \
			lpDestStr[cch] = SWAPBYTEORDER2(lpDestStr[cch]); \
	} \
\
	return cchWC; \
}

REGISTER_FUNC_I1_TO_I2(UTF8ToUTF16, UTF8ToScalar, ScalarToUTF16)
REGISTER_FUNC_I1_TO_I2(UTFEBCDICToUTF16, UTFEBCDICToScalar, ScalarToUTF16)
REGISTER_FUNC_I1_TO_I2(UTF1ToUTF16, UTF1ToScalar, ScalarToUTF16)


////////////////////////////////////////////////////////////////////////////
//
//  REGISTER_FUNC_I2_TO_I1
//
//  将16位字符串转为8位字符串
//
////////////////////////////////////////////////////////////////////////////

#define REGISTER_FUNC_I2_TO_I1(FuncName, I2ToScalar, ScalarToI1) \
int FuncName( \
	LPCWSTR lpSrcStr, \
	int cchSrc, \
	LPSTR lpDestStr, \
	int cchDest, \
	int BigEndian16) \
{ \
	LPCWSTR lpWC = lpSrcStr; \
	int utf32; \
	int cchU8 = 0; \
	int ret, ret2; \
	char buf[8]; \
\
	if (cchSrc <= -1) { \
		cchSrc = wcslen(lpSrcStr) + 1; \
	} \
\
	while ((cchSrc > 0) && ((cchDest == 0) || (cchU8 < cchDest))) { \
		ret = I2ToScalar(lpWC, cchSrc, &utf32, BigEndian16); \
		if (utf32 < 0) { \
			/* Error byte sequence */ \
		} else { \
			if (cchDest) { \
				ret2 = ScalarToI1(&lpDestStr[cchU8], cchDest - cchU8, utf32); \
			} else { \
				ret2 = ScalarToI1(buf, ARRAYSIZE(buf), utf32); \
			} \
			if (ret2 < 0) { \
				/* Error - insufficient buffer */ \
				break; \
			} \
			cchU8 += ret2; \
		} \
		lpWC += ret; \
		cchSrc -= ret; \
	} \
\
	if (cchDest && (cchSrc > 0)) { \
		SetLastError(ERROR_INSUFFICIENT_BUFFER); \
		return 0; \
	} \
\
	return cchU8; \
}

REGISTER_FUNC_I2_TO_I1(UTF16ToUTF8, UTF16ToScalar, ScalarToUTF8)
REGISTER_FUNC_I2_TO_I1(UTF16ToUTFEBCDIC, UTF16ToScalar, ScalarToUTFEBCDIC)
REGISTER_FUNC_I2_TO_I1(UTF16ToUTF1, UTF16ToScalar, ScalarToUTF1)

////////////////////////////////////////////////////////////////////////////
//
//  REGISTER_FUNC_I1_TO_I4
//
//  将8位字符串转为UTF32格式
//
////////////////////////////////////////////////////////////////////////////

#define REGISTER_FUNC_I1_TO_I4(FuncName, I1ToScalar) \
int FuncName( \
	LPCSTR lpSrcStr, \
	int cchSrc, \
	LPLSTR lpDestStr, \
	int cchDest, \
	int BigEndian32) \
{ \
	int cchLC = 0; \
	int ret = 0; \
	LPCSTR pUTF8 = lpSrcStr; \
	int utf32; \
\
	if (cchSrc <= -1) { \
		cchSrc = strlen(lpSrcStr) + 1; \
	} \
\
	while ((cchSrc > 0) && ((cchDest == 0) || (cchLC < cchDest))) { \
		ret = I1ToScalar(pUTF8, cchSrc, &utf32); \
		if (utf32 < 0) { \
			/* Error byte sequence */ \
		} else { \
			if (cchDest) { \
				lpDestStr[cchLC] = utf32; \
			} \
			cchLC++; \
		} \
		pUTF8 += ret; \
		cchSrc -= ret; \
	} \
\
	if (cchDest && (cchSrc > 0)) { \
		SetLastError(ERROR_INSUFFICIENT_BUFFER); \
		return 0; \
	} \
\
	if (cchDest && BigEndian32) { \
		int cch = 0; \
		while (cch++ < cchLC) \
			lpDestStr[cch] = SWAPBYTEORDER4(lpDestStr[cch]); \
	} \
\
	return cchLC; \
}

REGISTER_FUNC_I1_TO_I4(UTF8ToUTF32, UTF8ToScalar)
REGISTER_FUNC_I1_TO_I4(UTFEBCDICToUTF32, UTFEBCDICToScalar)
REGISTER_FUNC_I1_TO_I4(UTF1ToUTF32, UTF1ToScalar)

////////////////////////////////////////////////////////////////////////////
//
//  REGISTER_FUNC_I4_TO_I1
//
//  将UTF32字符串转为8位字符串
//
////////////////////////////////////////////////////////////////////////////

#define REGISTER_FUNC_I4_TO_I1(FuncName, ScalarToI1) \
int FuncName( \
	LPCLSTR lpSrcStr, \
	int cchSrc, \
	LPSTR lpDestStr, \
	int cchDest, \
	int BigEndian32) \
{ \
	LPCLSTR lpLC = lpSrcStr; \
	int utf32; \
	int cchU8 = 0; \
	int ret = 0; \
	char buf[8]; \
\
	if (cchSrc <= -1) { \
		cchSrc = lcslen(lpSrcStr) + 1; \
	} \
\
	while ((cchSrc--) && ((cchDest == 0) || (cchU8 < cchDest))) { \
		utf32 = *lpLC; \
		if (BigEndian32) utf32 = SWAPBYTEORDER4(utf32); \
		if (cchDest) { \
			ret = ScalarToI1(&lpDestStr[cchU8], cchDest - cchU8, utf32); \
		} else { \
			ret = ScalarToI1(buf, ARRAYSIZE(buf), utf32); \
		} \
		if (ret < 0) { \
			/* Error - insufficient buffer */ \
			break; \
		} \
		cchU8 += ret; \
		lpLC++; \
	} \
\
	if (cchDest && (cchSrc >= 0)) { \
		SetLastError(ERROR_INSUFFICIENT_BUFFER); \
		return 0; \
	} \
\
	return cchU8; \
}

REGISTER_FUNC_I4_TO_I1(UTF32ToUTF8, ScalarToUTF8)
REGISTER_FUNC_I4_TO_I1(UTF32ToUTFEBCDIC, ScalarToUTFEBCDIC)
REGISTER_FUNC_I4_TO_I1(UTF32ToUTF1, ScalarToUTF1)

////////////////////////////////////////////////////////////////////////////
//
//  UTF32ToUTF16
//
//  将UTF32字符串转为UTF16格式
//
////////////////////////////////////////////////////////////////////////////

int UTF32ToUTF16(
	LPCLSTR lpSrcStr,
	int cchSrc,
	LPWSTR lpDestStr,
	int cchDest,
	int BigEndian32,
	int BigEndian16)
{
	int cchWC = 0;				 // # of Unicode code points generated
	LPCLSTR pUTF32 = lpSrcStr;
	int ret = 0;
	int utf32;
	WCHAR buf[4];

	if (cchSrc <= -1) {
		cchSrc = lcslen(lpSrcStr) + 1;
	}

	while ((cchSrc--) && ((cchDest == 0) || (cchWC < cchDest))) {
		utf32 = *pUTF32;
		if (BigEndian32) utf32 = SWAPBYTEORDER4(utf32);
		if (cchDest) {
			ret = ScalarToUTF16(&lpDestStr[cchWC], cchDest - cchWC, utf32);
		} else {
			ret = ScalarToUTF16(buf, ARRAYSIZE(buf), utf32);
		}
		if (ret < 0) {
			// Error - insufficient buffer
			break;
		}
		cchWC += ret;
		pUTF32++;
	}

	//  Make sure the destination buffer was large enough.
	if (cchDest && (cchSrc >= 0)) {
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return (0);
	}

	if (cchDest && BigEndian16) {
		int cch = 0;
		while (cch++ < cchWC)
			lpDestStr[cch] = SWAPBYTEORDER2(lpDestStr[cch]);
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
////////////////////////////////////////////////////////////////////////////

int UTF16ToUTF32(
	LPCWSTR lpSrcStr,
	int cchSrc,
	LPLSTR lpDestStr,
	int cchDest,
	int BigEndian16,
	int BigEndian32)
{
	LPCWSTR lpWC = lpSrcStr;
	int utf32;
	int cchU32 = 0;				// # of UTF32 chars generated
	int ret = 0;

	if (cchSrc <= -1) {
		cchSrc = wcslen(lpSrcStr) + 1;
	}

	while ((cchSrc > 0) && ((cchDest == 0) || (cchU32 < cchDest))) {
		ret = UTF16ToScalar(lpWC, cchSrc, &utf32, BigEndian16);
		if (utf32 < 0) {
			// Error byte sequence
		} else {
			if (cchDest) {
				lpDestStr[cchU32] = utf32;
			}
			cchU32++;
		}
		lpWC += ret;
		cchSrc -= ret;
	}

	//  Make sure the destination buffer was large enough.
	if (cchDest && (cchSrc > 0)) {
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return (0);
	}

	if (cchDest && BigEndian32) {
		int cch = 0;
		while (cch++ < cchU32)
			lpDestStr[cch] = SWAPBYTEORDER4(lpDestStr[cch]);
	}

	//  Return the number of UTF-32 characters written.
	return (cchU32);
}

////////////////////////////////////////////////////////////////////////////
//
//  CESU8ToUTF16
//
//  将CESU8字符串转为UTF16格式
//
////////////////////////////////////////////////////////////////////////////

int CESU8ToUTF16(
	LPCSTR lpSrcStr,
	int cchSrc,
	LPWSTR lpDestStr,
	int cchDest,
	int BigEndian16)
{
	int cchWC = 0;				 // # of Unicode code points generated
	LPCSTR pUTF8 = lpSrcStr;
	int utf32;
	int ret;

	if (cchSrc <= -1) {
		cchSrc = strlen(lpSrcStr) + 1;
	}

	while ((cchSrc > 0) && ((cchDest == 0) || (cchWC < cchDest))) {
		ret = UTF8ToScalar(pUTF8, cchSrc, &utf32);
		if (utf32 < 0 || utf32 >= 0x10000) {
			// Error byte sequence
		} else {
			if (cchDest) {
				lpDestStr[cchWC] = (WCHAR)utf32;
			}
			cchWC++;
		}
		pUTF8 += ret;
		cchSrc -= ret;
	}

	//  Make sure the destination buffer was large enough.
	if (cchDest && (cchSrc > 0)) {
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return (0);
	}

	if (cchDest && BigEndian16) {
		int cch = 0;
		while (cch++ < cchWC)
			lpDestStr[cch] = SWAPBYTEORDER2(lpDestStr[cch]);
	}

	//  Return the number of Unicode characters written.
	return (cchWC);
}


////////////////////////////////////////////////////////////////////////////
//
//  UTF16ToCESU8
//
//  将UTF16字符串转为CESU8格式
//
////////////////////////////////////////////////////////////////////////////

int UTF16ToCESU8(
	LPCWSTR lpSrcStr,
	int cchSrc,
	LPSTR lpDestStr,
	int cchDest,
	int BigEndian16)
{
	LPCWSTR lpWC = lpSrcStr;
	WCHAR utf16;
	int cchU8 = 0;				// # of UTF8 chars generated
	int ret2;
	char buf[8];

	if (cchSrc <= -1) {
		cchSrc = wcslen(lpSrcStr) + 1;
	}

	while ((cchSrc > 0) && ((cchDest == 0) || (cchU8 < cchDest))) {
		utf16 = *lpWC;
		if (BigEndian16) utf16 = SWAPBYTEORDER2(utf16);
		if (cchDest) {
			ret2 = ScalarToUTF8(&lpDestStr[cchU8], cchDest - cchU8, utf16);
		} else {
			ret2 = ScalarToUTF8(buf, ARRAYSIZE(buf), utf16);
		}
		if (ret2 < 0) {
			// Error - insufficient buffer
			break;
		}
		cchU8 += ret2;
		lpWC++;
		cchSrc--;
	}

	//  Make sure the destination buffer was large enough.
	if (cchDest && (cchSrc > 0)) {
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return (0);
	}

	//  Return the number of UTF-8 characters written.
	return (cchU8);
}


////////////////////////////////////////////////////////////////////////////
//
//  API包装函数
//
////////////////////////////////////////////////////////////////////////////

int API UTF16BEToUTF16(LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest) {
	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return (0);
	}
	return SwapByte(2, lpSrcStr, cchSrc, lpDestStr, cchDest);
}

int API UTF32BEToUTF32(LPCLSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest) {
	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return (0);
	}
	return SwapByte(4, lpSrcStr, cchSrc, lpDestStr, cchDest);
}

int API UTF8ToUTF16LE(LPCSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest) {
	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPSTR)lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	return UTF8ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
}

int API UTF8ToUTF32LE(LPCSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest) {
	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPSTR)lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	return UTF8ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
}

int API UTF16LEToUTF8(LPCWSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest) {
	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPWSTR)lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	return UTF16ToUTF8(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
}

int API UTF16LEToUTF32LE(LPCWSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest) {
	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPWSTR)lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	return UTF16ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE, FALSE);
}

int API UTF16LEToUTF32BE(LPCWSTR lpSrcStr, int cchSrc, LPLSTR lpDestStr, int cchDest) {
	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPWSTR)lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	return UTF16ToUTF32(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE, TRUE);
}

int API UTF32LEToUTF8(LPCLSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest) {
	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPLSTR)lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	return UTF32ToUTF8(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE);
}

int API UTF32LEToUTF16LE(LPCLSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest) {
	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPLSTR)lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	return UTF32ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, FALSE, FALSE);
}

int API UTF32BEToUTF16LE(LPCLSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest) {
	if ( (cchSrc == 0) || (cchDest < 0) || (lpSrcStr == NULL) ||
		((cchDest != 0) && ((lpDestStr == NULL) || (lpSrcStr == (LPLSTR)lpDestStr))) )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	return UTF32ToUTF16(lpSrcStr, cchSrc, lpDestStr, cchDest, TRUE, FALSE);
}

////////////////////////////////////////////////////////////////////////////
//
//  GB18030ToUTF16
//
//  将GB18030字符串转为UTF16格式
//
////////////////////////////////////////////////////////////////////////////

int API GB18030ToUTF16(
	LPCSTR lpSrcStr,
	int cchSrc,
	LPWSTR lpDestStr,
	int cchDest)
{
	if(IsValidCodePage(CP_GB18030))
	{
		return MultiByteToWideChar(CP_GB18030, 0, lpSrcStr, cchSrc, lpDestStr, cchDest);
	}
	else
	{
		return NlsDllCodePageTranslation(CP_GB18030, NLS_CP_MBTOWC, (LPSTR)lpSrcStr, cchSrc, lpDestStr, cchDest, NULL);
	}
}

////////////////////////////////////////////////////////////////////////////
//
//  UTF16ToGB18030
//
//  将UTF16字符串转为GB18030格式
//
////////////////////////////////////////////////////////////////////////////

int API UTF16ToGB18030(
	LPCWSTR lpSrcStr,
	int cchSrc,
	LPSTR lpDestStr,
	int cchDest)
{
	if(IsValidCodePage(CP_GB18030))
	{
		return WideCharToMultiByte(CP_GB18030, 0, lpSrcStr, cchSrc, lpDestStr, cchDest, NULL, NULL);
	}
	else
	{
		return NlsDllCodePageTranslation(CP_GB18030, NLS_CP_WCTOMB, lpDestStr, cchDest, (LPWSTR)lpSrcStr, cchSrc, NULL);
	}
}
