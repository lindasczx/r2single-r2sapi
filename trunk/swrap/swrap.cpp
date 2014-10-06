#include "inc.h"

#ifdef DEBUG
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
static FILE *fp;

void printd(char const* format, ...){
	fprintf(fp, "%d: ", GetCurrentThreadId());
	va_list ap;
	va_start(ap, format);
	vfprintf(fp, format, ap);
	va_end(ap);
	fflush(fp);
}
void wprintd(wchar_t const* format, ...){
	fwprintf(fp, L"%d: ", GetCurrentThreadId());
	va_list ap;
	va_start(ap, format);
	vfwprintf(fp, format, ap);
	va_end(ap);
	fflush(fp);
}
#endif

extern "C"
BOOL APIENTRY DllMain (HINSTANCE hInst	/* Library instance handle. */ ,
			DWORD reason	/* Reason this function is being called. */ ,
			LPVOID reserved	/* Not used. */ )
{
#ifdef DEBUG
	//extern HandleManager hHandleList[];
	extern int handlecount;
#endif
	//int i;

	switch (reason){
	case DLL_PROCESS_ATTACH:
		;
#ifdef DEBUG
		fp=fopen("swrap.log", "wt");
		printd("LOG START\n");
		assert(fp!=NULL);
#endif
//		for(i=0;i<=MAX_HANDLE;i++)
//			hHandleList[i].type=-1;
			
		break;

	case DLL_PROCESS_DETACH:
		;
#ifdef DEBUG
		printd("Leaked handles count = %d\n", handlecount);
//		for(i=0;i<=MAX_HANDLE;i++)
//			if(hHandleList[i].type!=-1){
//				printd("(%d, %d, %p)\n", i, hHandleList[i].type, hHandleList[i].handle);
//			}

		fclose(fp);
#endif
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}

	/* Returns TRUE on success, FALSE on failure */
	return TRUE;
}
