#include "r2sapi.h"

#define BITCOUNT 32

int API I4Shl(int a, int b);
int API I4Shr(int a, int b);
int API I4Rol(int a, int b);
int API I4Ror(int a, int b);
unsigned int API UI4Mul(unsigned int a, unsigned int b);
unsigned int API UI4Div(unsigned int a, unsigned int b);
unsigned int API UI4Mod(unsigned int a, unsigned int b);
unsigned int API UI4Shl(unsigned int a, int b);
unsigned int API UI4Shr(unsigned int a, int b);
int API UI4Cmp(unsigned int a, unsigned int b);

int API I4Shl(int a, int b) {
	if (b < 0)
		return I4Shr(a, -b);
	else if (b >= BITCOUNT)
		return 0;
	else
		return a << b;
}

int API I4Shr(int a, int b) {
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

int API I4Rol(int p, int b) {
	unsigned int a = (unsigned int)p;
	b &= BITCOUNT - 1;
	return a << b | a >> BITCOUNT - b;
}

int API I4Ror(int p, int b) {
	unsigned int a = (unsigned int)p;
	b &= BITCOUNT - 1;
	return a >> b | a << BITCOUNT - b;
}

unsigned int API UI4Mul(unsigned int a, unsigned int b) {
	return a * b;
}

unsigned int API UI4Div(unsigned int a, unsigned int b) {
	return a / b;
}

unsigned int API UI4Mod(unsigned int a, unsigned int b) {
	return a % b;
}

unsigned int API UI4Shl(unsigned int a, int b) {
	if (b < 0)
		return UI4Shr(a, -b);
	else if (b >= BITCOUNT)
		return 0;
	else
		return a << b;
}

unsigned int API UI4Shr(unsigned int a, int b) {
	if (b < 0)
		return UI4Shl(a, -b);
	else if (b >= BITCOUNT)
		return 0;
	else
		return a >> b;
}

int API UI4Cmp(unsigned int a, unsigned int b) {
	if (a > b)
		return 1;
	else if (a == b)
		return 0;
	else
		return -1;
}

