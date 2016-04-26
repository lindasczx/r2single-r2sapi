#include "soundwrapper.h"
#include <string.h>
#include <io.h>
#include <cmath>
using namespace std;

int hDig;

//extern "C" Handle hHandleList[MAX_HANDLE+1];
#ifdef DEBUG
extern "C" int handlecount=0;
#endif
//int curhandlepos=1;
//int lasthandle=0, lasttype=H_MSS;
//void *lastmem=NULL;
Handle *lasthandle = NULL;	// gfis内存泄漏hack
AIL_file_open_callback	fopenproc=NULL;
AIL_file_close_callback	fcloseproc=NULL;
AIL_file_seek_callback	fseekproc=NULL;
AIL_file_read_callback	freadproc=NULL;

// MSS对sample有数量限制，但是stream没有；BASS统一为stream，没有限制。
// v115fix: 把两种handle统一起来：
//          sample不再使用循环数组管理，而使用malloc/free管理。MSS对sample的数量限制扔给MSS管理。
//          stream用malloc/free接管，不再野外放生，删除有BUG的MSS/BASS handle鉴别函数。

//////////////////////////////////////
// 内部函数
//////////////////////////////////////

inline Handle* GetFreeHandle(){
	if (lasthandle && lasthandle->value == 0) {	// gfis由于Alloc以后出错没有Free会导致内存泄漏，这里堵上
		return lasthandle;
	} else {
		Handle *p = (Handle *)malloc(sizeof(Handle));
		lasthandle = p;
#ifdef DEBUG
		handlecount++;
#endif
		return p;
	}
}

inline Handle* AddHandle(int type, int value){
	Handle *handle = GetFreeHandle();
	if (handle == NULL) return 0;
	handle->type = type;
	handle->value = value;
	return handle;
}

inline void AssignHandle(Handle* handle, int type, int value){
	handle->type = type;
	handle->value = value;
}

inline void DelHandle(Handle* handle){
	//handle->type = -1;
	//handle->value = 0;
	free(handle);
	if (handle == lasthandle) lasthandle = NULL;	// 应gfis而做的特殊情况处理，防止对lasthandle进行double free
#ifdef DEBUG
	handlecount--;
#endif
}

inline int FindHandle(Handle* handle){
	//if (handle->type == -1) return 0;
	return handle->value;
}

inline int FindHandleType(Handle* handle){
	return handle->type;
}

// mp3文件调用MSS解码
// 其它文件调用BASS解码
int FileType(const char* FileName) {
#ifdef USEBASS
	int l;
	char *t;
	l = strlen(FileName);
	if (l < 4)
		return H_BASS;
	t = (char *)FileName + l - 4;
	if (strcmpi(t, ".mp3") == 0) {
		return H_MSS;
	} else {
		return H_BASS;
	}
#else
	return H_MSS;
#endif
}

float DirectSoundVolumeToLinearVolume(float v) {
	if (v <= -10000.0f)
		return 0;
	else
		return pow(10.0f, v * 0.0005f);
}
/*
float DirectSoundVolumeToMssVolume(float v) {
	if (v <= -10000.0f)
		return 0;
	else
		return exp((v + 200) / 1430);
}

float LinearVolumeToDirectSoundVolume(float v) {
	float r;
	if (v <= 0)
		return -10000;
	else
		r = log10(v) * 2000.0f;
	if (r < -10000.0f) return -10000; else return r;
}
*/
float MssVolumeToDirectSoundVolume(float v) {
	// MSS的最低音量是0.0116785098798573025795，低于此数无法播放
	if (v < 0.0116785098798573025795f)
		return -10000;
	else
		return log(v) * 1430.0f - 200.0f;
}

//////////////////////////////////////
// 文件操作回调函数
//////////////////////////////////////

void CALLBACK MyFcloseProc(void* user){
	fcloseproc((unsigned)user);
}

QWORD CALLBACK MyFlenProc(void* user){
	int offset=fseekproc((unsigned)user,0,SEEK_CUR);
	int len=fseekproc((unsigned)user,0,SEEK_END);
	fseekproc((unsigned)user,offset,SEEK_SET);
	return len;
}

DWORD CALLBACK MyFreadProc(void* buffer, DWORD length, void* user){
	return freadproc((unsigned)user,buffer,length);
}

BOOL CALLBACK MyFseekProc(QWORD offset, void* user){
	fseekproc((unsigned)user,(int)offset,SEEK_SET);
	return TRUE;
}

//////////////////////////////////////
// 通用函数
//////////////////////////////////////

int LoadBassPlugin() {
#ifdef DEBUG
	printd("LoadBassPlugin\n");
#endif
	long hFind;
	int hNext, r;
	unsigned int OldErrorMode;
	_wfinddata_t FileInfo;

	OldErrorMode=SetErrorMode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
#ifdef DEBUG
	printd("OldErrorMode=%p\n", OldErrorMode);
#endif

	if( (hFind=_wfindfirst(L"bass*.dll", &FileInfo))!=-1 ){
		hNext=0;
		while(hNext==0) {
			r=BASS_PluginLoad((const char*)FileInfo.name, BASS_UNICODE);
#ifdef DEBUG
			wprintd(L"LoadBassPlugin %s, ret code %p\n", &FileInfo.name, r);
#endif
			hNext=_wfindnext(hFind, &FileInfo);
		}
		_findclose(hFind);
	}
	
	SetErrorMode(OldErrorMode);
	return 0;
}

int FreeBassPlugin() {
#ifdef DEBUG
	printd("FreeBassPlugin\n");
#endif
	BASS_PluginFree(0);
	return 0;
}

void API Shutdown(void) {
#ifdef DEBUG
	printd("Shutdown\n");
#endif
	if (lasthandle && lasthandle->value == 0)	// 应gfis而做的特殊情况处理，释放已分配却未使用的handle
		DelHandle(lasthandle);
	AIL_shutdown();
#ifdef DEBUG
	printd("MSS shut down\n");
#endif
#ifdef USEBASS
	FreeBassPlugin();
//	以前Win6.0+系统执行这句会挂起，原因未知，但是最新的Win7SP1x64正常
	BASS_Free();
	#ifdef DEBUG
		printd("BASS shut down.\n");
	#endif

#endif
/*
	ExitProcess(0);
#ifdef DEBUG
	printd("Exited process.\n");
#endif
*/
}

int API SetDirectSoundHwnd(int hDig, int hWnd) {
	int ret;
#ifdef DEBUG
	printd("SetDirectSoundHwnd: %p, %p\n", hDig, hWnd);
#endif
	AIL_set_DirectSound_HWND( (MSS::HDIGDRIVER)hDig, (HWND)hWnd );
#ifdef USEBASS
	ret=BASS_Init(-1, 44100, 0, (HWND)hWnd, 0);
#ifdef DEBUG
	printd("BASS_Init returned: %d\n", ret);
#endif
	LoadBassPlugin();
#endif
	return 0;
}

void API SetFileCallbacks(AIL_file_open_callback  opencb,
			AIL_file_close_callback closecb,
			AIL_file_seek_callback  seekcb,
			AIL_file_read_callback  readcb)
{
	fopenproc =opencb;
	fcloseproc=closecb;
	fseekproc =seekcb;
	freadproc =readcb;
	AIL_set_file_callbacks(opencb, closecb, seekcb, readcb);
}

int API SetPreference(unsigned int num, int value) {
#ifdef DEBUG
	printd("SetPreference: %d, %d\n", num, value);
#endif
	//AIL_set_preference(num, value);
	if (num == DIG_MIXER_CHANNELS){			// DIG_MIXER_CHANNELS = 1
		return AIL_set_preference(num, MAX_HANDLE);	//v76fix: num=1: 最大handle数量，超过就会报错。每个MSS的Sample结构占688字节，AIL_open_digital_driver时分配。
	} else {
		return AIL_set_preference(num, value);
	}
}

//////////////////////////////////////
// Sample相关函数
//////////////////////////////////////

Handle* API AllocateSampleHandle(int hDig) {
	::hDig=hDig;
	//int h = (int)AIL_allocate_sample_handle( (MSS::HDIGDRIVER)hDig );
	int h = 0;
	Handle *handle = AddHandle(H_MSS, h);
#ifdef DEBUG
	printd("AllocateSampleHandle: %p\n", handle);
#endif
	return handle;
}

int API SetNamedSampleFile(Handle* hSample, const char* file_type_suffix, const void* file_image, int file_size, int block) {
	int h, r;
//	if (file_image==lastmem){
//		//AssignHandle(hSample, lasttype, lasthandle);
//		r=1;
//	}else{
		if (FileType(file_type_suffix)==H_MSS) {
			h = (int)AIL_allocate_sample_handle( (MSS::HDIGDRIVER)::hDig );
			if (h) {
				AssignHandle(hSample, H_MSS, h);
				r = AIL_set_named_sample_file( (MSS::HSAMPLE)h, file_type_suffix, file_image, file_size, block );
			} else {
				r = 0;
			}
		}else{
			//h=FindHandle(hSample);
			//AIL_release_sample_handle( (MSS::HSAMPLE)h );
			h=BASS_StreamCreateFile( 1, file_image, 0, file_size, 0 );
			if(!h){
				r=0;
			}else{
				AssignHandle(hSample, H_BASS, h);
				r=1;
			}
		}
		//lasthandle=h;
		//lastmem=(void*)file_image;
//	}
#ifdef DEBUG
	printd("SetNamedSampleFile: %p->%p, %s, %p, %p, %d\n", hSample, h, file_type_suffix, file_image, file_size, block);
#endif
	return r;
}

void API ReleaseSampleHandle(Handle* hSample) {
#ifdef DEBUG
	printd("ReleaseSampleHandle: %p\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
	   	AIL_release_sample_handle( (MSS::HSAMPLE)FindHandle(hSample) );
	}else{
		BASS_StreamFree(FindHandle(hSample));
	}
	DelHandle(hSample);
}

void API GetSampleMsPosition(Handle* hSample, int* total_ms, int* current_ms){
#ifdef DEBUG
	printd("GetSampleMsPosition: %p, total, curr\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_sample_ms_position( (MSS::HSAMPLE)FindHandle(hSample), total_ms, current_ms );
	}else{
		//*total_ms=(int)( 1000.* BASS_ChannelBytes2Seconds(hStream, BASS_ChannelGetLength(hStream, BASS_POS_BYTE)) );
		*current_ms=(int)( 1000.* BASS_ChannelBytes2Seconds(FindHandle(hSample),
			BASS_ChannelGetPosition(FindHandle(hSample), BASS_POS_BYTE)) );
	}
}

int API GetSamplePlaybackRate(Handle* hSample) {
#ifdef DEBUG
	printd("GetSamplePlaybackRate: %p\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
		return AIL_sample_playback_rate( (MSS::HSAMPLE)FindHandle(hSample) );
	}else{
		float freq;
		BASS_ChannelGetAttribute(FindHandle(hSample), BASS_ATTRIB_FREQ, &freq);
		return (int)freq;
	}
}

unsigned int API GetSampleStatus(Handle* hSample) {
#ifdef DEBUG
	printd("GetSampleStatus: %p\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
		return AIL_sample_status( (MSS::HSAMPLE)FindHandle(hSample) );
	}else{
		unsigned int s=BASS_ChannelIsActive(FindHandle(hSample));
		switch(s){
		case BASS_ACTIVE_STOPPED:
			return SMP_STOPPED;
		case BASS_ACTIVE_PAUSED:
			return SMP_DONE;
		case BASS_ACTIVE_PLAYING:
			return SMP_PLAYING;
		case BASS_ACTIVE_STALLED:
			return SMP_DONE;
		default:
			return 0;
		}
	}
}

void API ResumeSample(Handle* hSample) {
#ifdef DEBUG
	printd("ResumeSample: %p\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_resume_sample( (MSS::HSAMPLE)FindHandle(hSample) );
	}else{
		BASS_ChannelPlay( FindHandle(hSample), 0 );
	}
}

void API SetSampleLoopCount(Handle* hSample, int count) {
#ifdef DEBUG
	printd("SetSampleLoopCount: %p, %d\n", hSample, count);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_set_sample_loop_count( (MSS::HSAMPLE)FindHandle(hSample), count );
	}else{
		if(count==0){
			BASS_ChannelFlags( FindHandle(hSample), BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
		}else{
			BASS_ChannelFlags( FindHandle(hSample), 0, BASS_SAMPLE_LOOP);
		}
	}
}

void API SetSampleMsPosition(Handle* hSample, int ms) {
#ifdef DEBUG
	printd("SetSampleMsPosition: %p, %d\n", hSample, ms);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_set_sample_ms_position( (MSS::HSAMPLE)FindHandle(hSample), ms );
	}else{
		int bps;
		BASS_CHANNELINFO info;
		BASS_ChannelGetInfo( FindHandle(hSample), &info );
		bps=info.freq * info.chans;
		if((info.flags & BASS_SAMPLE_8BITS)==0)bps*=2;
		BASS_ChannelSetPosition( FindHandle(hSample), (QWORD)(0.001*ms*bps), BASS_POS_BYTE );
	}
}

void API SetSamplePlaybackRate(Handle* hSample, int rate) {
#ifdef DEBUG
	printd("SetSamplePlaybackRate: %p, %d\n", hSample, rate);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_set_sample_playback_rate( (MSS::HSAMPLE)FindHandle(hSample), rate);
	}else{
		BASS_ChannelSetAttribute(FindHandle(hSample), BASS_ATTRIB_FREQ, (float)rate);
	}
}

void API SetSampleVolume(Handle* hSample, float left_level, float right_level) {
#ifdef DEBUG
	printd("SetSampleVolume: %p, %g, %g\n", hSample, left_level, right_level);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_set_sample_volume_levels( (MSS::HSAMPLE)FindHandle(hSample), left_level, right_level);
	}else{
		float vol=(left_level+right_level)*0.5f;
		float pan=(right_level-left_level);
		vol = DirectSoundVolumeToLinearVolume(MssVolumeToDirectSoundVolume(vol));
		BASS_ChannelSetAttribute(FindHandle(hSample), BASS_ATTRIB_VOL, vol);
		BASS_ChannelSetAttribute(FindHandle(hSample), BASS_ATTRIB_PAN, pan);
	}
}

void API StartSample(Handle* hSample) {
#ifdef DEBUG
	printd("StartSample: %p\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_start_sample( (MSS::HSAMPLE)FindHandle(hSample) );
	}else{
		BASS_ChannelPlay( FindHandle(hSample), 1 );
	}
}

void API StopSample(Handle* hSample) {
#ifdef DEBUG
	printd("StopSample: %p\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_stop_sample( (MSS::HSAMPLE)FindHandle(hSample) );
	}else{
		BASS_ChannelStop( FindHandle(hSample) );
	}
}

//////////////////////////////////////
// Stream相关函数
//////////////////////////////////////

Handle* API OpenStream(int hDig, char const* FileName, int Stream_Mem) {
	int r, h;
	Handle *handle;
	if (FileType(FileName) == H_MSS) {
		h = (int)AIL_open_stream( (MSS::HDIGDRIVER)hDig, FileName, Stream_Mem );
		if (h) {
			handle = AddHandle(H_MSS, h);
		} else {
			handle = NULL;
		}
	} else {
		if (fopenproc==NULL || fcloseproc==NULL || fseekproc==NULL || freadproc==NULL){
			h = BASS_StreamCreateFile( 0, FileName, 0, 0, 0 );
			handle = AddHandle(H_BASS, h);
		}else{
			void *userhandle;
			BASS_FILEPROCS fileprocs = {MyFcloseProc, MyFlenProc, MyFreadProc, MyFseekProc};
			r = fopenproc(FileName, (unsigned int*)&userhandle);
			if (r) {
				h = BASS_StreamCreateFileUser( STREAMFILE_NOBUFFER, 0, &fileprocs, userhandle );
				if (h) {
					handle = AddHandle(H_BASS, h);
				} else {
					handle = NULL;
				}
			} else {
				handle = NULL;
			}
		}
	}
#ifdef DEBUG
	printd("OpenStream: %s, %d, %p -> %p\n", FileName, Stream_Mem, h, handle);
#endif
	return handle;
}

void API CloseStream(Handle* hStream) {
#ifdef DEBUG
	printd("CloseStream: %p\n", hStream);
#endif
	if (FindHandleType(hStream)==H_MSS) {
		AIL_close_stream( (MSS::HSTREAM)FindHandle(hStream) );
	}else{
		BASS_StreamFree( FindHandle(hStream) );
	}
	DelHandle(hStream);
}

void API GetStreamInfo(Handle* hStream, int* datarate, int* sndtype, int* length, int* memory) {
	return;
}

void API GetStreamMsPosition(Handle* hStream, int* total_ms, int* current_ms){
#ifdef DEBUG
	printd("GetStreamMsPosition: %p, total, curr\n", hStream);
#endif
	if (FindHandleType(hStream)==H_MSS) {
		AIL_stream_ms_position( (MSS::HSTREAM)FindHandle(hStream), total_ms, current_ms );
	}else{
		//*total_ms=(int)( 1000.* BASS_ChannelBytes2Seconds( FindHandle(hStream), BASS_ChannelGetLength(FindHandle(hStream), BASS_POS_BYTE) ) );
		*current_ms=(int)( 1000.* BASS_ChannelBytes2Seconds( FindHandle(hStream), BASS_ChannelGetPosition(FindHandle(hStream), BASS_POS_BYTE) ) );
	}
}

int API GetStreamPlaybackRate(Handle* hStream) {
#ifdef DEBUG
	printd("GetStreamPlaybackRate: %p\n", hStream);
#endif
	if (FindHandleType(hStream)==H_MSS) {
		return AIL_stream_playback_rate( (MSS::HSTREAM)FindHandle(hStream) );
	}else{
		float freq;
		BASS_ChannelGetAttribute( FindHandle(hStream), BASS_ATTRIB_FREQ, &freq );
		return (int)freq;
	}
}

void API PauseStream(Handle* hStream) {
#ifdef DEBUG
	printd("PauseStream: %p\n", hStream);
#endif
	if (FindHandleType(hStream)==H_MSS) {
		AIL_pause_stream( (MSS::HSTREAM)FindHandle(hStream), 1 );
	}else{
		BASS_ChannelPause( FindHandle(hStream) );
	}
}

void API SetStreamLoopCount(Handle* hStream, int count) {
#ifdef DEBUG
	printd("SetStreamLoopCount: %p, %d\n", hStream, count);
#endif
	if (FindHandleType(hStream)==H_MSS) {
		AIL_set_stream_loop_count( (MSS::HSTREAM)FindHandle(hStream), count );
	}else{
		if(count==0){
			BASS_ChannelFlags( FindHandle(hStream), BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP );
		}else{
			BASS_ChannelFlags( FindHandle(hStream), 0, BASS_SAMPLE_LOOP );
		}
	}
}

void API SetStreamMsPosition(Handle* hStream, int ms) {
#ifdef DEBUG
	printd("SetStreamMsPosition: %p, %d\n", hStream, ms);
#endif
	if (FindHandleType(hStream)==H_MSS) {
		AIL_set_stream_ms_position( (MSS::HSTREAM)FindHandle(hStream), ms );
	}else{
		int bps;
		BASS_CHANNELINFO info;
		BASS_ChannelGetInfo( FindHandle(hStream), &info );
		bps=info.freq * info.chans;
		if((info.flags & BASS_SAMPLE_8BITS)==0)bps*=2;
		BASS_ChannelSetPosition( FindHandle(hStream), (QWORD)(0.001*ms*bps), BASS_POS_BYTE );
	}
}

void API SetStreamPlaybackRate(Handle* hStream, int rate) {
#ifdef DEBUG
	printd("SetStreamPlaybackRate: %p, %d\n", hStream, rate);
#endif
	if (FindHandleType(hStream)==H_MSS) {
		AIL_set_stream_playback_rate( (MSS::HSTREAM)FindHandle(hStream), rate );
	}else{
		BASS_ChannelSetAttribute( FindHandle(hStream), BASS_ATTRIB_FREQ, (float)rate );
	}
}

void API SetStreamVolume(Handle* hStream, float left_level, float right_level) {
#ifdef DEBUG
	printd("SetStreamVolume: %p, %g, %g\n", hStream, left_level, right_level);
#endif
	if (FindHandleType(hStream)==H_MSS) {
		AIL_set_stream_volume_levels( (MSS::HSTREAM)FindHandle(hStream), left_level, right_level );
	}else{
		float vol=(left_level+right_level)*0.5f;
		float pan=(right_level-left_level);
		vol = DirectSoundVolumeToLinearVolume(MssVolumeToDirectSoundVolume(vol));
		BASS_ChannelSetAttribute( FindHandle(hStream), BASS_ATTRIB_VOL, vol );
		BASS_ChannelSetAttribute( FindHandle(hStream), BASS_ATTRIB_PAN, pan );
	}
}

void API StartStream(Handle* hStream) {
#ifdef DEBUG
	printd("StartStream: %p\n", hStream);
#endif
	if (FindHandleType(hStream)==H_MSS) {
		AIL_start_stream( (MSS::HSTREAM)FindHandle(hStream) );
	}else{
		BASS_ChannelPlay( FindHandle(hStream), 1 );
	}
}

/*

int API GetStreamStatus(Handle* hStream);
int AIL_stream_status(void* hStream);

int API ServiceStream(Handle* hStream, int fillup);
int AIL_service_stream(void* hStream, int fillup);



*/
