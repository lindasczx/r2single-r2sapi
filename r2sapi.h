#ifndef R2SAPI_H_
#define R2SAPI_H_

#include "resource.h"

#ifdef __cplusplus
extern "C"{
#endif

#define API __stdcall
#define API_ALIGN16 __attribute__((force_align_arg_pointer)) __stdcall

int API _();

typedef struct {
	unsigned int MMX :1;
	unsigned int SSE :1;
	unsigned int SSE2 :1;
	unsigned int SSE3 :1;
	unsigned int SSE3S :1;
	unsigned int SSE41 :1;
	unsigned int SSE42 :1;
	unsigned int AVX :1;
	unsigned int AVX2 :1;
	unsigned int XOP :1;
} SIMD_VERSION;
void GetSIMDVersion();
extern SIMD_VERSION SIMD_Version;

#ifdef __cplusplus
}
#endif

#endif

