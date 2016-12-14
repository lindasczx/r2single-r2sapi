#include "r2sapi.h"
#include <stdint.h>

#define BITCOUNT 32

int32_t API I4Add(int32_t a, int32_t b);	// 为了允许溢出
int32_t API I4Sub(int32_t a, int32_t b);	// 为了允许溢出
int32_t API I4Shl(int32_t a, int b);
int32_t API I4Shr(int32_t a, int b);
int32_t API I4Rol(int32_t a, int b);
int32_t API I4Ror(int32_t a, int b);
uint32_t API UI4Mul(uint32_t a, uint32_t b);
uint32_t API UI4Div(uint32_t a, uint32_t b);
uint32_t API UI4Mod(uint32_t a, uint32_t b);
uint32_t API UI4Shl(uint32_t a, int b);
uint32_t API UI4Shr(uint32_t a, int b);
int API UI4Cmp(uint32_t a, uint32_t b);

int32_t API I4Add(int32_t a, int32_t b) {
	return a + b;
}

int32_t API I4Sub(int32_t a, int32_t b) {
	return a - b;
}

int32_t API I4Shl(int32_t a, int b) {
	if (b < 0)
		return I4Shr(a, -b);
	else if (b >= BITCOUNT)
		return 0;
	else
		return a << b;
}

int32_t API I4Shr(int32_t a, int b) {
	if (b < 0)
		return I4Shl(a, -b);
	else if (b >= BITCOUNT)
		if (a < 0)
			return -1;
		else
			return 0;
	else
		return a >> b;
}

int32_t API I4Rol(int32_t p, int b) {
	uint32_t a = (uint32_t)p;
	b &= BITCOUNT - 1;
	return a << b | a >> BITCOUNT - b;
}

int32_t API I4Ror(int32_t p, int b) {
	uint32_t a = (uint32_t)p;
	b &= BITCOUNT - 1;
	return a >> b | a << BITCOUNT - b;
}

uint32_t API UI4Mul(uint32_t a, uint32_t b) {
	return a * b;
}

uint32_t API UI4Div(uint32_t a, uint32_t b) {
	return a / b;
}

uint32_t API UI4Mod(uint32_t a, uint32_t b) {
	return a % b;
}

uint32_t API UI4Shl(uint32_t a, int b) {
	if (b < 0)
		return UI4Shr(a, -b);
	else if (b >= BITCOUNT)
		return 0;
	else
		return a << b;
}

uint32_t API UI4Shr(uint32_t a, int b) {
	if (b < 0)
		return UI4Shl(a, -b);
	else if (b >= BITCOUNT)
		return 0;
	else
		return a >> b;
}

int API UI4Cmp(uint32_t a, uint32_t b) {
	if (a > b)
		return 1;
	else if (a == b)
		return 0;
	else
		return -1;
}

