#ifndef CRC64_H_
#define CRC64_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "r2sapi.h"

const unsigned __int64 * API get_crc_table64();
unsigned __int64 API crc64(unsigned __int64 crc, const unsigned char* buf, unsigned __int64 len);
unsigned __int64 API crc64_combine(unsigned __int64 crc1, unsigned __int64 crc2, __int64 len2);

#ifdef __cplusplus
}
#endif

#endif
