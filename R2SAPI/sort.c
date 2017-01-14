#include "sort.h"
#include <stdlib.h>
#include <string.h>

//typedef int (__stdcall *stdcallcomparefunc)(const void *a, const void *b);

void __stdcall QuickSort(void* base, size_t num, size_t size, stdcallcomparefunc stdcallcompare) {
	int __cdecl cdeclcompare(const void *a, const void *b) {
		return stdcallcompare(a, b);
	}
	qsort(base, num, size, cdeclcompare);
}

static int min(int x, int y) {
	return x < y ? x : y;
}
/*
static void swap(char *a, char *b, size_t width) {
	char tmp;
	if ( a != b )
	while ( width-- ) {
		tmp = *a;
		*a++ = *b;
		*b++ = tmp;
	}
}
*/
void __stdcall MergeSort(void* base, size_t num, size_t size, stdcallcomparefunc stdcallcompare) {
	char* a = base;
	char* b = malloc(num * size);
	int seg, start;
	for (seg = 1; seg < num; seg += seg) {
		for (start = 0; start < num; start += seg + seg) {
			int low = start, mid = min(start + seg, num), high = min(start + seg + seg, num);
			int k = low;
			int start1 = low, end1 = mid;
			int start2 = mid, end2 = high;
			while (start1 < end1 && start2 < end2)
				memcpy(b + size * k++, stdcallcompare(a + size * start1, a + size * start2) <= 0 ? a + size * start1++ : a + size * start2++, size);
			while (start1 < end1)
				memcpy(b + size * k++, a + size * start1++, size);
			while (start2 < end2)
				memcpy(b + size * k++, a + size * start2++, size);
		}
		char* temp = a;
		a = b;
		b = temp;
	}
	if (a != base) {
		memcpy(b, a, num * size);
		b = a;
	}
	free(b);
}

