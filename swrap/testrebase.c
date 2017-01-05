#include "mss.h"
#include <stdio.h>
#include <windows.h>

int main() {
	HMODULE hMod = LoadLibraryW(L"swrap.dll");
	AIL_startup();
	if (hMod == NULL)
		printf("rebase test FAILED!\n");
	else
		printf("rebase test OK, loaded at address 0x%p\n", hMod);
	getchar();
}
