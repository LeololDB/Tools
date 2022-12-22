/*/
 * Functions used to make mathematical operations and some others common operations
 * See all comments to know what function does what or guess yourself! :P
 *
 *  -=> Operating System <=-
 * This library is cross-platform!
 *
 *  -=> Source Code <=-
 * All of the library's code is provided by Léolol DB.
 * If you have used the code in one of your project, please credit, that would be nice!
 *
 *  -=> License <=-
 * This library is under the terms of the Lebray Open-Source License:
 *
 * This program is distributed WITHOUT ANY WARRANTY, not even the
 * warranty for the program to work with any fitness of any purpose.
 * Not any responsabilities are taken for any possible damage being done.
 *
 * Please credit
 *
 *  -=> Credits and Version <=-
 * Version 0.0.0.1
 * In development since the 2/10/03/2020 (18331)
/*///By Léolol DB

#ifndef LBRMaths
#define LBRMaths

#include <cstdint> //To get more int control

#define MacroMax(A,B) ((A)>(B)?(A):(B))
#define MacroMiMax(Min,X,Max) ((Min)<(X)?((X)>(Max)?(Max):(X)):(Min))
#define MacroMiNax(Min,X,Nax) ((Min)<(X)?((X)>=(Nax)?(Nax)-1:(X)):(Min))
#define MacroMin(A,B) ((A)<(B)?(A):(B))
#define MacroDiff(A,B) ((A)>(B)?(A)-(B):(B)-(A))
#define MacroAbs(A) ((A)<0?(-A):(A))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef char c8;
typedef const char cc8;

typedef u8 byte;

bool ArrayObjectGet(void* Array,u32 ArraySize,u8* Object,u8* ObjectSize,u8 ObjectPos);
bool ArrayObjectSet(void* Array,u32 ArraySize,u8* Object,u8 ObjectSize,u8 ObjectPos); //Assumes which the ArraySize is the allocated Size of Array

bool IsHittingBox(u16 HBXPos,u16 HBYPos,u16 HBXSize,u16 HBYSize,
		u16 OXPos,u16 OYPos);//Checks if Object (O) touches HitBox (HB)

//v Gets bit of Number by his pos
bool GetBit(u8 Number,u8 Pos);
bool GetBit(void* Number,u8 Pos);
//v Sets bit of Number by his pos
u8 SetBit(u8 Number,u8 Pos,bool Bit);
u8 SetBit(void* Number,u8 Pos,bool Bit);

u8 GetSUInt(u8 Number,u8 Size,u8 Pos); //Gets a uint with a uncommon binary size

u8 SetSUInt(u8 Number,u8 Size,u8 Pos,u8 SUInt); //Sets a uint of an uncommon binary size


//v After 6/06/03/2020

namespace LBR{
	u8 Decimals(uint64_t Number);
}

s64 StrToInt(const char* Str);
char* IntToStr(u64 Int);	//I would favor that one over the following
char* SIntToStr(s64 Int);	//~Oh that ugly function!~
char* IntToStr(u64 Int,char Separator);
//char* SIntToStr(s64 Int,char Separator);	//~Who needs that?~


void* CopyBlock(const void* Block,u64 Size);

inline u64 FracToInt(u64 A,u64 B,u64 T){//return A/B;
	return A*(T+1)/B-A*T/B;
}
#endif
