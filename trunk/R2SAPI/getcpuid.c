// intrinsics
#if defined(__GNUC__)	// GCC
#	include <cpuid.h>
#elif defined(_MSC_VER)	// MSVC
#	if _MSC_VER >=1400	// VC2005
#		include <intrin.h>
#	endif	// #if _MSC_VER >=1400
#else
#	error Only supports MSVC or GCC.
#endif	// #if defined(__GNUC__)

void getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue)
{
#if defined(__GNUC__)	// GCC
	__cpuid_count(InfoType, ECXValue, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
#elif defined(_MSC_VER)	// MSVC
	#if defined(_WIN64) || _MSC_VER>=1600	// 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.
		__cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
	#else
		if (NULL==CPUInfo) return;
		_asm{
			// load. 读取参数到寄存器.
			mov edi, CPUInfo;	// 准备用edi寻址CPUInfo
			mov eax, InfoType;
			mov ecx, ECXValue;
			// CPUID
			cpuid;
			// save. 将寄存器保存到CPUInfo
			mov    [edi], eax;
			mov    [edi+4], ebx;
			mov    [edi+8], ecx;
			mov    [edi+12], edx;
		}
	#endif
#endif	// #if defined(__GNUC__)
}

void getcpuid(unsigned int CPUInfo[4], unsigned int InfoType)
{
#if defined(__GNUC__)	// GCC
	__cpuid(InfoType, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
#elif defined(_MSC_VER)	// MSVC
	#if _MSC_VER>=1400	// VC2005才支持__cpuid
		__cpuid((int*)(void*)CPUInfo, (int)InfoType);
	#else
		getcpuidex(CPUInfo, InfoType, 0);
	#endif
#endif	// #if defined(__GNUC__)
}

