#ifndef CRC64_H_
#define CRC64_H_

const unsigned __int64 * __stdcall get_crc_table64();
unsigned __int64 __stdcall crc64(unsigned __int64 crc, const unsigned char* buf, unsigned __int64 len);
unsigned __int64 __stdcall crc64_combine(unsigned __int64 crc1, unsigned __int64 crc2, __int64 len2);

#endif
