#include "lltow.h"
#include <stdint.h>

// GCC快，VC++慢
size_t lltow_upper(int64_t value, wchar_t *sp, int radix) {
	wchar_t tmp[65];	// be careful with the length of the buffer
	wchar_t *tp = tmp;
	int i;
	uint64_t v;
	size_t len;

	int sign = (radix == 10 && value < 0);    
	if (sign)
		v = -value;
	else
		v = (uint64_t)value;

	while (v || tp == tmp) {
		i = v % radix;
		v /= radix;	// v/=radix uses less CPU clocks than v=v/radix does
		if (i < 10)
			*tp++ = i + '0';
		else
			*tp++ = i + 'A' - 10;
	}

	len = tp - tmp;

	if (sign) {
		*sp++ = '-';
		len++;
	}

	while (tp > tmp)
		*sp++ = *--tp;
	*sp = '\0';

	return len;
}

// GCC快，VC++慢
size_t ulltow_upper(uint64_t value, wchar_t *sp, int radix) {
	wchar_t tmp[65];	// be careful with the length of the buffer
	wchar_t *tp = tmp;
	int i;
	uint64_t v = value;
	size_t len;

	while (v || tp == tmp) {
		i = v % radix;
		v /= radix;	// v/=radix uses less CPU clocks than v=v/radix does
		if (i < 10)
			*tp++ = i + '0';
		else
			*tp++ = i + 'A' - 10;
	}

	len = tp - tmp;

	while (tp > tmp)
		*sp++ = *--tp;
	*sp = '\0';

	return len;
}

