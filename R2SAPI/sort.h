#ifndef _SORT_H_
#define _SORT_H_

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (__stdcall *stdcallcomparefunc)(const void *a, const void *b);

void __stdcall QuickSort(void* base, size_t num, size_t size, stdcallcomparefunc stdcallcompare);
void __stdcall MergeSort(void* base, size_t num, size_t size, stdcallcomparefunc stdcallcompare);

#ifdef __cplusplus
}
#endif

#endif
