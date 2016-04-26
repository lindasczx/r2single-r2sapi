#ifndef SOUNDWRAPPER_H
#define SOUNDWRAPPER_H

#include "inc.h"

namespace MSS {
#include "mss.h"
}

namespace BASS {
#include "bass.h"
}

using namespace MSS;
using namespace BASS;

EXPORT void API Shutdown(void);
EXPORT int  API SetDirectSoundHwnd(int hDig, int hWnd);
EXPORT void API SetFileCallbacks(AIL_file_open_callback opencb, AIL_file_close_callback closecb, AIL_file_seek_callback seekcb, AIL_file_read_callback readcb);
EXPORT int  API SetPreference(unsigned int num, int value);

EXPORT Handle* API AllocateSampleHandle(int hDig);
EXPORT int  API SetNamedSampleFile(Handle* hSample, char const* file_type_suffix, void const* file_image, int file_size, int block);
EXPORT void API ReleaseSampleHandle(Handle* hSample);
EXPORT void API GetSampleMsPosition(Handle* hSample, int* total_ms, int* current_ms);
EXPORT int  API GetSamplePlaybackRate(Handle* hSample);
EXPORT unsigned int API GetSampleStatus(Handle* hSample);
EXPORT void API ResumeSample(Handle* hSample);
EXPORT void API SetSampleLoopCount(Handle* hSample, int count);
EXPORT void API SetSampleMsPosition(Handle* hSample, int ms);
EXPORT void API SetSamplePlaybackRate(Handle* hSample, int rate);
EXPORT void API SetSampleVolume(Handle* hSample, float left_level, float right_level);
EXPORT void API StartSample(Handle* hSample);
EXPORT void API StopSample(Handle* hSample);

EXPORT Handle* API OpenStream(int hDig, char const* FileName, int Stream_Mem);
EXPORT void API CloseStream(Handle* hStream);
EXPORT void API GetStreamInfo(Handle* hStream, int* datarate, int* sndtype, int* length, int* memory);
EXPORT void API GetStreamMsPosition(Handle* hStream, int* total_ms, int* current_ms);
EXPORT int  API GetStreamPlaybackRate(Handle* hStream);
EXPORT int  API GetStreamStatus(Handle* hStream);
EXPORT void API PauseStream(Handle* hStream);
EXPORT int  API ServiceStream(Handle* hStream, int fillup);
EXPORT void API SetStreamLoopCount(Handle* hStream, int count);
EXPORT void API SetStreamMsPosition(Handle* hStream, int ms);
EXPORT void API SetStreamPlaybackRate(Handle* hStream, int rate);
EXPORT void API SetStreamVolume(Handle* hStream, float left_level, float right_level);
EXPORT void API StartStream(Handle* hStream);

int FreeBassPlugin();
int LoadBassPlugin();
FILECLOSEPROC*MyFcloseProc();
FILELENPROC  *MyFlenProc();
FILEREADPROC *MyFreadProc();
FILESEEKPROC *MyFseekProc();

inline Handle* GetFreeHandle();
inline Handle* AddHandle(int type, int value);
inline void AssignHandle(Handle* h, int type, int value);
inline void DelHandle(Handle* h);
inline int FindHandle(Handle* h);
inline int FindHandleType(Handle* h);
int FileType(const char* FileName);

float DirectSoundVolumeToLinearVolume(float v);
//float DirectSoundVolumeToMssVolume(float v);
//float LinearVolumeToDirectSoundVolume(float v);
float MssVolumeToDirectSoundVolume(float v);

#endif
