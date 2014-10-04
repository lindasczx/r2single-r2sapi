#include "r2sapi.h"
#include <stdint.h>
#include <wchar.h>
#include <errno.h>
#include <windows.h>

#define BITCOUNT 64

// I8/UI8 <=> R8
double API I8R8(__int64 a);
double API UI8R8(unsigned __int64 a);
__int64 API R8I8(double a);
unsigned __int64 API R8UI8(double a);

// I8/UI8 <=> BSTR
BSTR API I8Bstr(__int64 a);
BSTR API UI8Bstr(unsigned __int64 a);
BSTR API I8BstrHex(__int64 a);
BSTR API I8BstrOct(__int64 a);
__int64 API BstrI8(const BSTR a);
unsigned __int64 API BstrUI8(const BSTR a);

double API I8R8(__int64 a) {
	return (double)a;
}

double API UI8R8(unsigned __int64 a) {
	return (double)a;
}

__int64 API R8I8(double a) {
	return (__int64)a;
}

unsigned __int64 API R8UI8(double a) {
	return (unsigned __int64)a;
}

BSTR API I8Bstr(__int64 a) {
	wchar_t b[34];
	_i64tow(a, b, 10);
	BSTR s = SysAllocString(b);
	return s;
}

BSTR API UI8Bstr(unsigned __int64 a) {
	wchar_t b[34];
	_ui64tow(a, b, 10);
	BSTR s = SysAllocString(b);
	return s;
}

BSTR API I8BstrHex(__int64 a) {
	wchar_t b[34];
	_i64tow(a, b, 16);
	BSTR s = SysAllocString(b);
	return s;
}


BSTR API I8BstrOct(__int64 a) {
	wchar_t b[34];
	_i64tow(a, b, 8);
	BSTR s = SysAllocString(b);
	return s;
}

// 支持 &B &O &H 表示N进制
// 支持 0 0x 表示N进制
__int64 API BstrI8(const BSTR a) {
	__int64 r;
	int base = 0;
	const wchar_t *p, *s = a;
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
	r = _wcstoi64(s, NULL, base);
	if (errno == ERANGE)
		SetLastError(ERROR_ARITHMETIC_OVERFLOW);
	return r;
}

unsigned __int64 API BstrUI8(const BSTR a) {
	__int64 r;
	int base = 0;
	const wchar_t *p, *s = a;
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

double API I8R8R(__int64 *a) { return I8R8(*a); }
double API UI8R8R(unsigned __int64 *a) { return UI8R8(*a); }
BSTR API I8BstrR(__int64 *a) { return I8Bstr(*a); }
BSTR API UI8BstrR(unsigned __int64 *a) { return UI8Bstr(*a); }
BSTR API I8BstrHexR(__int64 *a) { return I8BstrHex(*a); }
BSTR API I8BstrOctR(__int64 *a) { return I8BstrOct(*a); }

