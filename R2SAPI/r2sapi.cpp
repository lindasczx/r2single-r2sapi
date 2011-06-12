#include "r2sapi.h"

extern "C"
BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
                       DWORD reason        /* Reason this function is being called. */ ,
                       LPVOID reserved     /* Not used. */ )
{
	switch (reason){
	case DLL_PROCESS_ATTACH:
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

int WINAPI _(){
	return APIVER;
}
