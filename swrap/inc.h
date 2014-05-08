#ifndef INC_H_
#define INC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define UNICODE
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#include <windows.h>

#define EXPORT extern "C"
#define API __stdcall

#define USEBASS
//#define DEBUG
#ifdef DEBUG
#define _DEBUG
void printd(char const* format, ...);
void wprintd(wchar_t const* format, ...);
#endif

#define H_MSS	0
#define H_BASS	1

#define MAX_HANDLE	10000
typedef struct _HandleManager{
	int type;
	int handle;
}HandleManager;



#ifdef __cplusplus
}
#endif

#endif // #ifndef INC_H_
