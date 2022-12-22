/*/Functions used to debug especially segmentation faults
 * Go to SegFault.h to have more documentation
/*///By Léolol DB

#include "SegFault.h"
#include "IO.h"
#include <stdlib.h>
#include <signal.h>

typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

namespace LBR{
	namespace DBG{
		u64 Line=0;
		u64 Level=0;
		bool Enabled=false;
		char*** FunCall=nullptr;
	}
}

LBR::DBG::Tracker::Tracker(const char* Func,...){
	if(!Enabled)return;
	const char* P0=Func;

	while(*P0)if(*P0++=='(')break;
	const char* P1=P0;

	u64 ArgC=*P0&&*P0!=')';
	while(*P0&&*P0!=')')if(*P0++==',')ArgC++;//Get ArgC

	while(*P0++);
	u64 StrSize=P0-Func;//Get StrSize

	char** Args=(char**)malloc(sizeof(char**)*(ArgC+2));

	//v Similar to Args[0]=&ArgC;
	Args[0]=(char*)malloc(8);
	*(u64*)Args=ArgC;

	//v Args[1]=StrCopy(Func);
	P0=Func;
	char* P2=(char*)malloc(StrSize+1);
	char* P3=P2;
	do *P2++=*P0;while(*P0++);
	Args[1]=P3;

	va_list List;
	va_start(List,ArgC);//Init pos

	//v Loops on each arguments
	u64 i=0;
	while(i<ArgC){
		P0=P1++;
		u16 Size=0;//v Size=StrToInt(P1);
		while(*P1&&*P1!=','){
			if(*P1>='0'&&'9'>=*P1){
				Size*=10;
				Size+=*P1-'0';
			}
			P1++;
		}
		P1++;
		char* Str=nullptr;

		if(*P0=='s'||*P0=='u'){
			u64 Var=va_arg(List,u64);
			bool Neg=*P0=='s'&&Var&1<<(Size-1);
			if(Neg)Var=-Var;

			if(Size<64)Var&=((u64)1<<Size)-1;
			u64 A=Var;
			u8 Size=0;do Size++;while(A/=10);	//Gets number of digits in Int

			Str=(char*)malloc(Size+1+Neg);
			Str+=Size+Neg;
			*Str--=0;
			do *Str--=Var%10+'0';while(Var/=10);
			if(Neg){
				*Str='-';
			}else Str++;
		}
		Args[i+2]=Str;

		i++;
	}
	va_end(List);

	FunCall=(char***)realloc(FunCall,Level+1);
	FunCall[Level++]=Args;
}

LBR::DBG::Tracker::~Tracker(){
	if(Enabled&&Level){
		Level--;
		char** P=FunCall[Level];
		u64 ArgC=*(u64*)P+1;P++;//Gets the argument count
		while(ArgC--)free(*P++);
		free(FunCall[Level]);

		if(Level){
			FunCall=(char***)realloc(FunCall,Level);
		}else{
			free(FunCall);
			FunCall=nullptr;
		}
	}
}

void LBR::DBG::TraceLine(u64 Ln){
	Line=Ln;
}

static void SegFaultLens(int Sig){
	if(Sig==SIGSEGV){
		PrintSp("\n\x1B[0mSegmentation fault after line");
		PrintLn(LBR::DBG::Line);
		for(u64 i=0;i<LBR::DBG::Level;i++){
			char** P=LBR::DBG::FunCall[i];

			char** End=P+(*(u64*)P+2);

			P++;
			bool Arg=false;
			while(P<End){
				for(u64 j=0;j<i+Arg;j++)PrintSp('|');
				PrintSp(Arg?'|':'+');
				if(Arg)PrintSp(' ');

				PrintLn(*P);
				free(*P++);
				Arg=true;
			}
			for(u64 j=0;j<=i;j++)PrintSp('|');
			PrintLn();
			free(LBR::DBG::FunCall[i]);
		}
		free(LBR::DBG::FunCall);

		exit(-1);
	}
}

void LBR::DBG::InitSegFault(){
	struct sigaction SigAct;
	SigAct.sa_handler=SegFaultLens;	//Assign the handling function
	sigemptyset(&SigAct.sa_mask);	//Reset
	SigAct.sa_flags=0;				//Reset too
	sigaction(SIGSEGV,&SigAct,nullptr);	//Enables
	Enabled=true;
}
