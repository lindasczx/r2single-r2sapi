#include <stddef.h>
#include <stdio.h>
#include <winsock2.h>

// fix windows missing inet_ntop missing symbol when built on >= vista but run on < vista!
// see: switch_utils.c inside FreeSwitch.
/*
 * WARNING: Don't even consider trying to compile this on a system where
 * sizeof(int) < 4.  sizeof(int) > 4 is fine; all the world's not a VAX.
 */

static const char *inet_ntop4(const unsigned char *src, char *dst, size_t size);
static const char *inet_ntop6(const unsigned char *src, char *dst, size_t size);


/* char *
* inet_ntop(af, src, dst, size)
*	convert a network format address to presentation format.
* return:
*	pointer to presentation format address (`dst'), or NULL (see errno).
* author:
*	Paul Vixie, 1996.
*/
const char* WINAPI inet_ntop(int af, void const *src, char *dst, size_t size)
{

	switch (af) {
	case AF_INET:
		return inet_ntop4(src, dst, size);
	case AF_INET6:
		return inet_ntop6(src, dst, size);
	default:
		return NULL;
	}
	/* NOTREACHED */
}

/* const char *
 * inet_ntop4(src, dst, size)
 *	format an IPv4 address, more or less like inet_ntoa()
 * return:
 *	`dst' (as a const)
 * notes:
 *	(1) uses no statics
 *	(2) takes a unsigned char* not an in_addr as input
 * author:
 *	Paul Vixie, 1996.
 */
static const char *inet_ntop4(const unsigned char *src, char *dst, size_t size)
{
	static const char fmt[] = "%u.%u.%u.%u";
	char tmp[sizeof "255.255.255.255"];

	memset(tmp, 0, sizeof(tmp));
	_snprintf(tmp, sizeof(tmp)-1, fmt, src[0], src[1], src[2], src[3]);
	// if (switch_snprintf(tmp, sizeof tmp, fmt, src[0], src[1], src[2], src[3]) >= (int) size) 
	//	return NULL;

	return strcpy(dst, tmp);
}

/* const char *
 * inet_ntop6(src, dst, size)
 *	convert IPv6 binary address into presentation (printable) format
 * author:
 *	Paul Vixie, 1996.
 */
static const char *inet_ntop6(unsigned char const *src, char *dst, size_t size)
{
	/*
	 * Note that int32_t and int16_t need only be "at least" large enough
	 * to contain a value of the specified size.  On some systems, like
	 * Crays, there is no such thing as an integer variable with 16 bits.
	 * Keep this in mind if you think this function should have been coded
	 * to use pointer overlays.  All the world's not a VAX.
	 */
	char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
	struct {
		int base, len;
	} best = {-1, 0}, cur = {-1, 0};
	unsigned int words[8];
	int i;

	/*
	 * Preprocess:
	 *  Copy the input (bytewise) array into a wordwise array.
	 *  Find the longest run of 0x00's in src[] for :: shorthanding.
	 */
	for (i = 0; i < 16; i += 2)
		words[i / 2] = (src[i] << 8) | (src[i + 1]);
	best.base = -1;
	cur.base = -1;
	for (i = 0; i < 8; i++) {
		if (words[i] == 0) {
			if (cur.base == -1)
				cur.base = i, cur.len = 1;
			else
				cur.len++;
		} else {
			if (cur.base != -1) {
				if (best.base == -1 || cur.len > best.len)
					best = cur;
				cur.base = -1;
			}
		}
	}
	if (cur.base != -1) {
		if (best.base == -1 || cur.len > best.len)
			best = cur;
	}
	if (best.base != -1 && best.len < 2)
		best.base = -1;

	/*
	 * Format the result.
	 */
	tp = tmp;
	for (i = 0; i < 8; i++) {
		/* Are we inside the best run of 0x00's? */
		if (best.base != -1 && i >= best.base && i < (best.base + best.len)) {
			if (i == best.base)
				*tp++ = ':';
			continue;
		}
		/* Are we following an initial run of 0x00s or any real hex? */
		if (i != 0)
			*tp++ = ':';
		/* Is this address an encapsulated IPv4? */
		if (i == 6 && best.base == 0 && (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
			if (!inet_ntop4(src + 12, tp, sizeof tmp - (tp - tmp)))
				return (NULL);
			tp += strlen(tp);
			break;
		}
		tp += sprintf(tp, "%x", words[i]);
	}
	/* Was it a trailing run of 0x00's? */
	if (best.base != -1 && (best.base + best.len) == 8)
		*tp++ = ':';
	*tp++ = '\0';

	/*
	 * Check for overflow, copy, and we're done.
	 */
	if ((size_t) (tp - tmp) >= size) {
		return NULL;
	}

	return strcpy(dst, tmp);
}

