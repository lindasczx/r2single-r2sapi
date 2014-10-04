#include "r2sapi.h"
#include <stdint.h>

#define BITCOUNT 64

__int64 API I8Add(__int64 a, __int64 b);
__int64 API I8Sub(__int64 a, __int64 b);
__int64 API I8Mul(__int64 a, __int64 b);
__int64 API I8Div(__int64 a, __int64 b);
__int64 API I8Mod(__int64 a, __int64 b);
__int64 API I8And(__int64 a, __int64 b);
__int64 API I8Or(__int64 a, __int64 b);
__int64 API I8Xor(__int64 a, __int64 b);
__int64 API I8Not(__int64 a);
__int64 API I8Shl(__int64 a, int b);
__int64 API I8Shr(__int64 a, int b);
__int64 API I8Rol(__int64 a, int b);
__int64 API I8Ror(__int64 a, int b);
int API I8Cmp(__int64 a, __int64 b);
unsigned __int64 API UI8Mul(unsigned __int64 a, unsigned __int64 b);
unsigned __int64 API UI8Div(unsigned __int64 a, unsigned __int64 b);
unsigned __int64 API UI8Mod(unsigned __int64 a, unsigned __int64 b);
unsigned __int64 API UI8Shl(unsigned __int64 a, int b);
unsigned __int64 API UI8Shr(unsigned __int64 a, int b);
int API UI8Cmp(unsigned __int64 a, unsigned __int64 b);

__int64 API I8Add(__int64 a, __int64 b) {
	return a + b;
}

__int64 API I8Sub(__int64 a, __int64 b) {
	return a - b;
}

__int64 API I8Mul(__int64 a, __int64 b) {
	return a * b;
}

__int64 API I8Div(__int64 a, __int64 b) {
	return a / b;
}

__int64 API I8Mod(__int64 a, __int64 b) {
	return a % b;
}

__int64 API I8And(__int64 a, __int64 b) {
	return a & b;
}

__int64 API I8Or(__int64 a, __int64 b) {
	return a | b;
}

__int64 API I8Xor(__int64 a, __int64 b) {
	return a ^ b;
}

__int64 API I8Not(__int64 a) {
	return ~a;
}

__int64 API I8Shl(__int64 a, int b) {
	if (b < 0)
		return I8Shr(a, -b);
	else if (b >= BITCOUNT)
		return 0;
	else
		return a << b;
}

__int64 API I8Shr(__int64 a, int b) {
	if (b < 0)
		return I8Shl(a, -b);
	else if (b >= BITCOUNT)
		if (a < 0)
			return -1;
		else
			return 0;
	else
		return a >> b;
}

__int64 API I8Rol(__int64 p, int b) {
	unsigned __int64 a = (unsigned __int64)p;
	b &= BITCOUNT - 1;
	return a << b | a >> BITCOUNT - b;
}

__int64 API I8Ror(__int64 p, int b) {
	unsigned __int64 a = (unsigned __int64)p;
	b &= BITCOUNT - 1;
	return a >> b | a << BITCOUNT - b;
}

int API I8Cmp(__int64 a, __int64 b) {
	if (a > b)
		return 1;
	else if (a == b)
		return 0;
	else
		return -1;
}

unsigned __int64 API UI8Mul(unsigned __int64 a, unsigned __int64 b) {
	return a * b;
}

unsigned __int64 API UI8Div(unsigned __int64 a, unsigned __int64 b) {
	return a / b;
}

unsigned __int64 API UI8Mod(unsigned __int64 a, unsigned __int64 b) {
	return a % b;
}

unsigned __int64 API UI8Shl(unsigned __int64 a, int b) {
	if (b < 0)
		return UI8Shr(a, -b);
	else if (b >= BITCOUNT)
		return 0;
	else
		return a << b;
}

unsigned __int64 API UI8Shr(unsigned __int64 a, int b) {
	if (b < 0)
		return UI8Shl(a, -b);
	else if (b >= BITCOUNT)
		return 0;
	else
		return a >> b;
}

int API UI8Cmp(unsigned __int64 a, unsigned __int64 b) {
	if (a > b)
		return 1;
	else if (a == b)
		return 0;
	else
		return -1;
}

__int64 API I8AddR(__int64 *a, __int64 *b) { return I8Add(*a, *b); }
__int64 API I8SubR(__int64 *a, __int64 *b) { return I8Sub(*a, *b); }
__int64 API I8MulR(__int64 *a, __int64 *b) { return I8Mul(*a, *b); }
__int64 API I8DivR(__int64 *a, __int64 *b) { return I8Div(*a, *b); }
__int64 API I8ModR(__int64 *a, __int64 *b) { return I8Mod(*a, *b); }
__int64 API I8AndR(__int64 *a, __int64 *b) { return I8And(*a, *b); }
__int64 API I8OrR(__int64 *a, __int64 *b) { return I8Or(*a, *b); }
__int64 API I8XorR(__int64 *a, __int64 *b) { return I8Xor(*a, *b); }
__int64 API I8NotR(__int64 *a) { return I8Not(*a); }
__int64 API I8ShlR(__int64 *a, int b) { return I8Shl(*a, b); }
__int64 API I8ShrR(__int64 *a, int b) { return I8Shr(*a, b); }
__int64 API I8RolR(__int64 *a, int b) { return I8Rol(*a, b); }
__int64 API I8RorR(__int64 *a, int b) { return I8Ror(*a, b); }
int API I8CmpR(__int64 *a, __int64 *b) { return I8Cmp(*a, *b); }
unsigned __int64 API UI8MulR(unsigned __int64 *a, unsigned __int64 *b) { return UI8Mul(*a, *b); }
unsigned __int64 API UI8DivR(unsigned __int64 *a, unsigned __int64 *b) { return UI8Div(*a, *b); }
unsigned __int64 API UI8ModR(unsigned __int64 *a, unsigned __int64 *b) { return UI8Mod(*a, *b); }
unsigned __int64 API UI8ShlR(unsigned __int64 *a, int b) { return UI8Shl(*a, b); }
unsigned __int64 API UI8ShrR(unsigned __int64 *a, int b) { return UI8Shr(*a, b); }
int API UI8CmpR(unsigned __int64 *a, unsigned __int64 *b) { return UI8Cmp(*a, *b); }

