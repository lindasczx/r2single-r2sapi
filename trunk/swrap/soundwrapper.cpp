#include "soundwrapper.h"

int hDig;

extern "C" HandleManager hHandleList[MAX_HANDLE+1];
int handlecount=0, curhandlepos=1;
int lasthandle=0, lasttype=H_MSS;
void *lastmem=NULL;
AIL_file_open_callback	fopenproc=NULL;
AIL_file_close_callback	fcloseproc=NULL;
AIL_file_seek_callback	fseekproc=NULL;
AIL_file_read_callback	freadproc=NULL;

int GetFreeHandle(){
	int i,c;
	if (handlecount >= MAX_HANDLE) return 0;
	for (i=curhandlepos, c=0; c<MAX_HANDLE; i++,c++) {
		if (hHandleList[i].type == -1 || hHandleList[i].handle == 0){
			curhandlepos=i;
			return i;
		}
		if (i>=MAX_HANDLE) i -= MAX_HANDLE;   //v75fix:这里的>=是为了防止i=10001的数组越界情况出现
	}
	return 0;
}

int AddHandle(int type, int handle, int callbackhandle){
	int h=GetFreeHandle();
	if(h==0)return 0;
	if(hHandleList[h].type==-1)handlecount++;
	hHandleList[h].type=type;
	hHandleList[h].handle=handle;
	return h;
}

void AssignHandle(int h, int type, int handle){
	hHandleList[h].type=type;
	hHandleList[h].handle=handle;
}

void DelHandle(int h){
	if(hHandleList[h].type!=-1)handlecount--;
	hHandleList[h].type=-1;
	hHandleList[h].handle=0;
}

int FindHandle(int h){
	if(hHandleList[h].type==-1)return 0;
	return hHandleList[h].handle;
}

int FindHandleType(int h){
	return hHandleList[h].type;
}

int HandleType(int Handle) {
#ifdef USEBASS
	if (Handle>0){
		return H_MSS;
	}else{
		return H_BASS;
	}
#else
	return H_MSS;
#endif
}

int FileType(char const* FileName) {
#ifdef USEBASS
	int l;
	char *t;
	l=strlen(FileName);
	if(l<4)
		return H_BASS;
	t=(char*)FileName+l-4;
	if (strcmpi(t,".mp3")==0) {
		return H_MSS;
	}else{
		return H_BASS;
	}
#else
	return H_MSS;
#endif
}

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
	fseekproc((unsigned)user,offset,SEEK_SET);
	return TRUE;
}

int API AllocateSampleHandle(int hDig) {
	::hDig=hDig;
	//int h=(int)AIL_allocate_sample_handle( (MSS::HDIGDRIVER)hDig );
	int h=0;
	h=AddHandle(H_MSS, h, 0);
#ifdef DEBUG
	printd("AllocateSampleHandle: %p->%d\n", hDig, h);
#endif
	return h;
}

void API CloseStream(int hStream) {
#ifdef DEBUG
	printd("CloseStream: %p\n", hStream);
#endif
	if (HandleType(hStream)==H_MSS) {
		AIL_close_stream( (MSS::HSTREAM)hStream );
	}else{
		BASS_StreamFree( hStream );
	}
}

void API GetSampleMsPosition(int hSample, long* total_ms, long* current_ms){
#ifdef DEBUG
	printd("GetSampleMsPosition: %d, total, curr\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_sample_ms_position( (MSS::HSAMPLE)FindHandle(hSample), total_ms, current_ms );
	}else{
		//*total_ms=(long)( 1000.* BASS_ChannelBytes2Seconds(hStream, BASS_ChannelGetLength(hStream, BASS_POS_BYTE)) );
		*current_ms=(long)( 1000.* BASS_ChannelBytes2Seconds(FindHandle(hSample),
			BASS_ChannelGetPosition(FindHandle(hSample), BASS_POS_BYTE)) );
	}
}

int API GetSamplePlaybackRate(int hSample) {
#ifdef DEBUG
	printd("GetSamplePlaybackRate: %d\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
		return AIL_sample_playback_rate( (MSS::HSAMPLE)FindHandle(hSample) );
	}else{
		float freq;
		BASS_ChannelGetAttribute(FindHandle(hSample), BASS_ATTRIB_FREQ, &freq);
		return (long)freq;
	}
}

unsigned int API GetSampleStatus(int hSample) {
#ifdef DEBUG
	printd("GetSampleStatus: %d\n", hSample);
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

void API GetStreamInfo(int hStream, int* datarate, int* sndtype, int* length, int* memory) {
	return;
}

void API GetStreamMsPosition(int hStream, long* total_ms, long* current_ms){
#ifdef DEBUG
	printd("GetStreamMsPosition: %p, total, curr\n", hStream);
#endif
	if (HandleType(hStream)==H_MSS) {
		AIL_stream_ms_position( (MSS::HSTREAM)hStream, total_ms, current_ms );
	}else{
		//*total_ms=(long)( 1000.* BASS_ChannelBytes2Seconds(hStream, BASS_ChannelGetLength(hStream, BASS_POS_BYTE)) );
		*current_ms=(long)( 1000.* BASS_ChannelBytes2Seconds(hStream, BASS_ChannelGetPosition(hStream, BASS_POS_BYTE)) );
	}
}

int API GetStreamPlaybackRate(int hStream) {
#ifdef DEBUG
	printd("GetStreamPlaybackRate: %p\n", hStream);
#endif
	if (HandleType(hStream)==H_MSS) {
		return AIL_stream_playback_rate( (MSS::HSTREAM)hStream );
	}else{
		float freq;
		BASS_ChannelGetAttribute(hStream, BASS_ATTRIB_FREQ, &freq);
		return (long)freq;
	}
}

int API OpenStream(int hDig, char const* FileName, int Stream_Mem) {
	int r;
#ifdef DEBUG
	printd("OpenStream: %p, %s, %d\n", hDig, FileName, Stream_Mem);
#endif
	if (FileType(FileName)==H_MSS) {
		return (int)AIL_open_stream( (MSS::HDIGDRIVER)hDig, FileName, Stream_Mem );
	}else{
		if(fopenproc==NULL || fcloseproc==NULL || fseekproc==NULL || freadproc==NULL){
			return BASS_StreamCreateFile( 0, FileName, 0, 0, 0 );
		}else{
			void *userhandle;
			BASS_FILEPROCS fileprocs={MyFcloseProc, MyFlenProc, MyFreadProc, MyFseekProc};
			r=fopenproc(FileName, (unsigned long*)&userhandle);
			if(r){
				return BASS_StreamCreateFileUser( STREAMFILE_NOBUFFER, 0, &fileprocs, userhandle );
			}else{
				return 0;
			}
		}
	}
}

void API PauseStream(int hStream) {
#ifdef DEBUG
	printd("PauseStream: %p\n", hStream);
#endif
	if (HandleType(hStream)==H_MSS) {
		AIL_pause_stream( (MSS::HSTREAM)hStream, 1 );
	}else{
		BASS_ChannelPause( hStream );
	}
}

void API ReleaseSampleHandle(int hSample) {
#ifdef DEBUG
	printd("ReleaseSampleHandle: %d\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
	   	AIL_release_sample_handle( (MSS::HSAMPLE)FindHandle(hSample) );
	}else{
		BASS_StreamFree(FindHandle(hSample));
	}
	DelHandle(hSample);
}

void API ResumeSample(int hSample) {
#ifdef DEBUG
	printd("ResumeSample: %d\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_resume_sample( (MSS::HSAMPLE)FindHandle(hSample) );
	}else{
		BASS_ChannelPlay( FindHandle(hSample), 0 );
	}
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

int API SetNamedSampleFile(int hSample, const char* file_type_suffix, const void* file_image, int file_size, int block) {
	int h,r;
//	if (file_image==lastmem){
//		//AssignHandle(hSample, lasttype, lasthandle);
//		r=1;
//	}else{
		if (FileType(file_type_suffix)==H_MSS) {
			h=(int)AIL_allocate_sample_handle( (MSS::HDIGDRIVER)::hDig );
#ifdef DEBUG
			printd("SetNamedSampleFile->AllocHandle: %p->%p\n", hDig, h);
#endif
			AssignHandle(hSample, H_MSS, h);
			r=AIL_set_named_sample_file( (MSS::HSAMPLE)h, file_type_suffix, file_image, file_size, block );
			//return AIL_set_named_sample_file( (MSS::HSAMPLE)FindHandle(hSample), file_type_suffix, file_image, file_size, block );
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
		lasthandle=h;
		lastmem=(void*)file_image;
//	}
#ifdef DEBUG
	printd("SetNamedSampleFile: %d->%p, %s, %p, %p, %d\n", hSample, FindHandle(hSample), file_type_suffix, file_image, file_size, block);
#endif
	return r;
}

int API SetPreference(unsigned int num, int value) {
#ifdef DEBUG
	printd("SetPreference: %d, %d\n", num, value);
#endif
	//AIL_set_preference(num, value);
	if(num==1){						//v76fix:
		return AIL_set_preference(num, MAX_HANDLE);	//num=1: 最大handle数量,超过就会报错
	}else{
		return AIL_set_preference(num, value);
	}
}

void API SetSampleLoopCount(int hSample, int count) {
#ifdef DEBUG
	printd("SetSampleLoopCount: %d, %d\n", hSample, count);
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

void API SetSampleMsPosition(int hSample, long ms) {
#ifdef DEBUG
	printd("SetSampleMsPosition: %d, %d\n", hSample, ms);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_set_sample_ms_position( (MSS::HSAMPLE)FindHandle(hSample), ms );
	}else{
		long bps;
		BASS_CHANNELINFO info;
		BASS_ChannelGetInfo( FindHandle(hSample), &info );
		bps=info.freq * info.chans;
		if((info.flags & BASS_SAMPLE_8BITS)==0)bps*=2;
		BASS_ChannelSetPosition( FindHandle(hSample), (QWORD)(0.001*ms*bps), BASS_POS_BYTE );
	}
}

void API SetSamplePlaybackRate(int hSample, int rate) {
#ifdef DEBUG
	printd("SetSamplePlaybackRate: %d, %d\n", hSample, rate);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_set_sample_playback_rate( (MSS::HSAMPLE)FindHandle(hSample), rate);
	}else{
		BASS_ChannelSetAttribute(FindHandle(hSample), BASS_ATTRIB_FREQ, rate);
	}
}

void API SetSampleVolume(int hSample, float left_level, float right_level) {
#ifdef DEBUG
	printd("SetSampleVolume: %d, %g, %g\n", hSample, left_level, right_level);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_set_sample_volume_levels( (MSS::HSAMPLE)FindHandle(hSample), left_level, right_level);
	}else{
		float vol=(left_level+right_level)*0.5f;
		float pan=(right_level-left_level);
		BASS_ChannelSetAttribute(FindHandle(hSample), BASS_ATTRIB_VOL, vol);
		BASS_ChannelSetAttribute(FindHandle(hSample), BASS_ATTRIB_PAN, pan);
	}
}

void API SetStreamLoopCount(int hStream, int count) {
#ifdef DEBUG
	printd("SetStreamLoopCount: %p, %d\n", hStream, count);
#endif
	if (HandleType(hStream)==H_MSS) {
		AIL_set_stream_loop_count( (MSS::HSTREAM)hStream, count );
	}else{
		if(count==0){
			BASS_ChannelFlags( hStream, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
		}else{
			BASS_ChannelFlags( hStream, 0, BASS_SAMPLE_LOOP);
		}
	}
}

void API SetStreamMsPosition(int hStream, long ms) {
#ifdef DEBUG
	printd("SetStreamMsPosition: %p, %d\n", hStream, ms);
#endif
	if (HandleType(hStream)==H_MSS) {
		AIL_set_stream_ms_position( (MSS::HSTREAM)hStream, ms );
	}else{
		long bps;
		BASS_CHANNELINFO info;
		BASS_ChannelGetInfo(hStream, &info);
		bps=info.freq * info.chans;
		if((info.flags & BASS_SAMPLE_8BITS)==0)bps*=2;
		BASS_ChannelSetPosition( hStream, (QWORD)(0.001*ms*bps), BASS_POS_BYTE );
	}
}

void API SetStreamPlaybackRate(int hStream, int rate) {
#ifdef DEBUG
	printd("SetStreamPlaybackRate: %p, %d\n", hStream, rate);
#endif
	if (HandleType(hStream)==H_MSS) {
		AIL_set_stream_playback_rate( (MSS::HSTREAM)hStream, rate);
	}else{
		BASS_ChannelSetAttribute(hStream, BASS_ATTRIB_FREQ, rate);
	}
}

void API SetStreamVolume(int hStream, float left_level, float right_level) {
#ifdef DEBUG
	printd("SetStreamVolume: %p, %g, %g\n", hStream, left_level, right_level);
#endif
	if (HandleType(hStream)==H_MSS) {
		AIL_set_stream_volume_levels( (MSS::HSTREAM)hStream, left_level, right_level);
	}else{
		float vol=(left_level+right_level)*0.5f;
		float pan=(right_level-left_level);
		BASS_ChannelSetAttribute(hStream, BASS_ATTRIB_VOL, vol);
		BASS_ChannelSetAttribute(hStream, BASS_ATTRIB_PAN, pan);
	}
}

void API Shutdown(void) {
#ifdef DEBUG
	printd("Shutdown\n");
#endif
	AIL_shutdown();
#ifdef DEBUG
	printd("MSS shut down\n");
#endif
#ifdef USEBASS
	FreeBassPlugin();
//	Win6.0+系统执行这句会挂起,原因未知
/*	BASS_Free();
	#ifdef DEBUG
		printd("BASS shut down.\n");
	#endif
*/
#endif
/*
	ExitProcess(0);
#ifdef DEBUG
	printd("Exited process.\n");
#endif
*/
}

void API StartSample(int hSample) {
#ifdef DEBUG
	printd("StartSample: %d\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_start_sample( (MSS::HSAMPLE)FindHandle(hSample) );
	}else{
		BASS_ChannelPlay( FindHandle(hSample), 1 );
	}
}

void API StartStream(int hStream) {
#ifdef DEBUG
	printd("StartStream: %p\n", hStream);
#endif
	if (HandleType(hStream)==H_MSS) {
		AIL_start_stream( (MSS::HSTREAM)hStream );
	}else{
		BASS_ChannelPlay( hStream, 1 );
	}
}

void API StopSample(int hSample) {
#ifdef DEBUG
	printd("StopSample: %d\n", hSample);
#endif
	if(FindHandleType(hSample)==H_MSS){
		AIL_stop_sample( (MSS::HSAMPLE)FindHandle(hSample) );
	}else{
		BASS_ChannelStop( FindHandle(hSample) );
	}
}



/*

int API GetStreamStatus(int hStream);
int AIL_stream_status(void* hStream);

int API ServiceStream(int hStream, int fillup);
int AIL_service_stream(void* hStream, int fillup);



*/
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

