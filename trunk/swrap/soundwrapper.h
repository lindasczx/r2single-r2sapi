#ifndef SOUNDWRAPPER_H
#define SOUNDWRAPPER_H

#include "inc.h"
#include <string.h>
#include <io.h>

namespace MSS {
#include "mss.h"
}

namespace BASS {
#include "bass.h"
}

using namespace MSS;
using namespace BASS;

EXPORT int  API AllocateSampleHandle(int hDig);
EXPORT void API CloseStream(int hStream);
EXPORT void API GetSampleMsPosition(int hSample, long* total_ms, long* current_ms);
EXPORT int  API GetSamplePlaybackRate(int hSample);
EXPORT unsigned int	API GetSampleStatus(int hSample);
EXPORT void API GetStreamInfo(int hStream, int* datarate, int* sndtype, int* length, int* memory);
EXPORT void API GetStreamMsPosition(int hStream, long* total_ms, long* current_ms);
EXPORT int  API GetStreamPlaybackRate(int hStream);
EXPORT int  API GetStreamStatus(int hStream);
EXPORT int  API OpenStream(int hDig, char const* FileName, int Stream_Mem);
EXPORT void API PauseStream(int hStream);
EXPORT void API ReleaseSampleHandle(int hSample);
EXPORT void API ResumeSample(int hSample);
EXPORT int  API ServiceStream(int hStream, int fillup);
EXPORT int  API SetDirectSoundHwnd(int hDig, int hWnd);
EXPORT void API SetFileCallbacks(AIL_file_open_callback opencb, AIL_file_close_callback closecb, AIL_file_seek_callback seekcb, AIL_file_read_callback readcb);
EXPORT int  API SetNamedSampleFile(int hSample, char const* file_type_suffix, void const* file_image, int file_size, int block);
EXPORT int  API SetPreference(unsigned int num, int value);
EXPORT void API SetSampleLoopCount(int hSample, int count);
EXPORT void API SetSampleMsPosition(int hSample, long ms);
EXPORT void API SetSamplePlaybackRate(int hSample, int rate);
EXPORT void API SetSampleVolume(int hSample, float left_level, float right_level);
EXPORT void API SetStreamLoopCount(int hStream, int count);
EXPORT void API SetStreamMsPosition(int hStream, long ms);
EXPORT void API SetStreamPlaybackRate(int hStream, int rate);
EXPORT void API SetStreamVolume(int hStream, float left_level, float right_level);
EXPORT void API Shutdown(void);
EXPORT void API StartSample(int hSample);
EXPORT void API StartStream(int hStream);
EXPORT void API StopSample(int hSample);

int FreeBassPlugin();
int LoadBassPlugin();
FILECLOSEPROC*MyFcloseProc();
FILELENPROC  *MyFlenProc();
FILEREADPROC *MyFreadProc();
FILESEEKPROC *MyFseekProc();

int GetFreeHandle();
int AddHandle(int type, int handle, int callbackhandle);
void AssignHandle(int h, int type, int handle);
void DelHandle(int h);
int FindHandle(int h);
int FindHandleType(int h);



#endif
