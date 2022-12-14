#include "../Lebray/IO.h"
#include "../Lebray/Maths.h"
#include "../Lebray/Memo.h"
#include "../Lebray/Time.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>//For mkdir

u64 StrToInt(cc8* Str,bool &IsRawNum){
	u64 Num=0;
	for(;*Str;Str++)if(*Str∈['0';'9'])Num=Num*10+*Str-'0';
	else IsRawNum=0;
	return Num;
}

c8* StrAdd(cc8* Str0,cc8* Str1){
	cc8* P0=Str0;
	while(*P0)P0++;u64 Size=P0-Str0;
	P0=Str1;
	while(*P0)P0++;Size+=P0-Str1;

	c8* Add=(c8*)Memo(Size+1);
	while(*Str0)*Add++=*Str0++;
	do *Add++=*Str1;while(*Str1++);
	return Add-Size-1;
}

c8* StrAdd(c8* Str0,cc8* Str1){
	cc8* P0=Str0;
	while(*P0)P0++;u64 Size0=P0-Str0;
	P0=Str1;
	while(*P0)P0++;u64 Size1=P0-Str1;

	Str0=(c8*)Remo(Str0,Size0+Size1+1);
	Str0+=Size0;
	do *Str0++=*Str1;while(*Str1++);
	return Str0-Size0-Size1-1;
}

c8* u64ToStr(u64 Int){//Should be IntToStr but it would cause name conflict
	u64 A=Int;
	u8 Size=0;do Size++;while(A/=10);	//Gets number of digits in Int

	c8* Str=((c8*)Memo(Size+1))+Size;
	*Str--=0;
	do *Str--=Int%10+'0';while(Int/=10);
	return Str+1;
}


/*/Number of day depending on the month
 * 0: 31	(January)
 * 1: 28 or 29
 * 2: 31
 * 3: 30
 * 4: 31
 * 5: 30
 * 6: 31
 * 7: 31
 * 8: 30
 * 9: 31
 * 10: 30
 * 11: 31
/*/
inline u8 DaysInMonth0(u8 Month,u16 Year){
	return Month==1?(IsLeapYear(Year)?29:28):30|((Month&1)^(Month<=6));
}
cc8* HelpStr=//Tabs length 8 spaces for terminals.
"			+=================+\n\
			| Examples of use |\n\
			+=================+\n\
\n\
+---------------------------------+--------------+----------------------+\n\
| Versioning [Day] [Month] [Year] |		 | Versioning 19 5 1973 |\n\
+---------------------------------+		 +----------------------+\n\
| Gives the day time-stamp using the given date, here 19/05/1973.	|\n\
| It's important to use spaces. 					|\n\
+-----------------------------------------------------------------------+\n\
\n\
+-----------------------------------------------------------------------+\n\
| Versioning [Period] [Ver] [Destination] ([VerFile])			|\n\
| Versioning 0 1234 OldFolder ./Stats/Ver	    Do you get the joke?|\n\
+-----------------------------------------------------------------------+\n\
| \x1B[4mCreate a directory and print its name to stdout.\x1B[24m			|\n\
| The name is: the current version calculated from the number given.	|\n\
|	(For this example: 1234 is given)				|\n\
|	That number is given by the first example, by putting the	|\n\
|	creation date.							|\n\
| Then with the file \x1B[4m./Stats/Ver\x1B[24m (created automatically),		|\n\
|  it deduces the sub-versioning.					|\n\
+-[Return value]--------------------------------------------------------+\n\
| The return value is \x1B[4m0 when there are no errors.\x1B[24m			|\n\
| Else the stdout is used to show the error. (Yes this is uncommon.)	|\n\
+-[Example]-------------------------------------------------------------+\n\
| Version Example: 0.1.2.3.45						|\n\
|	0th period (big-version)					|\n\
|	1 year								|\n\
|	2 month								|\n\
|	3 days								|\n\
|	45th sub-version (starting from 0th)				|\n\
| Result: \x1B[4mOldFolder/1.2.3.45/\x1B[24m is created.				|\n\
+-----------------------------------------------------------------------+\n\
\n\
+-------------------------------------+-----+---------------------------+\n\
| Versioning [SubVersion] ([VerFile]) |	    | Versioning 45 ./Stats/Ver |\n\
+-------------------------------------+	    +---------------------------+\n\
| Gives the day time-stamp using the given date, here 19/05/1973.	|\n\
| It's important to use spaces. 					|\n\
+-----------------------------------------------------------------------+\n\
\n\
+===============================+=========+=============================+\n\
|				| License |				|\n\
| By the way…			+=========+				|\n\
| As that program is not meant to be secure, anything done different	|\n\
| than what mentioned will result in undefined behaviour.		|\n\
| However, it should not crash, its more likely to discard things.	|\n\
|									|\n\
| So…									|\n\
| This is licensed under the terms of the Lebray Open-Source License NW	|\n\
|									|\n\
| This program was made in the hope to be useful WITHOUT ANY WARRANTY,	|\n\
| not even the warranty for the program to work nor to fit any purpose.	|\n\
| The user is solely responsible for their use of the program. This	|\n\
| program may be modified by anyone and shared to anyone but the program|\n\
| alone must never be sold, as it's already available freely (⋅-. )	|\n\
+=======================================================================+\n\
Look, the license is smaller than the documentation!\n\
By Léolol DB from the 6/10/12/2022 to the 1/12/12/2022";
s32 main(s32 ArgC,c8** ArgV){
	s32 Ret=0;
	if(ArgC>=4){//3 arguments (or more)
		bool IsInt=1;
		u16 Year=StrToInt(ArgV[3],IsInt);
		if(IsInt){//v Day Month Year to TimeStanp
			u64 T=StrToInt(ArgV[1])-1;
			u8 Month=StrToInt(ArgV[2]);
			//v This is very inneficient but good enough for now
			for(u16 i=1970;i<Year;i++)T+=DaysInYear(i);
			for(u8 i=0;i+1<Month;i++)T+=DaysInMonth0(i,Year);
			PrintLn(T);
		}else{//v Period Ver to Destination
			u64 Period=StrToInt(ArgV[1]);
			u64 T=StrToInt(ArgV[2]);
			u64 DTS=GetDayTimeStamp();
			if(DTS>=T)T=DTS-T;else T=0;
			
			u64 SubVer=0;{
				s32 FD=open(ArgC>=5?ArgV[4]:"./Stats/Ver",O_RDWR|O_CREAT,0644);
				if(FD>=0){
					u64 TS;
					s32 R=read(FD,&TS,8);
					
					if(R==8&&TS==DTS){
						R=read(FD,&SubVer,8);
						SubVer++;
						
						if(R!=8){
							PrintLn("Couldn't read fully ./Stats/Ver but got ",R);
							if(!Ret)Ret=-1;
							Ret&=~4;
						}
					}else SubVer=0;
					
					lseek(FD,0,SEEK_SET);
					R=write(FD,&DTS,8);
					R+=write(FD,&SubVer,8);
					if(R!=16){
						PrintLn("Couldn't write fully to ./Stats/Ver but got ",R);
						if(!Ret)Ret=-1;
						Ret&=~1;
					}
					close(FD);
				}else{
					PrintLn("Couldn't open ",ArgC>=4?ArgV[4]:"./Stats/Ver","\nError: ",errno);
					if(!Ret)Ret=-1;
					Ret&=~2;
				}
			}
			
			u8 Day=0;
			u8 Month=0;
			TimeStampToVersion(&Day,&Month,&Year,T);
			
			c8* FileName=StrAdd((cc8*)ArgV[3],"/");
			c8* TStr=u64ToStr(Period);FileName=StrAdd(FileName,TStr);Remo(TStr);
			FileName=StrAdd(FileName,".");
			TStr=u64ToStr(Year);FileName=StrAdd(FileName,TStr);Remo(TStr);
			FileName=StrAdd(FileName,".");
			TStr=u64ToStr(Month);FileName=StrAdd(FileName,TStr);Remo(TStr);
			FileName=StrAdd(FileName,".");
			TStr=u64ToStr(Day);FileName=StrAdd(FileName,TStr);Remo(TStr);
			FileName=StrAdd(FileName,".");
			TStr=u64ToStr(SubVer);FileName=StrAdd(FileName,TStr);Remo(TStr);
			s32 R=mkdir(FileName,0774);
			if(R){
				PrintLn("Couldn't create ",FileName,"\nError: ",errno);
				if(!Ret)Ret=-1;
				Ret&=~8;
			}else FPrint(FileName);
			Remo(FileName);
		}
	}else if(ArgC>=2){
		u32 FD=open(ArgC>=3?ArgV[2]:"./Stats/Ver",O_WRONLY|O_TRUNC|O_CREAT,0644);
		if(FD>=0){
			u64 DTS=GetDayTimeStamp();
			u64 TS;
			s32 R=read(FD,&TS,8);
			
			u64 Ver=StrToInt(ArgV[1])-1;
			R=write(FD,&DTS,8);
			R+=write(FD,&Ver,8);
			if(R!=16){
				PrintLn("Couldn't write fully to ./Stats/Ver but got ",R);
				if(!Ret)Ret=-1;
				Ret&=~1;
			}else PrintLn("Changed SubVersion to ",Ver+1);
			close(FD);
		}else{
			PrintLn("Couldn't open ",ArgC>=3?ArgV[2]:"./Stats/Ver","\nError: ",errno);
			if(!Ret)Ret=-1;
			Ret&=~2;
		}
	}else PrintLn(HelpStr);//Less than 3 arguments
	return Ret;
}
