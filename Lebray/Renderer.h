/*/████████████████████████████████████████████████████████████████████████████▒
 * ██                                                                        ██▒
 * ██ ██▒                                                               ██▒  ██▒
 * ██                                                                        ██▒
 * ██             ██████▒   ██████▒ ██▒   ██▒ ████████▒ ██▒                  ██▒
 * ██             ██▒   ██▒   ██▒   ██▒   ██▒ ██▒       ██▒                  ██▒
 * ██             ██████▒     ██▒     ████▒   ██████▒   ██▒                  ██▒
 * ██             ██▒         ██▒   ██▒   ██▒ ██▒       ██▒                  ██▒
 * ██             ██▒       ██████▒ ██▒   ██▒ ████████▒ ████████▒            ██▒
 * ██                                                                        ██▒
 * ██     ████████▒ ██▒     ██▒   ██████▒ ██████▒ ██▒     ██▒ ████████▒      ██▒
 * ██     ██▒       ████▒   ██▒ ██▒         ██▒   ████▒   ██▒ ██▒            ██▒
 * ██     ██████▒   ██▒ ██▒ ██▒ ██▒ ████▒   ██▒   ██▒ ██▒ ██▒ ██████▒        ██▒
 * ██     ██▒       ██▒   ████▒ ██▒   ██▒   ██▒   ██▒   ████▒ ██▒            ██▒
 * ██     ████████▒ ██▒     ██▒   ██████▒ ██████▒ ██▒     ██▒ ████████▒      ██▒
 * ██                                                                        ██▒
 * ██ ██▒                                                               ██▒  ██▒
 * ██                                                                        ██▒
 * ████████████████████████████████████████████████████████████████████████████▒
 *
 * Functions used to make graphical stuff
 * See all comments to know what function does what or guess yourself! :P
 *
 *  -=> Requirements <=-
 * PixelEngine uses X11 with Xlib as windowing system.
 * However, PixelEngine is designed to work in a sort of compatibility
 * mode that only uses the standard IO if necessary. It is advised to
 * only use that mode as last option as it may not give the best user
 * experience.
 *
 *  -=> Libraries <=-
 * The library is made using Xlib.
 *
 *  -=> License <=-
 * This library is under the terms of the Lebray Open-Source License No Warranty:
 *
 * This program is distributed WITHOUT ANY WARRANTY, not even the
 * warranty for the program to work with any fitness of any purpose.
 * Not any responsabilities are taken for any possible damage being done.
 *
 *	-=> Credits <=-
 * All of the library's code is provided by Léolol DB.
 * If you have used the code in one of your project,
 * please credit, that would be nice!
 *
 *  -=> Version Log <=-
 *
 *	Version 5 - Phoenix Powder
 *
 * Version 5.1.2.25:
 * - Renderer is renamed PixelEngine
 * - The ArtWork is updated
 * - Replaced most libraries
 *
 * Version 5.1.4.5:
 *	- Splitted the PixelEngine class:
 *		- Added EventEngine for general Event handling
 *		- Added IOEngine for IO Event handling
 *		- Added Frame class for BitMap handling
 *	- Removed a lot of old and redundant functions from PixelEngine
 *
 * Version 5.1.4.8:
 *	- Corrected an error with the version
 *	- Added TextEngine
 *		- Added LBR::TextEngine::GenerateEvents()
 *			- Heavy stdin handler
 *			- Catches single characters without echo
 *			- Handles UTF-8 encoding
 *			- Handles some ESCape sequences
 *
 * Version 5.1.4.13:
 *	- Added many functions in LBR::Frame
 *		- LBR::Frame::DrawRectangle()
 *		- LBR::Frame::DrawText()
 *		- LBR::Frame::Everything.lol
 *
 * Version 5.1.4.15:
 *	- Added a terminal emulation connecting LBR::TextEngine to LBR::PixelEngine
 *		- LBR::TextEngine::ConnectWindow()
 *
 * Version 5.1.4.17:
 *	- Added a some support for Unicode on LBR::IOEngine::NextEvent();
 *
 * Version 5.1.4.19:
 *	- Added the LBR::TextEngine::Print() functions
 *	- Added a private LBR::TextEngine::_TxtBuf
 *	- Added LBR::TextEngine::GetPrint()
 *		- Allows direct write to the text buffer
 *		- No need to write twice
 *	- Added a conversion from LBR::PixelEngine to LBR::TextEngine
 *
 * Version 5.1.8.1:
 *  - Added native VGF1 support
 *	- Dropped old font support
 *
 * In development since the 6/25/01/2020 (18286)
/*///(C) By Léolol DB 2021 LOSL-NW

#ifndef LBRPixelEngineVersion
#define LBRPixelEngineVersion 514 //In Days

/*/Are you thinking that: "There is too much Macros!"?
 * Be reassured, I try to use Macros only in the right place!
 * Sadly, we need a lot of them.
 * I like that after forgetting my code I can understand it again.
 * -> Scroll down, don't look at macros, they aren't mandatory.
 *
/*///v Allowed rendering method: Remove one to force the usage of the other.
#if defined(__linux__)||defined(__unix__)||defined(_POSIX_VERSION)
	//#define LBRPxEngineUseX11 1	//Allows GUI (Highest priority)
	#define LBRPxEngineUseTUI 1	//Allows TUI
#endif//No rendering won't crash but won't show anything

#define LBRKeyPress		1
#define LBRKeyRelease	2
#define LBRKeyChange	3
#define LBRButtonPress		4
#define LBRButtonRelease	8
#define LBRButtonChange		12
#define LBRAnyButtonChange	15
#define LBRPointerMotion	16
#define LBRWindowRestructurate	32
#define LBRWindowExpose			64

#if true	//< Put "false" if you hate having too much libraries
	#include <cstdint> //To get more int control

	#define u8 uint8_t
	#define u16 uint16_t
	#define u32 uint32_t
	#define u64 uint64_t
	#define s8 int8_t
	#define s16 int16_t
	#define s32 int32_t
	#define s64 int64_t
#else
	#define u8 unsigned char
	#define u16 unsigned short
	#define u32 unsigned int
	#define u64 unsigned long
	#define s8 signed char
	#define s16 signed short
	#define s32 signed int
	#define s64 signed long
#endif

//v All simple structures, mainly 16-bit
namespace LBR{
	struct KeyEvent{
		bool State;
		u8 Code;
		u32 Unicode;
	};
	struct PointerEvent{
		bool BtnState;
		u8 Button;
		s16 XPos;
		s16 YPos;
	};
	struct WindowEvent{
		s16 XPos;
		s16 YPos;
		s16 XSize;//Should really be signed?
		s16 YSize;
	};
	struct Event{
		u8 Type;
		KeyEvent Key;
		PointerEvent Pointer;
		WindowEvent Window;
		u64 Time;
	};
}

#ifdef LBRPxEngineUseX11
/*/ PxEngine using X11:
 * X11 is a simple windowing system to use,
 * it is recommanded for pixel-perfect graphics!
 *
 *	-> Troubleshooting:
 * If you experience any issues here with X11, please check the followings:
 * - Check that you're in the right OS with X11 as windowing system
 * - Add -lX11 flag to your compiler to link the library
 * - Install X11 $sudo apt install libx11-dev
 *
 * If none of that worked or if you're on an exotic OS:
 * - In the top of that file: comment LBRRendererUseGUI to use TUI instead
 * - Or contact Léolol DB#4584 on Discord
/*/
#include <X11/Xlib.h>	//^ Read above! ^(^-^ )^
#define IOProtected		\
	Cursor _Cursor;		\
	Display* _Display;	\
	Window _Window;		\
	GC _GraphicCTX;		\
	u8 _ModKeys
#else
	#define IOProtected
#endif

namespace LBR{
	/* A grid
	 *
	 *
	struct Grid{
		Grid(u32 XSize,u32 YSize);

		void* ToBitMap(u16 ColourPalette,u8 Padding);
		void* ToBitMap(u32 Tile,u16 ColourPalette,u8 Padding);
		bool PutOnBitMap(
			u32 Tile,u32 XPos,u32 YPos,
			void* BitMap,u32 XSize,u32 YSize,
			u16 ColourPalette,u8 Padding
		);

		void* Map;			//The TileMap or BitMap
		LBR::Grid* Tiles;	//The Tiles for a TileMap

		u16 ColourPalette;	//Can be either tiled or bitmapped
		u8 Padding;			//The Padding (redundant), about the size in bits of a Tile or Pixel

		u32 XSize;	//In Tiles or Pixel
		u32 YSize;
		u32 ZSize;	//If Grid was used as a Tile array, how many Tiles is there?
	};*/

	class EventEngine{
		public:
			//EventEngine();//Setup

			//v Events handling
			bool SelectEvents(u8 Events);

			bool GenerateEvent(LBR::Event &Event);
			u16 PendingEventsNumber();
			LBR::Event NextEvent();
		protected:
			u8 _EventSel=0;
			u16 _EventNum=0;
			LBR::Event* _Events=nullptr;
	};

	class IOEngine:public EventEngine{
		public:
			IOEngine();//Setup

			//v Events handling
			bool SelectEvents(u8 Events);

			u16 GenerateEvents();
			u16 PendingEventsNumber();
			LBR::Event NextEvent();

			bool ConnectWindow(u32 XSize,u32 YSize);

			bool WinConnected;
		protected:
			IOProtected;
	};

	class Frame{
		public:
			Frame();					//Setup
			Frame(u32 XSize,u32 YSize);	//Setup

			bool SetPixel(u32 X,u32 Y,u32 Colour);
			u32 GetPixel(u32 X,u32 Y);

			bool DrawLine(s32 X0,s32 Y0,s32 X1,s32 Y1,u32 Colour);//Draws a line
			bool DrawXLine(u32 YPos,u32 Colour);
			bool DrawXLine(s32 X0,s32 X1,u32 YPos,u32 Colour);
			bool DrawYLine(u32 XPos,u32 Colour);
			bool DrawYLine(u32 XPos,s32 Y0,s32 Y1,u32 Colour);

			bool DrawRectangle(s32 XPos,s32 YPos,u32 XSize,u32 YSize,u32 Colour);//Draws a rectangle outline
			bool DrawFullRectangle(s32 XPos,s32 YPos,u32 XSize,u32 YSize,u32 Colour);//Draws a full rectangle
			inline bool DrawFullSquare(s32 XPos,s32 YPos,u32 Size,u32 Colour);


			bool DrawBitMap(
				void* BitMap,u32 BMXSize,u32 BMYSize,
				s32 XPos,s32 YPos
			);
			bool DrawBitMap(
				void* BitMap,
				u32 BMXSize,u32 BMYSize,
				s32 XPos,s32 YPos,
				u32 XSize,u32 YSize,
				s32 WinXPos,s32 WinYPos,
				u32 WinXSize,u32 WinYSize
			);

			bool UseTextFont(void* TextFont,u16 Size);
			bool UseTextFontFile(const char* FilePath);

			bool GetTextSize(const char* Text,u32* IMGXSize,u32* Lines);
			u16 GetTextFontSize(u16 TFont);//X in LS Nybble & Y in MS Nybble

			bool DrawText(
				const char* Text,
				s32 XPos,s32 YPos,
				u32 TextColor,u32 BackColor,
				u8 YSize,u8 Resize
			);
			bool DrawChar(
				u32 Char,
				s32 XPos,s32 YPos,
				u32 TextColour,u32 BackColour,
				u8 YSize,u8 Resize
			);
			bool DrawChar(
				u32 Char,
				s32 XPos,s32 YPos,
				u32 TextColour,u32 BackColour,
				u8 XSize,u8 YSize,u8 Resize
			);

			bool Clear();	//Rubouts everything with black
			bool Render();	//Renders everything

			bool Resize(u32 XSize,u32 YSize);//Changes the size

			bool Test();

			bool Propagate(bool Shade,u8 Size);

			void* ToBitMap(u16 ColourPalette,u8 Padding);
			void* Convert(u64 Quality);


			u32 XSize;
			u32 YSize;

			u16 ColourPalette;
			u8 Padding;

			operator void*(){
				return BitMap;
			}
			operator char*(){
				return (char*)BitMap;
			}

			//void* LinkTable;

			void* BitMap;
			u64 BitMapSize;	//In Bytes
		private:
			//*
			void*** _TextFont;
			u16 _TFCount;
			/*/
			void* _TextFont;
			u16 _TFSize;
			//*/
	};

	class PixelEngine:public IOEngine,public Frame{
		public:
			PixelEngine(u32 XSize,u32 YSize);//Setup

			bool Render();	//Renders everything

			bool SetCursorShape(u32 Shape);

			bool UpdateOnEvent(LBR::Event Event);

			/*operator LBR::TextEngine&(){
				LBR::TextEngine Text(*this);
				return Text;
			}*/

			u16 PtrXPos;
			u16 PtrYPos;
		/*private:
			LBR::TextEngine* _Text;*/
	};

	class TextEngine:public EventEngine{
		public:
			TextEngine();							//Setup
			TextEngine(LBR::PixelEngine &Pixel);	//Setup with Pixel

			TextEngine(const LBR::TextEngine &Text);		//Copy
			~TextEngine();									//Restore and clean
			/*TextEngine& operator =(const TextEngine &Text){	//Assignement


			}*/

			void ClassPrepare();	//Reset the contents. Use once, not twice! The setups already call it!


			bool ConnectWindow(u32 XSize,u32 YSize);

			bool SelectEvents(u8 Events);

			u16 GenerateEvents();
			u16 PendingEventsNumber();
			LBR::Event NextEvent();

			bool UpdateOnEvent(LBR::Event Event);

			bool Clear();
			bool Render();

			void Print(const void* Block,u32 Size);
			void Print(const char* Str);
			void Print(char Char);
			void Print(s64 Num);
			void Print(u64 Num);
			void Print(s32 Num);
			void Print(u32 Num);
			void Print(s16 Num);
			void Print(u16 Num);
			void Print(u8 Num);

			void ColourTxt(u32 TxtColour,u32 BGColour);
			void Colour12bTxt(u16 TxtColour,u16 BGColour);
			void Colour12bTxt(u16 Colour);
			void Colour12bBG(u16 Colour);
			void GrayTxt(u8 TxtGray,u8 BGGray);
			void GrayTxt(u8 Gray);
			void GrayBG(u8 Gray);

			u16 YSize;
			u16 XSize;
		private:
			u8* GetPrint(u32 Size);//Allows to write directly into the buffer
			void ColourTxt(u8 R,u8 G,u8 B);//Optimise colour but doesn't put full ESC!

			LBR::PixelEngine* _Pixel;

			void* _ScreenBuf;

			u8* _TxtBuf;
			u32 _TxtBufSize;
			u32 _TxtBufMSize;

			u8* _InBuf;
			u32 _InBufSize;
			u32 _InBufMSize;

			u32 _Pos;
			u32 _TxtClr;
			u32 _BakClr;
			u8 _Modif;
	};

	bool Frame::DrawFullSquare(s32 XPos,s32 YPos,u32 Size,u32 Colour){
		return DrawFullRectangle(XPos,YPos,Size,Size,Colour);
	}
}

//v We don't want them outside!
#undef u8
#undef u16
#undef u32
#undef u64
#undef s8
#undef s16
#undef s32
#undef s64
#undef IOProtected

#endif
