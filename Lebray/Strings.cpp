/*/Functions used for strings
 * Go to Strings.h to have more documentation
/*///By Léolol DB

#include "Strings.h"
#include "Maths.h"
#include <malloc.h>

char* StrCopy(const char* Str){					//Clones Str
	char* StrC=(char*)malloc(StrSize(Str)+1);	//+1 because we still have a 0 to add
	if(StrC==nullptr)return StrC;				//May error out if out of RAM

	char* P=StrC;
	do *P++=*Str;while(*Str++);	//Copies Str to P$
	return StrC;				//Returns true if everything right
}

bool StrAdd(char* &Str0,const char* Str1){
	char* P=Str0;
	u64 Size=0;
	if(Str0!=nullptr){
		while(*P)P++;
		Size=P-Str0;
	}

	Str0=(char*)realloc(Str0,Size+StrSize(Str1)+1);	//+1 because we still have a 0 to add
	if(Str0==nullptr)return 0;							//May error out if out of RAM
	P=Str0+Size;

	do *P++=*Str1;while(*Str1++);	//Copies Str1 to P

	return 1;					//Returns true if everything right
}

u64 StrSize(const char* Str){
	if(Str==nullptr)return 0;
	const char* P=Str;
	while(*P)P++;
	return P-Str;
}

u64 StrSizeUTF8(const char* Str){
	if(Str==nullptr)return 0;
	const char* P=Str;
	u64 Size=0;
	while(*P){
		if(*P&0x80){
			if(*P&0x40)Size++;
		}else Size++;
		P++;
	}
	return Size;
}

u64 StrDiff(const char* Str0,const char* Str1){	//Counts how many chars are different from the two
	u64 Diff=0;
	do{
		Diff+=*Str0!=*Str1;
	}while(*Str0++&&*Str1++);

	Str0--;Str1--;
	if(*Str0){
		while(*Str0++)Diff++;
	}else if(*Str1){
		while(*Str1++)Diff++;
	}

	return Diff;
}
bool StrEqual(const char* Str0,const char* Str1){
	do{
		if(*Str0!=*Str1++)return 0;
	}while(*Str0++);

	return 1;
}
u64 StrMatch(const char* Str,const char Match){ //Gives the first pos at wich Match is found in Str
	const char* P=Str;
	do if(*P==Match)return P-Str;while(*P++);
	return -1;
}
void ToUpper(char* Str){
	for(char* P=Str;*P;P++)if(*P>='a'&&'z'>=*P)*P-=32;
}
