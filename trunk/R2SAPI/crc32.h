#ifndef CRC32_H_
#define CRC32_H_

const unsigned int * __stdcall get_crc_table();
unsigned int __stdcall crc32(unsigned int crc, const unsigned char* buf, unsigned int len);
unsigned int __stdcall crc32_combine(unsigned int crc1, unsigned int crc2, int len2);
unsigned int __stdcall crc32_combine64(unsigned int crc1, unsigned int crc2, __int64 len2);

#endif
