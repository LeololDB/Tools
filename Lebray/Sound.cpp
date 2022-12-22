/*/Functions used for sound
 * Go to Sound.h to have more documentation
/*///By Léolol DB

#include "Sound.h"
#include "IO.h"

#define MacroWrap(Instructions) do{Instructions}while(0)

#define SndErrCheck(ErrStr)MacroWrap(\
if(Error<0){\
	Print(ErrStr);\
	PrintSp(':');\
	PrintLn(snd_strerror(Error));\
	return 0;\
})

LBR::Sound::Sound():Setup(0){}

bool LBR::Sound::SetupOutput(cc8* Dev,u32 Rate,u8 Channels_,u32 Latency_,u8 Format_){
	Device=Dev;
	SplRate=Rate;
	Latency=Latency_;
	Channels=Channels_;
	Format=Format_;

	s32 Error=snd_pcm_open(&Handle,Device,SND_PCM_STREAM_PLAYBACK,SND_PCM_NO_AUTO_RESAMPLE);
	SndErrCheck("Failed opening audio device");//\"",Device,'"');

	snd_pcm_format_t AlsaFormat;

	if(Format==0)AlsaFormat=SND_PCM_FORMAT_U8;
	else if(Format==1)AlsaFormat=SND_PCM_FORMAT_S8;
	else if(Format==2)AlsaFormat=SND_PCM_FORMAT_U16;
	else if(Format==3)AlsaFormat=SND_PCM_FORMAT_S16;
	else{
		PrintSp("Unknown format number");
		PrintLn(Format);
		return 0;
	}

	Error=snd_pcm_set_params(Handle,AlsaFormat,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		Channels,SplRate,0,Latency
	);
	SndErrCheck("Sound Output parameters Error");

	Error=snd_pcm_prepare(Handle);
	SndErrCheck("Audio interface preparation Failed");

	Setup=1;
	return 1;
}

bool LBR::Sound::SetupInput(cc8* Dev,u32 Rate,u8 Channels_,u32 Latency_,u8 Format_){
	Device=Dev;
	SplRate=Rate;
	Latency=Latency_;
	Channels=Channels_;
	Format=Format_;

	s32 Error=snd_pcm_open(&Handle,Device,SND_PCM_STREAM_CAPTURE,SND_PCM_NO_AUTO_RESAMPLE);
	SndErrCheck("Failed opening audio device");//\"",Device,'"');

	snd_pcm_format_t AlsaFormat;

	if(Format==0)AlsaFormat=SND_PCM_FORMAT_U8;
	else if(Format==1)AlsaFormat=SND_PCM_FORMAT_S8;
	else if(Format==2)AlsaFormat=SND_PCM_FORMAT_U16;
	else if(Format==3)AlsaFormat=SND_PCM_FORMAT_S16;
	else{
		PrintSp("Unknown format number");
		PrintLn(Format);
		return 0;
	}

	Error=snd_pcm_set_params(Handle,AlsaFormat,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		Channels,SplRate,0,Latency
	);
	SndErrCheck("Sound Input parameters Error");

	Error=snd_pcm_prepare(Handle);
	SndErrCheck("Audio interface preparation Failed");

	Setup=1;
	return 1;
}

bool LBR::Sound::PlaySound(void* Samples,u32 SplFrames){
	if(!Setup&&SplFrames)return 0;
	while(SplFrames){
		//int Err=snd_pcm_avail(Handle);
		s64 Ret=snd_pcm_writei(Handle,Samples,SplFrames);
		if(Ret==SplFrames)return 1;//Gud
		else if(Ret==-EAGAIN){
			PrintSp("«Try again» they say.");
			snd_pcm_wait(Handle,(Latency+999)/1000);//Wait so PCM buffer is available
		}else if(Ret>=0&&SplFrames>Ret){
			Samples=(u8*)Samples+Ret;
			SplFrames-=Ret;

			PrintSp("Short write (left");
			PrintSp(SplFrames);
			PrintLn("samples)");

			snd_pcm_wait(Handle,(Latency+999)/1000);//Wait so PCM buffer is available
		}else if(Ret==-EPIPE){
			PrintLn("Underrun occured.");

			snd_pcm_status_t* Stat;
			snd_pcm_status_alloca(&Stat);

			if(snd_pcm_status(Handle,Stat)<0){
				PrintLn("Stoopidity ALSA channel status error.");
				return 0;
			}else if(snd_pcm_status_get_state(Stat)==SND_PCM_STATE_XRUN){
				if(snd_pcm_prepare(Handle)<0){
					PrintLn("Stoopid ALSA couldn't resolve.");
					return 0;
				}
			}else if(snd_pcm_status_get_state(Stat)==SND_PCM_STATE_DRAINING){
				PrintLn("ALSA is Draining.");
			}else{
				PrintLn("Unexpected ALSA channel status.");
				return 0;
			}
		}else if(Ret==-ESTRPIPE){
			PrintLn("ALSA channel is suspended.");
			return 0;
		}else{
			PrintLn("Unexpected error on function snd_pcm_writei.");
			return 0;
		}
	}

	return 0;
}

bool LBR::Sound::CaptureSound(void* Samples,u32 SplFrames){
	if(SplFrames){
		if(!Setup)return 0;

		s64 Error=snd_pcm_readi(Handle,Samples,SplFrames);
		if(Error!=SplFrames){
			PrintSp("Read from audio interface failed:");
			PrintLn(snd_strerror(Error));
			return 0;
		}
	}
	return 1;
}

bool LBR::Sound::Close(){
	Setup=0;
	snd_pcm_close(Handle);
	return 1;
}

bool LBR::Sound::WaitEndClose(){
	Setup=0;
	int Error=snd_pcm_drain(Handle);		//Waits for end
	snd_pcm_close(Handle);					//Close
	SndErrCheck("Function snd_pcm_drain failed");
	return 1;
}

#undef SndErrCheck
