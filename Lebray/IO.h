/*/Functions used to browse files
 * See all comments to know what function does what or guess yourself! :P
 *
 *  -=[ Library Dependencies ]=-
 * This library only uses Lebray/Maths.h prvided by Léolol DB
 *
 *	-=[ Platforms ]=-
 * This library is cross-platform.
 *
 *	-=[ License ]=-
 * This library is under the terms of the Lebray Open-Source License:
 *
 * This program is distributed WITHOUT ANY WARRANTY, not even the
 * warranty for the program to work with any fitness of any purpose.
 * Not any responsabilities are taken for any possible damage being done.
 *
 * Please credit
 *
 *	-=[ Credits ]=-
 * All of the library's code is provided by Léolol DB.
 *
 * Version 0.0.0.1
 * In development since the 1/12/04/2021
/*///By Léolol DB

#ifndef LBRIOLib
#define LBRIOLib

#define PrintAllK(Type,Var)\
Print(Type Var);\
inline void PrintLn(Type Var){\
	Print(Var);\
	PrintLn();\
}\
inline void PrintSp(Type Var){\
	Print(Var);\
	PrintSp();\
}\
inline bool FPrint(Type Var){\
	Print(Var);\
	return FPrint();\
}\
inline bool FPrintLn(Type Var){\
	Print(Var);\
	PrintLn();\
	return FPrint();\
}

#define PrintIntK(Type0,Type1)\
inline void Print(Type0 Type1 Int){\
	Print((Type0 long)Int);\
}\
inline void PrintLn(Type0 Type1 Int){\
	Print((Type0 long)Int);\
	PrintLn();\
}\
inline void PrintSp(Type0 Type1 Int){\
	Print((Type0 long)Int);\
	PrintSp();\
}\
inline bool FPrint(Type0 Type1 Int){\
	Print((Type0 long)Int);\
	return FPrint();\
}\
inline bool FPrintLn(Type0 Type1 Int){\
	Print((Type0 long)Int);\
	PrintLn();\
	return FPrint();\
}


#define PrintInt(Type)\
PrintIntK(,Type)\
PrintIntK(unsigned,Type)

void PrintLn();
void PrintSp();
bool FPrint();//To Flush Print

void PrintAllK(const char*,Str)
void PrintAllK(char,Char)
void PrintAllK(unsigned long,Int)	//u64
void PrintAllK(long,Int)			//s64


PrintInt(int)				//int32
PrintInt(short)				//int16
PrintIntK(unsigned,char)	//u8

void PrintHex(unsigned long Int);
inline void PrintHexSp(unsigned long Int){
	PrintHex(Int);
	PrintSp();
}
inline void PrintHexLn(unsigned long Int){
	PrintHex(Int);
	PrintLn();
}

char* InputLn();

#undef PrintInt
#undef PrintLnInt
#undef PrintAllK
#undef PrintIntK

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int

void ColourTxt(u8 R,u8 G,u8 B);
inline void ColourTxt(u32 Colour){
	ColourTxt(Colour/0x10000,Colour/0x100,Colour);
}
inline void Colour12bTxt(u16 Colour){
	ColourTxt(((Colour/0x100)&0xF)*0x11,((Colour/0x10)&0xF)*0x11,(Colour&0xF)*0x11);
}
inline void GrayTxt(u8 Gray){
	ColourTxt(Gray,Gray,Gray);
}

void ColourBG(u8 R,u8 G,u8 B);
inline void ColourBG(u32 Colour){
	ColourBG(Colour/0x10000,Colour/0x100,Colour);
}
inline void Colour12bBG(u16 Colour){
	ColourBG(((Colour/0x100)&0xF)*0x11,((Colour/0x10)&0xF)*0x11,(Colour&0xF)*0x11);
}
inline void GrayBG(u8 Gray){
	ColourBG(Gray,Gray,Gray);
}

inline void ColourTxt(u32 TxtColour,u32 BGColour){
	ColourTxt(TxtColour);
	ColourBG(BGColour);
}
inline void Colour12bTxt(u16 TxtColour,u16 BGColour){
	Colour12bTxt(TxtColour);
	Colour12bBG(BGColour);
}
inline void GrayTxt(u8 TxtGray,u8 BGGray){
	GrayTxt(TxtGray);
	GrayBG(BGGray);
}

char* FPrint(u32* Size);

#undef u8
#undef u16
#undef u32

#endif
