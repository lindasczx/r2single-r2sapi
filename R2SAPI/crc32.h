#ifndef CRC32_H_
#define CRC32_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r2sapi.h"

const unsigned int * API get_crc_table();
unsigned int API crc32_z(unsigned int crc, const unsigned char* buf, size_t len);
unsigned int API crc32_combine(unsigned int crc1, unsigned int crc2, int len2);
unsigned int API crc32_combine64(unsigned int crc1, unsigned int crc2, __int64 len2);

// for deflate/inflate
unsigned int API crc32(unsigned int crc, const unsigned char* buf, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif
