#include <stdlib.h>

typedef int (__stdcall *stdcallcomparefunc)(const void *a, const void *b);

void __stdcall QuickSort(void* base, size_t num, size_t size, stdcallcomparefunc stdcallcompare) {
	int __cdecl cdeclcompare(const void *a, const void *b) {
		return stdcallcompare(a, b);
	}
	qsort(base, num, size, cdeclcompare);
}

