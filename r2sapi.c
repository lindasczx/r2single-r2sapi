#include "r2sapi.h"
#include "getcpuid.h"
#include "mt19937.h"
#include <stdlib.h>
#include <time.h>
#include <windows.h>

SIMD_VERSION SIMD_Version = {0};

void GetSIMDVersion() {
	memset(&SIMD_Version, 0, sizeof(SIMD_Version));
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
		SIMD_Version.AVX2 = 1;
	if (CPUInfo[1][2] & 0x10000000)
		SIMD_Version.AVX = 1;
	if (CPUInfo[2][2] & 0x800)
		SIMD_Version.XOP = 1;
	if (CPUInfo[1][2] & 0x100000)
		SIMD_Version.SSE42 = 1;
	if (CPUInfo[1][2] & 0x80000)
		SIMD_Version.SSE41 = 1;
	if (CPUInfo[1][2] & 0x200)
		SIMD_Version.SSE3S = 1;
	if (CPUInfo[1][2] & 0x1)
		SIMD_Version.SSE3 = 1;
	if (CPUInfo[1][3] & 0x4000000)
		SIMD_Version.SSE2 = 1;
	if (CPUInfo[1][3] & 0x2000000)
		SIMD_Version.SSE = 1;
	if (CPUInfo[1][3] & 0x800000)
		SIMD_Version.MMX = 1;

	extern unsigned int OPENSSL_ia32cap_P[2];
	OPENSSL_ia32cap_P[0] = CPUInfo[1][3];
	OPENSSL_ia32cap_P[1] = CPUInfo[1][2];
}

BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
                       DWORD reason        /* Reason this function is being called. */ ,
                       LPVOID reserved     /* Not used. */ )
{
	switch (reason){
	case DLL_PROCESS_ATTACH:
		//if (!IsValidCodePage(54936))	// GB18030
		//	return FALSE;
		GetSIMDVersion();
		srand(time(NULL));
		MT64_SRand(time(NULL));
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

