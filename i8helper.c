#include "r2sapi.h"
#include <stdint.h>

#define BITCOUNT 64

int64_t API I8Add(int64_t a, int64_t b);
int64_t API I8Sub(int64_t a, int64_t b);
int64_t API I8Mul(int64_t a, int64_t b);
int64_t API I8Div(int64_t a, int64_t b);
int64_t API I8Mod(int64_t a, int64_t b);
int64_t API I8And(int64_t a, int64_t b);
int64_t API I8Or(int64_t a, int64_t b);
int64_t API I8Xor(int64_t a, int64_t b);
int64_t API I8Not(int64_t a);
int64_t API I8Shl(int64_t a, int b);
int64_t API I8Shr(int64_t a, int b);
int64_t API I8Rol(int64_t a, int b);
int64_t API I8Ror(int64_t a, int b);
int API I8Cmp(int64_t a, int64_t b);
uint64_t API UI8Mul(uint64_t a, uint64_t b);
uint64_t API UI8Div(uint64_t a, uint64_t b);
uint64_t API UI8Mod(uint64_t a, uint64_t b);
uint64_t API UI8Shl(uint64_t a, int b);
uint64_t API UI8Shr(uint64_t a, int b);
int API UI8Cmp(uint64_t a, uint64_t b);

int64_t API I8Add(int64_t a, int64_t b) {
	return a + b;
}

int64_t API I8Sub(int64_t a, int64_t b) {
	return a - b;
}

int64_t API I8Mul(int64_t a, int64_t b) {
	return a * b;
}

int64_t API I8Div(int64_t a, int64_t b) {
	return a / b;
}

int64_t API I8Mod(int64_t a, int64_t b) {
	return a % b;
}

int64_t API I8And(int64_t a, int64_t b) {
	return a & b;
}

int64_t API I8Or(int64_t a, int64_t b) {
	return a | b;
}

int64_t API I8Xor(int64_t a, int64_t b) {
	return a ^ b;
}

int64_t API I8Not(int64_t a) {
	return ~a;
}

int64_t API I8Shl(int64_t a, int b) {
	if (b < 0)
		return I8Shr(a, -b);
	else if (b >= BITCOUNT)
		return 0;
	else
		return a << b;
}

int64_t API I8Shr(int64_t a, int b) {
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

int64_t API I8Rol(int64_t p, int b) {
	uint64_t a = (uint64_t)p;
	b &= BITCOUNT - 1;
	return a << b | a >> BITCOUNT - b;
}

int64_t API I8Ror(int64_t p, int b) {
	uint64_t a = (uint64_t)p;
	b &= BITCOUNT - 1;
	return a >> b | a << BITCOUNT - b;
}

int API I8Cmp(int64_t a, int64_t b) {
	if (a > b)
		return 1;
	else if (a == b)
		return 0;
	else
		return -1;
}

uint64_t API UI8Mul(uint64_t a, uint64_t b) {
	return a * b;
}

uint64_t API UI8Div(uint64_t a, uint64_t b) {
	return a / b;
}

uint64_t API UI8Mod(uint64_t a, uint64_t b) {
	return a % b;
}

uint64_t API UI8Shl(uint64_t a, int b) {
	if (b < 0)
		return UI8Shr(a, -b);
	else if (b >= BITCOUNT)
		return 0;
	else
		return a << b;
}

uint64_t API UI8Shr(uint64_t a, int b) {
	if (b < 0)
		return UI8Shl(a, -b);
	else if (b >= BITCOUNT)
		return 0;
	else
		return a >> b;
}

int API UI8Cmp(uint64_t a, uint64_t b) {
	if (a > b)
		return 1;
	else if (a == b)
		return 0;
	else
		return -1;
}

int64_t API I8AddR(int64_t *a, int64_t *b) { return I8Add(*a, *b); }
int64_t API I8SubR(int64_t *a, int64_t *b) { return I8Sub(*a, *b); }
int64_t API I8MulR(int64_t *a, int64_t *b) { return I8Mul(*a, *b); }
int64_t API I8DivR(int64_t *a, int64_t *b) { return I8Div(*a, *b); }
int64_t API I8ModR(int64_t *a, int64_t *b) { return I8Mod(*a, *b); }
int64_t API I8AndR(int64_t *a, int64_t *b) { return I8And(*a, *b); }
int64_t API I8OrR(int64_t *a, int64_t *b) { return I8Or(*a, *b); }
int64_t API I8XorR(int64_t *a, int64_t *b) { return I8Xor(*a, *b); }
int64_t API I8NotR(int64_t *a) { return I8Not(*a); }
int64_t API I8ShlR(int64_t *a, int b) { return I8Shl(*a, b); }
int64_t API I8ShrR(int64_t *a, int b) { return I8Shr(*a, b); }
int64_t API I8RolR(int64_t *a, int b) { return I8Rol(*a, b); }
int64_t API I8RorR(int64_t *a, int b) { return I8Ror(*a, b); }
int API I8CmpR(int64_t *a, int64_t *b) { return I8Cmp(*a, *b); }
uint64_t API UI8MulR(uint64_t *a, uint64_t *b) { return UI8Mul(*a, *b); }
uint64_t API UI8DivR(uint64_t *a, uint64_t *b) { return UI8Div(*a, *b); }
uint64_t API UI8ModR(uint64_t *a, uint64_t *b) { return UI8Mod(*a, *b); }
uint64_t API UI8ShlR(uint64_t *a, int b) { return UI8Shl(*a, b); }
uint64_t API UI8ShrR(uint64_t *a, int b) { return UI8Shr(*a, b); }
int API UI8CmpR(uint64_t *a, uint64_t *b) { return UI8Cmp(*a, *b); }

