/*/Functions used for console IO
 * Go to IO.h to have more documentation
/*///By Léolol DB

#include "IO.h"

#include <stdint.h> //To get more int control
#include <malloc.h>
#include <unistd.h>	//To get read & write
#include <stdio.h>
#include <errno.h>

#ifdef LBRThreadSafe
	#include <pthread.h>	//For threading
	pthread_mutex_t Mutex=PTHREAD_MUTEX_INITIALIZER;
#endif

typedef unsigned char u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef signed char s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

inline char* IntToStr(s64 Int){
	bool Neg=Int<0;
	if(Neg)Int=-Int;

	s64 A=Int;
	u8 Size=0;do Size++;while(A/=10);	//Gets number of digits in Int

	char* Str=(char*)malloc(Size+1+Neg);
	Str+=Size+Neg;
	*Str--=0;
	do{
		*Str--=Int%10+'0';
	}while(Int/=10);
	if(Neg)*Str--='-';

	return Str+1;
}

inline char* IntToStr(u64 Int){
	u64 A=Int;
	u8 Size=0;do Size++;while(A/=10);	//Gets number of digits in Int

	char* Str=(char*)malloc(Size+1);
	Str+=Size;
	*Str--=0;
	do{
		*Str--=Int%10+'0';
	}while(Int/=10);

	return Str+1;
}

inline u64 StrSize(const char* Str){
	if(Str==nullptr)return 0;
	const char* P=Str;
	while(*P)P++;
	return P-Str;
}
void CopyBlock(void* Block0,const void* Block1,u64 Size){//Warn! Expect Block0 to fit in size!
	u8* P0=(u8*)Block0;
	const u8* P1=(const u8*)Block1;
	while(Size--)*P0++=*P1++;
}


u32 CBufMSize=1024;
char* CharBuffer=(char*)malloc(CBufMSize);
u32 CBufSize=0;
inline void Write(const void* Buffer,u64 Size){
	if(Size){
		#ifdef LBRThreadSafe
			pthread_mutex_lock(&Mutex);
		#endif
		if(CBufMSize<Size+CBufSize){
			CBufMSize=Size+CBufSize;
			CBufMSize=(CBufMSize+1023)/1024*1024;
			CharBuffer=(char*)realloc(CharBuffer,CBufMSize);
		}
		CopyBlock(CharBuffer+CBufSize,Buffer,Size);
		CBufSize+=Size;
		#ifdef LBRThreadSafe
			pthread_mutex_unlock(&Mutex);
		#endif
	}
}


char* InputLn(){
	u64 StrSize=0;
	char* Str=nullptr;
	while(1){
		Str=(char*)realloc(Str,StrSize+1);
		if(read(0,Str+StrSize,1)==-1){
			if(errno!=EAGAIN)break;
		}else if(Str[StrSize]=='\n'){
			break;
		}else StrSize++;
	}
	Str[StrSize]=0;
	return Str;
}


void Print(const char* Str){
	Write(Str,StrSize(Str));
}
void Print(char Char){
	Write(&Char,1);
}
void PrintHex(u64 Int){
	u64 A=Int;
	u8 Size=0;do Size++;while(A>>=4);	//Gets number of digits in Int

	char* Str=(char*)malloc(Size+1);
	Str+=Size;
	*Str--=0;
	do{
		*Str--=(Int&0xF)<10?(Int&0xF)+'0':(Int&0xF)-10+'A';
	}while(Int>>=4);

	Str++;
	Print(Str);
	free(Str);
}
void Print(u64 Int){
	char* Str=IntToStr(Int);
	Print(Str);
	free(Str);
}
void Print(s64 Int){
	char* Str=IntToStr(Int);
	Print(Str);
	free(Str);
}

bool FPrint(){
	if(CBufSize){
		u64 WSize=0;
		
		u8 TimeOut=1;
		do{//v Makes sure to write everything
			s32 Ret=write(1,CharBuffer+WSize,CBufSize-WSize);
			if(Ret>0)WSize+=Ret;
			else if(!Ret){
				fflush(stdout);//stdout≈1
				fdatasync(1);
				usleep(256);
			}else{
				CBufSize=0;
				return false;
			}
		}while(WSize<CBufSize&&TimeOut++);
		fflush(stdout);
		fdatasync(1);
		if(WSize<CBufSize)CopyBlock(CharBuffer,CharBuffer+WSize,CBufSize-WSize);
		CBufSize-=WSize;
	}
	return true;
}

char* FPrint(u32* Size){
	if(CBufSize){
		*Size=CBufSize;
		char* Buff=(char*)realloc(CharBuffer,CBufSize);

		CharBuffer=nullptr;
		CBufMSize=0;
		CBufSize=0;
		return Buff;
	}
	Size=0;
	return nullptr;
}


void PrintLn(){
	Print('\n');
	FPrint();
}
void PrintSp(){
	Print(' ');
}

void ColourTxt(u8 R,u8 G,u8 B){
	Print("\x1B[38;2;");
	Print(R);Print(';');
	Print(G);Print(';');
	Print(B);
	Print('m');
}
void ColourBG(u8 R,u8 G,u8 B){
	Print("\x1B[48;2;");
	Print(R);Print(';');
	Print(G);Print(';');
	Print(B);
	Print('m');
}
