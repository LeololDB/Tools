/*/Functions used to browse files
 * See all comments to know what function does what or guess yourself! :P
 *
 *	-=[ Library dependencies ]=-
 * This library uses ALSA, please link the library with your compiler using
 * "-lasoudlib" or "-llibasoud" and maybe run the command
 * "sudo apt-get install libasound2-dev"
 *
 *	-=[ Platforms ]=-
 * This library isn't cross-platform yet.
 *
 *
 *	-=[ License ]=-
 * This library is under the terms of the Lebray Open-Source License:
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * If you have used the code in one of your project, please credit, that would be nice!
 *
 *	-=[ Credits ]=-
 * All of the library's code is provided by Léolol DB.
 *
 * Version 0.0.0.1
 * In development since the 6/20/03/2021
/*///By Léolol DB

#ifndef LBRSoundH
#define LBRSoundH

#include "Maths.h"	//For better type support
#include <alsa/asoundlib.h>//Please add "-lasoudlib" or "-llibasoud" and maybe run "sudo apt-get install libasound2-dev"


namespace LBR{
	class Sound{
		public:
			Sound();

			bool SetupOutput(const char* Device,u32 SplRate,u8 Channels,u32 Latency,u8 Format);
			bool SetupInput(const char* Device,u32 SplRate,u8 Channels,u32 Latency,u8 Format);

			bool PlaySound(void* Samples,u32 SplFrames);
			bool CaptureSound(void* Samples,u32 SplFrames);

			bool Close();
			bool WaitEndClose();

			const char* Device=nullptr;	//Name of the device linked with

			u32 SplRate;		//SplRate in Hz
			u32 Latency;		//Target latency in µs
			u8 Channels;		//Numbers of channels

			u8 Format;			//Used sound format

			bool Setup=0;		//True on successful setup
		private:
			snd_pcm_t* Handle;	//A structure required by ALSA
	};
}

#endif
