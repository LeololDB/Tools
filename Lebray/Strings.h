/*/Functions used to browse files
 * See all comments to know what function does what or guess yourself! :P
 *
 *  -=[ Library Dependencies ]=-
 * Name				Version		Provider
 * Lebray/Maths.h	0.1.1.1		Léolol DB
 * malloc.h						cstdlib
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

#ifndef LBRStrings
#define LBRStrings

#define u64 unsigned long

u64 StrSize(const char* Str);		//In bytes
u64 StrSizeUTF8(const char* Str);	//In UTF8 chars

char* StrCopy(const char* Str);				//Clones Str

bool StrAdd(char* &Str0,const char* Str1);
inline bool StrAdd(char* &Str0,const char Char);
inline char* StrAdd(const char* Str0,const char* Str1);

bool StrEqual(const char* Str0,const char* Str1);
u64 StrDiff(const char* Str0,const char* Str1);	//Counts how many chars are different from the two
u64 StrMatch(const char* Str,const char Match);	//Gives the first pos at wich Match is found in Str
void ToUpper(char* Str);

//v Functions synonyms

inline char* StrAdd(const char* Str0,const char* Str1){
	char* Str=StrCopy(Str0);
	StrAdd(Str,Str1);
	return Str;
}

inline bool StrAdd(char* &Str,const char Char){
	const char CStr[2]={Char,0};
	return StrAdd(Str,CStr);
}

#undef u64
#endif
