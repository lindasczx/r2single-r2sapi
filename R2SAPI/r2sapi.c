#include "r2sapi.h"
#include "getcpuid.h"
#include <windows.h>

SIMD_VERSION _SIMD_Version;

SIMD_VERSION GetSIMDVersion() {
	unsigned int CPUInfo[4][4] = {0};
	unsigned int idcount, idcount2;
	getcpuid(CPUInfo[0], 0);
	idcount = CPUInfo[0][0];
	getcpuid(CPUInfo[0], 0x80000000);
	idcount2 = CPUInfo[0][0];
	if (idcount >= 7)
		getcpuidex(CPUInfo[3], 7, 0);
	if (idcount2 >= 1)
		getcpuid(CPUInfo[2], 0x80000001);
	if (idcount >= 1)
		getcpuid(CPUInfo[1], 1);
	if (CPUInfo[3][1] & 0x20)
		return SIMD_AVX2;
	if (CPUInfo[1][2] & 0x10000000)
		return SIMD_AVX;
	if (CPUInfo[2][2] & 0x800)
		return SIMD_XOP;
	if (CPUInfo[1][2] & 0x100000)
		return SIMD_SSE42;
	if (CPUInfo[1][2] & 0x80000)
		return SIMD_SSE41;
	if (CPUInfo[1][2] & 0x200)
		return SIMD_SSE3S;
	if (CPUInfo[1][2] & 0x1)
		return SIMD_SSE3;
	if (CPUInfo[1][3] & 0x4000000)
		return SIMD_SSE2;
	if (CPUInfo[1][3] & 0x2000000)
		return SIMD_SSE;
	if (CPUInfo[1][3] & 0x800000)
		return SIMD_MMX;
	if (CPUInfo[2][3] & 0x800000)
		return SIMD_MMX;
	return SIMD_NONE;
}

BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
                       DWORD reason        /* Reason this function is being called. */ ,
                       LPVOID reserved     /* Not used. */ )
{
	switch (reason){
	case DLL_PROCESS_ATTACH:
		//if (!IsValidCodePage(54936))	// GB18030
		//	return FALSE;
		_SIMD_Version = GetSIMDVersion();
		break;
		
	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}

	/* Returns TRUE on success, FALSE on failure */
	return TRUE;
}

int API _(){
	return APIVER;
}

