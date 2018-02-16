#ifndef CPUID_H_
#define CPUID_H_

#ifdef __cplusplus
extern "C" {
#endif

void getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue);
void getcpuid(unsigned int CPUInfo[4], unsigned int InfoType);

#ifdef __cplusplus
}
#endif

#endif
