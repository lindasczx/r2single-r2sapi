#ifndef _MT19937_H_
#define _MT19937_H_

#ifdef __cplusplus
extern "C" {
#endif

void __stdcall MT32_SRand(unsigned int s);
void __stdcall MT32_SRand_Array(unsigned int init_key[], int key_length);
unsigned int __stdcall MT_Rand();
double __stdcall MT32_DRand();

void __stdcall MT64_SRand(unsigned long long s);
void __stdcall MT64_SRand_Array(unsigned long long init_key[], unsigned long long key_length);
unsigned int __stdcall MT64_Rand();
unsigned long long __stdcall MT64_Rand64();
double __stdcall MT64_DRand();

#ifdef __cplusplus
}
#endif

#endif
