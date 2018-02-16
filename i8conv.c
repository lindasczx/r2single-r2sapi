#include "r2sapi.h"
#include <stdint.h>
#include <wchar.h>
#include <errno.h>
#include <windows.h>

#define BITCOUNT 64

// I8/UI8 <=> R8
double API I8R8(int64_t a);
double API UI8R8(uint64_t a);
int64_t API R8I8(double a);
uint64_t API R8UI8(double a);

// I8/UI8 <=> BSTR
BSTR API I8Bstr(int64_t a);
BSTR API UI8Bstr(uint64_t a);
BSTR API I8BstrHex(int64_t a);
BSTR API I8BstrOct(int64_t a);
int64_t API BstrI8(const BSTR a);
uint64_t API BstrUI8(const BSTR a);

double API I8R8(int64_t a) {
	return (double)a;
}

double API UI8R8(uint64_t a) {
	return (double)a;
}

int64_t API R8I8(double a) {
	return (int64_t)a;
}

uint64_t API R8UI8(double a) {
	return (uint64_t)a;
}

BSTR API I8Bstr(int64_t a) {
	wchar_t b[65];
	_i64tow(a, b, 10);
	BSTR s = SysAllocString(b);
	return s;
}

BSTR API UI8Bstr(uint64_t a) {
	wchar_t b[65];
	_ui64tow(a, b, 10);
	BSTR s = SysAllocString(b);
	return s;
}

BSTR API I8BstrHex(int64_t a) {
	wchar_t b[65];
	_i64tow(a, b, 16);
	BSTR s = SysAllocString(b);
	return s;
}


BSTR API I8BstrOct(int64_t a) {
	wchar_t b[65];
	_i64tow(a, b, 8);
	BSTR s = SysAllocString(b);
	return s;
}

// 支持 &B &O &H 表示N进制
// 支持 0 0x 表示N进制
int64_t API BstrI8(const BSTR a) {
	int64_t r;
	int base = 0;
	const wchar_t *p, *s = a;
	if (a == NULL) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	if (SysStringLen(a) == 0) return 0;
	if ((p = wcschr(a, L'&')) != NULL) {
		p++;
		if (*p) switch (*p) {
			case 'H':
			case 'h':
				base = 16;
				s = p + 1;
				break;
			case 'O':
			case 'o':
				base = 8;
				s = p + 1;
				break;
			case 'B':
			case 'b':
				base = 2;
				s = p + 1;
				break;
			default:
				base = 0;
		}
	}
	errno = 0;
	r = _wcstoi64(s, NULL, base);
	if (errno == ERANGE)
		SetLastError(ERROR_ARITHMETIC_OVERFLOW);
	return r;
}

uint64_t API BstrUI8(const BSTR a) {
	int64_t r;
	int base = 0;
	const wchar_t *p, *s = a;
	if (a == NULL) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	if (SysStringLen(a) == 0) return 0;
	if ((p = wcschr(a, L'&')) != NULL) {
		p++;
		if (*p) switch(*p) {
			case 'H':
			case 'h':
				base = 16;
				s = p + 1;
				break;
			case 'O':
			case 'o':
				base = 8;
				s = p + 1;
				break;
			case 'B':
			case 'b':
				base = 2;
				s = p + 1;
				break;
			default:
				base = 0;
		}
	}
	errno = 0;
	r = _wcstoui64(s, NULL, base);
	if (errno == ERANGE)
		SetLastError(ERROR_ARITHMETIC_OVERFLOW);
	return r;
}

double API I8R8R(int64_t *a) { return I8R8(*a); }
double API UI8R8R(uint64_t *a) { return UI8R8(*a); }
BSTR API I8BstrR(int64_t *a) { return I8Bstr(*a); }
BSTR API UI8BstrR(uint64_t *a) { return UI8Bstr(*a); }
BSTR API I8BstrHexR(int64_t *a) { return I8BstrHex(*a); }
BSTR API I8BstrOctR(int64_t *a) { return I8BstrOct(*a); }

