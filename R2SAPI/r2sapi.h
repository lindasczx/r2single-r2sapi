#ifndef R2SAPI_H_
#define R2SAPI_H_

#include "resource.h"

#ifdef __cplusplus
extern "C"{
#endif

#define API __attribute__((force_align_arg_pointer)) __stdcall

int API _();

typedef enum {
	SIMD_NONE,
	SIMD_MMX,
	SIMD_SSE,
	SIMD_SSE2,
	SIMD_SSE3,
	SIMD_SSE3S,
	SIMD_SSE41,
	SIMD_SSE42,
	SIMD_AVX,
	SIMD_AVX2,
	SIMD_XOP
} SIMD_VERSION;
SIMD_VERSION GetSIMDVersion();
extern SIMD_VERSION _SIMD_Version;

#ifdef __cplusplus
}
#endif

#endif

