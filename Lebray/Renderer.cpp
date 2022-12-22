/*/Functions used to make graphical things
 * Go to Renderer.h to have more documentation
 * Version 5.1.4.27
/*///By Léolol DB

#include "Renderer.h"
#include "Maths.h"		//For many math things
#include <malloc.h>

#define LBRDefaultTitle "Pixel Engine 5.1.8.1 - Phoenix Powder"
//#define LBRPxEDebug

#include "IO.h"
#include "Time.h"

inline void SetArr(void* Arr,u64 Pos,u8 IntSize,u64 Int){//Size in bits, Pos in bits
	//const u8 Shift=8-Pos%8;
	if(IntSize+Pos%8>8){//If writing goes over 1 byte
		SetArr(Arr,Pos,8-Pos%8,Int);				//Writes the first byte
		SetArr(Arr,(Pos/8+1)*8,IntSize-8+Pos%8,Int>>(8+Pos%8));	//Writes the rest
	}else{
		((u8*)Arr)[Pos/8]&=~((0xFF>>(8-IntSize))<<(Pos%8));
		((u8*)Arr)[Pos/8]|=((Int&0xFF)>>(8-IntSize))<<(Pos%8);
	}
}
inline u64 GetArr(void* Arr,u64 Pos,u8 IntSize){//Size in bits, Pos in bits
	if(IntSize+Pos%8>8){//If reading goes over 1 byte
		return GetArr(Arr,Pos,8-Pos%8)|(GetArr(Arr,(Pos/8+1)*8,IntSize-8+Pos%8)<<(8-Pos%8));
	}else return (((u8*)Arr)[Pos/8]>>(Pos%8))&(0xFF>>(8-IntSize));
}

inline u64 ArrSum(u8* Start,u8* End){
	u64 Sum=0;
	while(Start<End)Sum+=*Start++;
	return Sum;
}

inline u8 IntByteSize(u64 Value,u8 Min){
	Value>>=8*Min;
	while(Value){
		Min++;
		Value>>=8;
	}
	return Min;
}

inline u64 ArrBitSpop(u8* &Arr,u8 &Offset,u8 Size){
	u64 Value=0;
	for(u8 i=0;i<Size;i++){
		Value|=u64((*Arr>>Offset)&1)<<i;
		Offset++;
		if(Offset>=8){
			Offset=0;
			Arr++;
		}
	}
	return Value;
}
inline void ArrBitPush(u8* &Arr,u8 &Offset,u64 Value,u8 Size){
	while(Size--){
		*Arr&=~(1<<Offset);
		*Arr|=Value<<Offset;
		Value>>=1;
		Offset++;
		if(Offset>=8){
			Offset=0;
			Arr++;
		}
	}
}

inline u8 IntSize(u64 Value){
	u8 Size=0;
	while(Value){
		Size++;
		Value>>=1;
	}
	return Size;
}

u64 ToSquare(u8* Sq,u64 SqSize,u8* Data,u64 DSize){//All in Bytes but returns bits
	if(SqSize==0)return true;
	u8* SqData=Sq;

	u8 Range=1;
	while(DSize>>Range)Range++;

	u64* DecV=nullptr;
	u8* MulV=nullptr;

	u8 Offset=0;//For Sq

	u8* End=Data+DSize;
	u8* SqEnd=Sq+SqSize;
	while(Range--){
		u8* EP=Data;
		u64 SqCount=((DSize>>Range)+1)>>1;//How many will be added

		u64* NDecV=(u64*)malloc(SqCount*16);
		u8* NMulV=(u8*)malloc(SqCount*2);
		u64* NDVP=NDecV;
		u64* DVP=DecV;
		u8* NMVP=NMulV;
		u8* MVP=MulV;

		bool Shift=0;
		while(1){
			if(Sq>=SqEnd){
				Range=0;
				break;
			}
			u8* DP=EP;
			EP=DP+((u64)1<<Range);
			if(EP>End)break;

			u64 Var=ArrSum(DP,EP);

			DP=EP;
			EP=DP+((u64)1<<Range);
			if(EP>End){
				*NDVP=Var;
				*NMVP=Range-1;
				ArrBitPush(Sq,Offset,Var,Range+7);
				break;
			}
			u64 VRange=MacroMin(*DVP,((u64)510<<*MVP)-*DVP)>>(*MVP-Range);
			if(Sq+(IntSize(VRange*2)+Offset+7)/8>SqEnd){
				Range=0;
				break;
			}

			Var+=((u64)255<<Range)-ArrSum(DP,EP);

			*NDVP++=*DVP+(Var<<(*MVP-Range))-((u64)255<<*MVP);
			*NDVP++=(*DVP++)+((u64)255<<*MVP)-(Var<<(*MVP-Range));
			Var-=((u64)255<<Range)-VRange;//Reduces the value

			*NMVP++=*MVP;
			*NMVP++=*MVP++;

			ArrBitPush(Sq,Offset,Var,IntSize(VRange*2));
		}
		if(DecV!=nullptr)free(DecV);
		DecV=NDecV;
		if(MulV!=nullptr)free(MulV);
		MulV=NMulV;
	}
	if(DecV!=nullptr)free(DecV);
	if(MulV!=nullptr)free(MulV);
	return (Sq-SqData)*8+Offset;
}
bool FromSquare(u8* Sq,u64 SqSize,u8* Data,u64 DSize){//All in Bytes
	if(SqSize==0)return true;

	u8 Range=1;
	while(DSize>>Range)Range++;

	u64* DecV=nullptr;
	u8* MulV=nullptr;

	u8 Offset=0;//For Sq

	bool ValidData=true;

	//u8* End=Data+DSize;
	u8* SqEnd=Sq+SqSize;
	while(Range--){
		//u8* EP=Data;
		u64 SqCount=((DSize>>Range)+1)>>1;//How many will be added

		u64* NDecV=(u64*)malloc(SqCount*16);
		u8* NMulV=(u8*)malloc(SqCount*2);
		u64* NDVP=NDecV;
		u64* DVP=DecV;
		u8* NMVP=NMulV;
		u8* MVP=MulV;

		u64 DP=0;

		bool Shift=0;
		while(1){
			DP+=((u64)1<<Range);
			if(DP>DSize)break;
			if(ValidData&&Sq>=SqEnd)ValidData=false;

			DP+=((u64)1<<Range);
			if(DP>DSize){
				*NMVP=Range-1;
				*NDVP=ValidData?ArrBitSpop(Sq,Offset,Range+7):0;
				break;
			}
			u64 VRange;
			if(ValidData){
				VRange=MacroMin(*DVP,((u64)510<<*MVP)-*DVP)>>(*MVP-Range);
				if(Sq+(Offset+IntSize(VRange*2)+7)/8>SqEnd)ValidData=false;
			}if(ValidData){
				u64 Var=ArrBitSpop(Sq,Offset,IntSize(VRange*2));

				Var+=((u64)255<<Range)-VRange;//Augment the value
				*NDVP++=*DVP+(Var<<(*MVP-Range))-((u64)255<<*MVP);
				*NDVP++=(*DVP++)+((u64)255<<*MVP)-(Var<<(*MVP-Range));
			}else{
				*NDVP++=*DVP;
				*NDVP++=*DVP++;
			}

			*NMVP++=*MVP;
			*NMVP++=*MVP++;
		}
		if(DecV!=nullptr)free(DecV);
		DecV=NDecV;
		if(MulV!=nullptr)free(MulV);
		MulV=NMulV;
	}
	u8* DP=Data;
	u64* DVP=DecV;
	u8* MVP=MulV;
	while(DSize--){
		*DP++=*MVP?(((*DVP>>*MVP)+1)>>1):*DVP;
		MVP++;
		DVP++;
	}
	if(DecV!=nullptr)free(DecV);
	if(MulV!=nullptr)free(MulV);
	return true;
}

u64 CompressAlg1(u8* CD,u64 CDSize,u8* Img,u32 XSize,u32 YSize){
	u8* CDBeg=CD;
	u8* CDEnd=CD+CDSize;

	u8 Offset=0;
	u8 Buff[XSize*3];
	for(u32 X=0;X<XSize*3;X++)Buff[X]=0;

	for(u32 Y=0;Y<YSize;Y++){
		for(u32 X=0;X<XSize;X++){
			if(CD>=CDEnd)return (CD-CDBeg)*8+Offset;
			u32 Clr=0;
			for(u8 C=0;C<3;C++)Clr|=(*Img++)<<(C*8);
			u8 i;
			for(i=0;i<3;i++){
				u32 CC=0;
				for(u8 C=0;C<3;C++)CC|=Buff[(XSize-1+X+i)%XSize*3+C]<<(C*8);
				if(CC==Clr)break;
			}
			ArrBitPush(CD,Offset,i,2);

			if(i==3){
				if(CD+3>=CDEnd)return (CD-CDBeg)*8+Offset;
				ArrBitPush(CD,Offset,Clr,24);
			}
			for(u8 C=0;C<3;C++)Buff[X*3+C]=Clr>>(C*8);
		}
	}
	return (CD-CDBeg)*8+Offset;
}
u64 DecompressAlg1(u8* CD,u64 CDSize,u8* Img,u32 XSize,u32 YSize){
	u8* CDBeg=CD;
	u8* CDEnd=CD+CDSize;

	u8 Offset=0;
	u8 Buff[XSize*3];
	for(u32 X=0;X<XSize*3;X++)Buff[X]=0;

	u32 Y,X;
	for(Y=0;Y<YSize;Y++){
		for(X=0;X<XSize;X++){
			u32 Clr=0;
			if(CD>=CDEnd)break;//Security Check
			u8 A=ArrBitSpop(CD,Offset,2);
			if(A==3){
				if(CD+3>=CDEnd){CD+=3;break;}//Security Check
				Clr=ArrBitSpop(CD,Offset,24);
			}else for(u8 C=0;C<3;C++)Clr|=Buff[(XSize-1+X+A)%XSize*3+C]<<(C*8);

			for(u8 C=0;C<3;C++){
				*Img++=Clr>>(C*8);
				Buff[X*3+C]=Clr>>(C*8);
			}
		}
		if(CD>=CDEnd)break;
	}
	for(;Y<YSize;Y++){
		for(;X<XSize;X++)for(u8 C=0;C<3;C++)*Img++=0;
		X=0;
	}
	return (CD-CDBeg)*8+Offset;
}

u64 CompressAlg2(u8* CD,u64 CDSize,u8* Img,u32 XSize,u32 YSize){
	u8* CDBeg=CD;
	u8* CDEnd=CD+CDSize;

	u8 Offset=0;
	u8 Buff[XSize*3];
	for(u32 X=0;X<XSize*3;X++)Buff[X]=0;

	for(u32 Y=0;Y<YSize;Y++){
		//PrintLn();
		for(u32 X=0;X<XSize;X++){
			if(CD>=CDEnd)return (CD-CDBeg)*8+Offset;
			u32 Clr=0;
			for(u8 C=0;C<3;C++)Clr|=(*Img++)<<(C*8);
			u8 A;
			for(A=0;A<3;A++){
				u32 CC=0;
				for(u8 C=0;C<3;C++)CC|=Buff[(XSize-1+X+A)%XSize*3+C]<<(C*8);
				//PrintHex(CC);
				//PrintSp();
				if(CC==Clr)break;
			}
			for(u8 i=0;i<A&&i<2;i++)ArrBitPush(CD,Offset,1,1);
			ArrBitPush(CD,Offset,A==3,1);

			//PrintSp(i);
			if(A==3){
				if(CD+3>=CDEnd)return (CD-CDBeg)*8+Offset;
				ArrBitPush(CD,Offset,Clr,24);
			}
			for(u8 C=0;C<3;C++)Buff[X*3+C]=Clr>>(C*8);
		}
	}
	return (CD-CDBeg)*8+Offset;
}
u64 DecompressAlg2(u8* CD,u64 CDSize,u8* Img,u32 XSize,u32 YSize){
	u8* CDBeg=CD;
	u8* CDEnd=CD+CDSize;

	u8 Offset=0;
	u8 Buff[XSize*3];
	for(u32 X=0;X<XSize*3;X++)Buff[X]=0;
	//PrintLn();//

	u32 Y,X;
	for(Y=0;Y<YSize;Y++){
		//PrintLn();//
		for(X=0;X<XSize;X++){
			u32 Clr=0;
			if(CD+1>=CDEnd)break;
			u8 A=0;
			while(ArrBitSpop(CD,Offset,1)==1&&(A++<2));
			//PrintSp(A);//
			if(A==3){
				if(CD+3>=CDEnd){
					CD+=3;
					break;
				}
				Clr=ArrBitSpop(CD,Offset,24);
			}else for(u8 C=0;C<3;C++)Clr|=Buff[(XSize-1+X+A)%XSize*3+C]<<(C*8);

			for(u8 C=0;C<3;C++){
				//*Img++=A*0x55;//
				*Img++=Clr>>(C*8);
				Buff[X*3+C]=Clr>>(C*8);
			}
		}
		if(CD+1>=CDEnd)break;
	}
	for(;Y<YSize;Y++){
		for(;X<XSize;X++)for(u8 C=0;C<3;C++)*Img++=0;
		X=0;
	}
	return (CD-CDBeg)*8+Offset;
}

u64 CompressAlg3(u8* CD,u64 CDSize,u8* Img,u32 XSize,u32 YSize){
	u8* CDBeg=CD;
	u8* CDEnd=CD+CDSize;

	u8 Offset=0;
	u8 Buff[XSize*3];
	for(u32 X=0;X<XSize*3;X++)Buff[X]=0;

	u64 CopyCount=0;
	for(u32 Y=0;Y<YSize;Y++){
		for(u32 X=0;X<XSize;X++){
			if(CD+1>=CDEnd)return (CD-CDBeg)*8+Offset;
			u32 Clr=0;
			for(u8 C=0;C<3;C++)Clr|=(*Img++)<<(C*8);
			u8 A;
			for(A=0;A<3;A++){
				u32 CC=0;
				for(u8 C=0;C<3;C++)CC|=Buff[(XSize-1+X+A)%XSize*3+C]<<(C*8);

				if(CC==Clr)break;
			}

			if(A){
				if(CopyCount){//v Add CopyCount
					ArrBitPush(CD,Offset,0,1);
					CopyCount--;
					u8 S;
					for(S=0;CopyCount>>(S*4);S++){
						ArrBitPush(CD,Offset,0,1);
						CopyCount-=1<<(S*4);
					}
					if(CD+S/2+1>=CDEnd)return (CD-CDBeg)*8+Offset;
					ArrBitPush(CD,Offset,CopyCount*2+1,S*4+1);
					CopyCount=0;
				}
				//v Push(A)
				for(u8 i=0;i<A&&i<2;i++)ArrBitPush(CD,Offset,1,1);
				ArrBitPush(CD,Offset,A==3,1);

				if(A==3){
					if(CD+3>=CDEnd)return (CD-CDBeg)*8+Offset;
					ArrBitPush(CD,Offset,Clr,24);
				}
			}else CopyCount++;
			for(u8 C=0;C<3;C++)Buff[X*3+C]=Clr>>(C*8);
		}
	}
	if(CopyCount){//v Add CopyCount
		ArrBitPush(CD,Offset,0,1);
		u8 S;
		for(S=0;CopyCount>>(S*4);S++){
			ArrBitPush(CD,Offset,0,1);
			CopyCount-=1<<(S*4);
		}
		ArrBitPush(CD,Offset,CopyCount*2+1,S*4+1);
	}
	return (CD-CDBeg)*8+Offset;
}
u64 DecompressAlg3(u8* CD,u64 CDSize,u8* Img,u32 XSize,u32 YSize){
	u8* CDBeg=CD;
	u8* CDEnd=CD+CDSize;

	u8 Offset=0;
	u8 Buff[XSize*3];
	for(u32 X=0;X<XSize*3;X++)Buff[X]=0;

	u64 CopyCount=0;
	u32 Y,X;
	for(Y=0;Y<YSize;Y++){
		for(X=0;X<XSize;X++){
			u32 Clr=0;
			u8 A=0;

			if(CopyCount==0){
				if(CD+1>=CDEnd)break;
				while(ArrBitSpop(CD,Offset,1)==1&&(A++<2));

				if(A==0){//v Add CopyCount
					u8 S;
					CopyCount=0;
					for(S=0;ArrBitSpop(CD,Offset,1)==0;S++){
						if(CD+1>=CDEnd)break;
						CopyCount+=1<<(S*4);
					}
					if(CD+S/2+1>=CDEnd)break;
					CopyCount+=ArrBitSpop(CD,Offset,S*4);

				}
			}else CopyCount--;
			if(A==3){
				if(CD+3>=CDEnd){
					CD+=3;
					break;
				}
				Clr=ArrBitSpop(CD,Offset,24);
			}else for(u8 C=0;C<3;C++)Clr|=Buff[(XSize-1+X+A)%XSize*3+C]<<(C*8);

			for(u8 C=0;C<3;C++){
				//*Img++=A*0x55;//
				*Img++=Clr>>(C*8);
				Buff[X*3+C]=Clr>>(C*8);
			}
		}
		if(CD+1>=CDEnd)break;
	}
	for(;Y<YSize;Y++){
		for(;X<XSize;X++)for(u8 C=0;C<3;C++)*Img++=0;
		X=0;
	}
	return (CD-CDBeg)*8+Offset;
}

u64 CompressAlg4(u8* CD,u64 CDSize,u8* Img,u32 XSize,u32 YSize){
	u8* CDEnd=CD+CDSize;

	u8 Offset=0;
	u8 Buff[XSize*3];
	for(u32 X=0;X<XSize*3;X++)Buff[X]=0;

	u64 CopyCount=0;
	for(u32 Y=0;Y<YSize;Y++){
		bool ClrCopy=1;
		for(u32 X=0;X<XSize;X++){
			if(CD+1>=CDEnd)return (CD+CDSize-CDEnd)*8+Offset;
			u32 PxClr=0;for(u8 C=0;C<3;C++)PxClr|=(*Img++)<<(C*8);
			u32 CC0=0;for(u8 C=0;C<3;C++)CC0|=Buff[(XSize-1+X)%XSize*3+C]<<(C*8);
			u32 CC1=0;for(u8 C=0;C<3;C++)CC1|=Buff[X*3+C]<<(C*8);

			u8 A=0;//v Start from here!
			A=ClrCopy|(CC0==PxClr)*2|(CC1==PxClr)*4;
			if((A&3)==2||(A&5)==5){//Can copy -Y or -X?
				if(CopyCount++==0)ArrBitPush(CD,Offset,0,1);
			}else{
				if(CopyCount){//v Add CopyCount
					CopyCount--;
					u8 S;
					for(S=0;CopyCount>>(S*4);S++){
						ArrBitPush(CD,Offset,0,1);
						CopyCount-=1<<(S*4);
					}
					if(CD+S/2+1>=CDEnd)return (CD+CDSize-CDEnd)*8+Offset;
					ArrBitPush(CD,Offset,CopyCount*2+1,S*4+1);
					CopyCount=0;
				}

				if(CD+1>=CDEnd)return (CD+CDSize-CDEnd)*8+Offset;
				ArrBitPush(CD,Offset,A&6?A&2?0:1:3,A&2?1:2);//Generate header
				if((A&6)==0){//v
					if(CD+3>=CDEnd)return (CD+CDSize-CDEnd)*8+Offset;
					ArrBitPush(CD,Offset,PxClr,24);
					ClrCopy=0;
				}else{
					CopyCount=1;
					ClrCopy=(A&6)==4;
				}
			}
			for(u8 C=0;C<3;C++)Buff[X*3+C]=PxClr>>(C*8);
		}
	}
	if(CopyCount){//v Add CopyCount
		u8 S;
		for(S=0;CopyCount>>(S*4);S++){
			ArrBitPush(CD,Offset,0,1);
			CopyCount-=1<<(S*4);
		}
		ArrBitPush(CD,Offset,CopyCount*2+1,S*4+1);
	}
	return (CD+CDSize-CDEnd)*8+Offset;
}
u64 DecompressAlg4(u8* CD,u64 CDSize,u8* Img,u32 XSize,u32 YSize){
	//v Init everything
	u8* CDEnd=CD+CDSize;

	u8 Offset=0;
	u8 Buff[XSize*3];
	for(u32 X=0;X<XSize*3;X++)Buff[X]=0;

	u64 CopyCount=0;
	u32 Y,X=0;
	for(Y=0;Y<YSize;Y++){
		bool ClrCopy=1;
		for(X=0;X<XSize;X++){
			u32 PxClr=0;

			if(CopyCount==0){
				if(CD+1>=CDEnd)break;//Security Test

				u8 A=0;//v Read Header
				while(A<2&&ArrBitSpop(CD,Offset,1))A++;

				ClrCopy=A&1;
				if(A==2){//v Set 24-bit Colour
					if(CD+3>=CDEnd){CD+=3;break;}//Security Test
					PxClr=ArrBitSpop(CD,Offset,24);
				}else{
					CopyCount=1;//Get CopyCount
					u8 S=0;while(ArrBitSpop(CD,Offset,1)==0){
						if(CD+1>=CDEnd)break;//Security Test
						CopyCount+=1<<(S++*4);
					}
					if(CD+S/2+1>=CDEnd){CD+=S/2+1;break;}//Security Test
					CopyCount+=ArrBitSpop(CD,Offset,S*4);
				}
			}if(CopyCount){
				for(u8 C=0;C<3;C++)PxClr|=Buff[(XSize-1+ClrCopy+X)%XSize*3+C]<<(C*8);
				CopyCount--;
			}
			for(u8 C=0;C<3;C++){
				*Img++=PxClr>>(C*8);
				Buff[X*3+C]=PxClr>>(C*8);
			}
		}
		if(CD+1>=CDEnd)break;
	}
	for(;Y<YSize;Y++){
		for(;X<XSize;X++)for(u8 C=0;C<3;C++)*Img++=0;
		X=0;
	}
	return (CD+CDSize-CDEnd)*8+Offset;
}

//v LBR::Frame functions
LBR::Frame::Frame(){
	ColourPalette=0;
	Padding=0;

	XSize=0;
	YSize=0;

	BitMapSize=0;
	BitMap=nullptr;

	_TextFont=nullptr;
	_TFCount=0;
}
LBR::Frame::Frame(u32 XSize_,u32 YSize_){
	//Defaults to 24-bit RGB with 32-bit padding
	ColourPalette=0x0888;
	Padding=24;

	XSize=XSize_;
	YSize=YSize_;

	BitMapSize=((u64)XSize*YSize*Padding+7)/8;
	BitMap=malloc(BitMapSize);
}

bool LBR::Frame::SetPixel(u32 X,u32 Y,u32 Colour){
	if(X>=XSize||Y>=YSize)return false;
	SetArr(BitMap,(X+Y*XSize)*Padding,Padding,Colour);
	return true;
}
u32 LBR::Frame::GetPixel(u32 X,u32 Y){
	if(X>=XSize||Y>=YSize)return 0;
	return GetArr(BitMap,(X+Y*XSize)*Padding,Padding);
}

bool LBR::Frame::DrawLine(s32 X0,s32 Y0,s32 X1,s32 Y1,u32 Colour){
	s32 S=MacroMax(
		MacroDiff(X0,X1),
		MacroDiff(Y0,Y1)
	);
	for(s32 i=0;i<S;i++){
		SetPixel(
			(X0*(S-i)+X1*i+S/2)/S,
			(Y0*(S-i)+Y1*i+S/2)/S,
			Colour
		);
	}
	SetPixel(X1,Y1,Colour);
	return true;
}
bool LBR::Frame::DrawXLine(s32 X0,s32 X1,u32 YPos,u32 Colour){
	if(YPos>=YSize)return false;
	if(X0>X1){
		s32 Tmp=X0;
		X0=X1;
		X1=Tmp;
	}

	if(X1>=0&&X0<XSize){
		if(X0<0)X0=0;
		if(X1>=XSize)X1=XSize-1;

		X1-=X0;
		if(Padding==32){
			u32* P=(u32*)BitMap+X0+YPos*XSize;
			do *P++=Colour;while(X1--);
		}else{
			do{
				SetPixel(X0++,YPos,Colour);
			}while(X1--);
		}
		return true;
	}
	return false;
}
bool LBR::Frame::DrawYLine(u32 XPos,s32 Y0,s32 Y1,u32 Colour){
	if(XPos>=XSize)return false;
	if(Y0>Y1){
		s32 Tmp=Y0;
		Y0=Y1;
		Y1=Tmp;
	}

	if(Y1>=0&&Y0<YSize){
		if(Y0<0)Y0=0;
		if(Y1>=YSize)Y1=YSize-1;

		Y1-=Y0;
		if(Padding==32){
			u32* P=(u32*)BitMap+XPos+Y0*XSize;
			do{
				*P=Colour;
				P+=XSize;
			}while(Y1--);
		}else{
			do{
				SetPixel(XPos,Y0++,Colour);
			}while(Y1--);
		}
		return true;
	}
	return false;
}

bool LBR::Frame::DrawRectangle(s32 XPos,s32 YPos,u32 XSize,u32 YSize,u32 Colour){
	if(XSize&&YSize){
		bool Ret=false;
		Ret|=DrawXLine(XPos,XPos+XSize-1,YPos,Colour);
		Ret|=DrawXLine(XPos,XPos+XSize-1,YPos+YSize-1,Colour);
		Ret|=DrawYLine(XPos,YPos,YPos+YSize-1,Colour);
		Ret|=DrawYLine(XPos+XSize-1,YPos,YPos+YSize-1,Colour);
		//Doesn't work when outrange!
		return Ret;
	}
	return false;
}
bool LBR::Frame::DrawFullRectangle(s32 XPos,s32 YPos,u32 XSize_,u32 YSize_,u32 Colour){
	if(
		XSize_&&(XPos>=0||u32(0-XPos)<XSize_)&&XPos<(s32)XSize&&
		YSize_&&(YPos>=0||u32(0-YPos)<YSize_)&&YPos<(s32)YSize
	){
		#ifdef LBRPxEDebug
		u64 T=GetNanoTime();
		#endif
		XSize_+=XPos-1;//XSize_=X1;

		if(YPos<0){
			YSize_+=YPos;
			YPos=0;
		}
		if(YSize_+YPos-1>=YSize)YSize_=YSize-YPos;
		if(XPos<0)XPos=0;
		if(XSize_>=XSize)XSize_=XSize-1;

		u64 Size=u64(XSize_-XPos)*YSize_;


		if(Padding==32){
			XSize_-=XPos;//XSize_=XSize;
			u32* P=(u32*)BitMap+XPos+XSize*YPos;
			u32 XShift=XSize-XSize_-1;
			while(YSize_--){
				u32* End=P+XSize_;
				do *P=Colour;while(P++<End);
				P+=XShift;
			}
		}else while(YSize_--)DrawXLine(XPos,XSize_,YPos++,Colour);//Fallback
		#ifdef LBRPxEDebug
		T=GetNanoTime()-T;
		PrintSp(T);Print("ns\x1B[16G");
		PrintSp(Size);Print("\x1B[2mpx\x1B[22m\x1B[32G");
		if(T)PrintSp(Size*50000000/T/3);Print("px/f\x1B[48G");
		if(T)PrintSp(Size*1000000000/T);
		PrintLn("px/s");
		#endif
		return true;
	}
	return false;
}

bool LBR::Frame::Clear(){
	u8* P=(u8*)BitMap;
	u8* End=(u8*)BitMap+BitMapSize;
	while(P<End)*P++=0;
	return true;
}
bool LBR::Frame::Render(){
	return true;
}

bool LBR::Frame::Resize(u32 XSize_,u32 YSize_){
	if(XSize!=XSize_||YSize!=YSize_){
		#ifdef LBRPxEDebug
		u64 T=GetNanoTime();
		#endif

		BitMapSize=((u64)XSize_*YSize_*Padding+7)/8;
		u8* NBM=(u8*)malloc(BitMapSize);

		if(Padding%8){
			//v Fallback
			u64 BMP=0;
			for(u32 Y=0;Y<YSize_;Y++){
				u64 YPos=(u64)Y*YSize/YSize_*XSize;
				for(u32 X=0;X<XSize_;X++){
					u64 C=GetArr(BitMap,((u64)X*XSize/XSize_+YPos)*Padding,Padding);
					SetArr(NBM,BMP,Padding,C);
					BMP+=Padding;
				}
			}
		}else{
			u8* NBMP=NBM;

			u8* BMP=(u8*)BitMap;
			u8 Pad=Padding/8;

			for(u32 Y=0;Y<YSize_;Y++){
				u64 YPos=(u64)Y*YSize/YSize_*XSize;
				for(u32 X=0;X<XSize_;X++){
					u64 Pos=((u64)X*XSize/XSize_+YPos)*Pad;
					for(u8 i=0;i<Pad;i++)*NBMP++=BMP[i+Pos];
				}
			}
		}

		free(BitMap);
		BitMap=NBM;

		XSize=XSize_;
		YSize=YSize_;

		#ifdef LBRPxEDebug
		T=GetNanoTime()-T;
		PrintSp(T);
		Print("ns\x1B[16G");
		PrintSp(XSize_*YSize_);
		Print("px\x1B[32G");
		if(T)PrintSp((u64)XSize_*YSize_*50000000/T/3);
		PrintLn("px/f");
		#endif
	}
	return true;
}

bool LBR::Frame::Propagate(bool Shade,u8 Size){
	if(XSize&&YSize){
		u8 Pad=Padding/8;
		u8* BMP=(u8*)BitMap-(1+XSize)*Pad*Size;

		u8* NBM=(u8*)malloc(BitMapSize);
		u8* NBMP=NBM;

		for(u32 Y=0;Y<YSize;Y++){
			for(u32 X=0;X<XSize;X++){
				for(u8 i=0;i<Pad;i++){
					u8 A=Shade?0:0xFF;
					for(u8 y=0;y<Size*2+1;y++){
						if(y<Size){
							if(Y<Size-y)continue;
						}else if(y>Size&&Y>=YSize+Size-y)break;
						for(u8 x=0;x<Size*2+1;x++){
							if(x<Size){
								if(X<Size-x)continue;
							}else if(x>Size&&X>=XSize+Size-x)break;
							A=Shade?MacroMax(A,BMP[(y*XSize+x)*Pad]):
								MacroMin(A,BMP[(y*XSize+x)*Pad]);
						}
					}

					*NBMP++=A;
					BMP++;
				}
			}
		}
		free(BitMap);
		BitMap=NBM;
	}
	return true;
}

//The idea is to see a flow like NC<=NCP<-OCP<-OC
/*inline u32 ConvertClrPalette(u16 NewClrPalette,u16 OldClrPalette,u32 OldClr){
	return OldClr;
}*/

void* LBR::Frame::ToBitMap(u16 NewColourPalette,u8 NewPadding){
	if(XSize&&YSize){
		if(Padding==NewPadding&&ColourPalette==NewColourPalette){
			u8* BMP=(u8*)BitMap;
			u8* NBM=(u8*)malloc(BitMapSize);
			u8* NBMP=NBM;
			u8* End=BMP+BitMapSize;
			while(BMP<End)*NBMP++=*BMP++;
			return NBM;
		}

		if(Padding%8||NewPadding%8){//v Fallback
			void* NBM=malloc(((u64)XSize*YSize*NewPadding+7)/8);
			if(NBM==nullptr)return nullptr;

			u64 P0=0;
			u64 P1=0;
			for(u32 Y=0;Y<YSize;Y++){
				for(u32 X=0;X<XSize;X++){
					u64 C=GetArr(BitMap,P1,Padding);//Y&64?0x55FFFF:0xFFAADD;
					SetArr(NBM,P0,NewPadding,C);
					P0+=NewPadding;
					P1+=Padding;
				}
			}

			return NBM;
		}else{
			u8 MinPad=(MacroMin(NewPadding,Padding)+7)/8;
			u8 PadL=(Padding+7-MinPad*8)/8;
			u8 NPadL=(NewPadding+7-MinPad*8)/8;
			u8* BMP=(u8*)BitMap;

			u64 NewBitMapSize=((u64)XSize*YSize*NewPadding+7)/8;
			u8* NBM=(u8*)malloc(NewBitMapSize);
			if(NBM==nullptr)return nullptr;
			u8* NBMP=NBM;

			for(u32 Y=0;Y<YSize;Y++){
				for(u32 X=0;X<XSize;X++){
					u8 i=MinPad;
					while(i--)*NBMP++=*BMP++;
					NBMP+=NPadL;
					BMP+=PadL;
				}
			}
			return NBM;//Please free
		}
	}
	return nullptr;
}

void* LBR::Frame::Convert(u64 Quality){
	if(XSize&&YSize&&Quality>1){
		u64 Size=(u64)YSize*XSize*Padding/8;
		u8* Img=(u8*)malloc(Quality);

		/*
		PrintLn(
			ToSquare(Img,Quality,(u8*)BitMap,Size)/8
		);
		FromSquare(Img,Quality,(u8*)BitMap,Size);
		/*/
		PrintLn(
			CompressAlg4(Img,Quality,(u8*)BitMap,XSize,YSize)/8
		);
		DecompressAlg4(Img,Quality,(u8*)BitMap,XSize,YSize);
		//*/

		if(Img!=nullptr)free(Img);

		return nullptr;
	}
	return nullptr;
}

bool LBR::Frame::Test(){
	//Propagate(0,1);
	Propagate(1,1);
	return true;
}

bool LBR::Frame::DrawBitMap(
	void* BitMap_,u32 BMXSize,u32 BMYSize,
	s32 XPos,s32 YPos
){
	//if(XSize<196)return false;
	return DrawBitMap(
		BitMap_,
		BMXSize,BMYSize,
		XPos,YPos,
		BMXSize,BMYSize,
		0,0,
		~0,~0
	);
}

bool LBR::Frame::DrawBitMap(
	void* BitMap_,
	u32 BMXSize,u32 BMYSize,
	s32 XPos,s32 YPos,
	u32 XSize_,u32 YSize_,
	s32 WinXPos,s32 WinYPos,
	u32 WinXSize,u32 WinYSize
){
	#ifdef LBRPxEDebug
	u64 T=GetNanoTime();
	#endif
	if(WinXPos>=XSize||WinYPos>=YSize)return false;

	if((s64)WinXSize+WinXPos>(s64)XSize)WinXSize=XSize-WinXPos;
	if((s64)WinXSize+WinXPos>(s64)YSize)WinYSize=YSize-WinXPos;

	u32 XOff=WinXPos>XPos?WinXPos-XPos:0;
	u32 YOff=WinYPos>YPos?WinYPos-YPos:0;
	u32 XEnd=MacroMin(XSize_,XPos<(s32)XSize?XSize-XPos:0);
	u32 YEnd=MacroMin(YSize_,YPos<(s32)YSize?YSize-YPos:0);
	if((s64)WinXSize+WinXPos<(s64)XEnd+XPos){
		XEnd=WinXSize+WinXPos-XPos;
	}if((s64)WinYSize+WinYPos<(s64)YEnd+YPos){
		YEnd=WinYSize+WinYPos-YPos;
	}

	if(
		XEnd<=XOff||YEnd<=YOff||
		(s64)XPos>=(s64)WinXPos+WinXSize||(s64)YPos>=(s64)WinYPos+WinYSize
	)return false;//Is bitmap in the window?

	if(BMXSize==XSize_){
		if(BMYSize==YSize_){
			if(Padding==32){
				u32 XShift=XSize+XOff-XEnd;
				u32 BMXShift=XOff+BMXSize-XEnd;
				if((XPos|XSize|BMXSize|XOff|XEnd)&1){
					u32* P0=(u32*)BitMap+XPos+XOff+(YPos+YOff)*XSize;
					u32* P1=(u32*)BitMap_+XOff+BMXSize*YOff;
					u32* End=(u32*)BitMap_+BMXSize*YEnd;
					if(XOff==0&&XEnd==BMXSize){
						while(P1<End){
							u32* End=P1+BMXSize;
							while(P1<End)*P0++=*P1++;
							P0+=XShift;
						}
					}else{
						u32 XW=XEnd-XOff;
						while(P1<End){
							u32* End=P1+XW;
							while(P1<End)*P0++=*P1++;
							P0+=XShift;
							P1+=BMXShift;
						}
					}
				}else{
					u64* P0=(u64*)BitMap+(XPos+XOff+(YPos+YOff)*XSize)/2;
					XOff/=2;
					XEnd/=2;
					BMXSize/=2;
					XShift/=2;
					BMXShift/=2;
					u64* P1=(u64*)BitMap_+XOff+BMXSize*YOff;
					u64* End=(u64*)BitMap_+BMXSize*YEnd;
					if(XOff==0&&XEnd==BMXSize){
						while(P1<End){
							u64* End=P1+XEnd;
							while(P1<End)*P0++=*P1++;
							P0+=XShift;
						}
					}else{
						u32 XW=XEnd-XOff;
						while(P1<End){
							u64* End=P1+XW;
							while(P1<End)*P0++=*P1++;
							P0+=XShift;
							P1+=BMXShift;
						}
					}
					XOff*=2;
					XEnd*=2;
					BMXSize*=2;
				}
			}else{
				u32 BMXShift=XOff+BMXSize-XEnd;
				u32* P=(u32*)BitMap_+YOff*BMXSize+XOff;
				for(u32 Y=YOff;Y<YEnd;Y++){
					for(u32 X=XOff;X<XEnd;X++){
						SetPixel(X+XPos,Y+YPos,*P++);
					}
					P+=BMXShift;
				}
			}
		}else{
			if(Padding==32){
				u32 XShift=XSize+XOff-XEnd;
				u32* P0=(u32*)BitMap+XPos+XOff+(YPos+YOff)*XSize;
				for(u32 Y=YOff;Y<YEnd;Y++){
					u32* P1=(u32*)BitMap_+Y*BMYSize/YSize_*BMXSize;
					u32* End=P1+XEnd-XOff;
					while(P1<End)*P0++=*P1++;
					P0+=XShift;
				}
			}else{
				for(u32 Y=YOff;Y<YEnd;Y++){
					u32* P=(u32*)BitMap_+Y*BMYSize/YSize_*BMXSize+XOff;
					for(u32 X=XOff;X<XEnd;X++)SetPixel(X+XPos,Y+YPos,*P++);
				}
			}
		}
	}else{
		if(Padding==32){
			u32 XShift=XSize+XOff-XEnd;
			u32* P0=(u32*)BitMap+XPos+XOff+(YPos+YOff)*XSize;
			for(u32 Y=YOff;Y<YEnd;Y++){
				u32* P1=(u32*)BitMap_+Y*BMYSize/YSize_*BMXSize;
				for(u32 X=XOff;X<XEnd;X++)*P0++=P1[X*BMXSize/XSize_];
				P0+=XShift;
			}
		}else{
			//v FallBack
			for(u32 Y=YOff;Y<YEnd;Y++){
				u64 YP=(u64)Y*BMYSize/YSize_*BMXSize;
				for(u32 X=XOff;X<XEnd;X++){
					SetPixel(X+XPos,Y+YPos,
						GetArr(BitMap_,(X*BMXSize/XSize_+YP)*Padding,Padding)
					);
				}
			}
		}
	}

	#ifdef LBRPxEDebug
	T=GetNanoTime()-T;
	PrintSp(T);Print("ns\x1B[16G");
	PrintSp((XEnd-XOff)*(YEnd-YOff));Print("px\x1B[32G");
	if(T)PrintSp(u64(XEnd-XOff)*(YEnd-YOff)*50000000/T/3);Print("px/f\x1B[48G");
	if(T){
		u64 Num=u64(XEnd-XOff)*(YEnd-YOff)*1000000000/T;
		Print("\x1B[2m");
		Print(Num/1000000000);
		Print("\x1B[22m");
		Print(Num/100000000%10);
		Print(Num/10000000%10);
		Print(Num/1000000%10);
		Print("\x1B[2m");
		Print(Num/100000%10);
		Print(Num/10000%10);
		Print(Num/1000%10);
		Print("\x1B[22m");
		Print(Num/100%10);
		Print(Num/10%10);
		Print(Num%10);
	}
	PrintLn("px/s");
	#endif
	return true;
}

//v For text rendering
bool LBR::Frame::GetTextSize(const char* Text,u32* IMGXSize,u32* Lines){
	/*
	if(_TextFont==nullptr)return false;
	const u8* P0=(const u8*)Text;	//Pointer just for the Text
	u8* P1=(u8*)_TextFont;  		//Pointer for the TextFont
	u8* TFEnd=P1+_TFSize;	//For emergency stop
	u8 Count=0;				//Counts chars
	u16 IMGXPos=0;
	*IMGXSize=0;
	*Lines=1;

	//v Count size of text
	while(*P0!=0){
		//v Checks for control char
		if(*P0=='\n'){
			if(*IMGXSize<IMGXPos)*IMGXSize=IMGXPos;
			IMGXPos=0;
			(*Lines)++;
			P0++;
			continue;
		}else if(*P0=='\r'){
			if(*IMGXSize<IMGXPos)*IMGXSize=IMGXPos;
			IMGXPos=0;
			P0++;
			continue;
		}
		//v Checks if the pointer is in the right area
		if(Count>*P0){
			P1=(u8*)_TextFont;
			Count=0;
		}
		//v Go to the right char
		while(Count<*P0){
			P1+=*P1+1;
			if(P1>=TFEnd)return false;//Pointer overflow error
			Count++;
		}
		//v Get the size of the char
		IMGXPos+=*P1+1;P0++;
	}
	if(*IMGXSize<IMGXPos)*IMGXSize=IMGXPos;
	(*IMGXSize)--;
	return true;/*/
	const u8* P0=(const u8*)Text;	//TextPtr
	u16 IMGXPos=0;
	*IMGXSize=0;
	*Lines=1;

	//v Count size of text
	while(*P0!=0){
		//v Checks for control char
		if(*P0=='\n'){
			if(*IMGXSize<IMGXPos)*IMGXSize=IMGXPos;
			IMGXPos=0;
			(*Lines)++;
			P0++;
			continue;
		}else if(*P0=='\r'){
			if(*IMGXSize<IMGXPos)*IMGXSize=IMGXPos;
			IMGXPos=0;
			P0++;
			continue;
		}else if(*P0=='\v')(*Lines)++;

		//v Get the size of the char
		IMGXPos+=6;P0++;
	}
	if(*IMGXSize<IMGXPos)*IMGXSize=IMGXPos;
	(*IMGXSize)--;
	return true;
	//*/
}

u16 LBR::Frame::GetTextFontSize(u16 TFont){
	return TFont?0:0x0606;
}

bool LBR::Frame::DrawText(
	const char* Text,
	s32 XPos,s32 YPos,
	u32 TextColour,u32 BackColour,
	u8 YSize_,u8 Resize
){
	/*
	if(_TextFont==nullptr)return false;

	//v Get IMG sizes
	u32 IMGXSize;
	u32 Line;		//IMGYSize=Line*LineSize
	GetTextSize(Text,&IMGXSize,&Line);
	IMGXSize*=Resize;

	//v Init
	u64 IMGSize=(u32)IMGXSize*Line*YSize_*Resize;
	u32* IMG=(u32*)malloc(IMGSize*4);	//The BitMap of the text
	u32* P3=IMG;
	for(u64 i=0;i<IMGSize;i++)*P3++=BackColour;

	//v Draws every chars
	const u8* P0=(const u8*)Text;	//Pointer just for the Text
	u8* P1=(u8*)_TextFont;	//Pointer for the TextFont
	u8* TFEnd=P1+_TFSize;	//For emergency stop
	u8 Count=0;		//Counts chars for optimisation
	P3=IMG;Line=0;
	while(*P0){
		//v Checks for control char
		if(*P0=='\n'||*P0=='\r'){
			if(*P0=='\n')Line++;
			P3=IMG+(u64)IMGXSize*Line*YSize_*Resize;
		}else{
			//v Checks if the pointer is in the right area
			if(Count>*P0){
				P1=(u8*)_TextFont;
				Count=0;
			}
			//v Go to the right char
			while(Count<*P0){
				P1+=*P1+1;
				if(P1>=TFEnd)return false;//Pointer overflow error
				Count++;
			}
			//v Draws the 1 char
			u8* P2=P1+1;
			for(u8 j=0;j<*P1;j++){
				if(P2>=TFEnd)return false;//Pointer overflow error
				u32* P4=P3;
				u8 B=YSize_;
				while(B--){//Draws a vertical line of the char
					for(u8 i=0;i<Resize;i++){
						for(u8 k=0;k<Resize;k++){
							*P4++=((*P2>>B)&1)?TextColour:BackColour;
						}
						P4+=IMGXSize-Resize;
					}
				}
				P3+=Resize;
				P2++;
			}
			//Adds a blank
			P3+=Resize;
		}
		P0++;
	}
	Line++;
	bool Output=DrawBitMap(IMG,IMGXSize,YSize_*Line*Resize,XPos,YPos);
	free(IMG);
	return Output;/*/

	//v Draws every chars
	const u8* P0=(const u8*)Text;	//TextPtr
	s32 X=XPos;
	s32 Y=YPos;
	while(*P0){
		//v Checks for control char
		if(*P0=='\n'||*P0=='\r'||*P0=='\v'){
			if(*P0=='\n'||*P0=='\r')X=XPos;
			if(*P0=='\n'||*P0=='\v')Y+=6;
		}else{
			DrawChar(
				*P0,X,Y,
				TextColour,BackColour,
				6,6,Resize
			);
			X+=6*Resize;
		}
		P0++;
	}

	return true;//*/
}
bool LBR::Frame::DrawChar(
	u32 Char,
	s32 XPos,s32 YPos,
	u32 TextColour,u32 BackColour,
	u8 YSize_,u8 Resize
){
	return LBR::Frame::DrawChar(
		Char,XPos,YPos,
		TextColour,BackColour,
		0,YSize_,Resize
	);
}
bool LBR::Frame::DrawChar(
	u32 Char,
	s32 XPos,s32 YPos,
	u32 TextColour,u32 BackColour,
	u8 XSize_,u8 YSize_,u8 Resize
){
	if(_TextFont!=nullptr||_TextFont[0]!=nullptr){
		u8* P0=((u8*)(_TextFont[0]))+1;

		u16 CharXSize=P0[0]?P0[0]:256;
		u16 CharYSize=P0[1]?P0[1]:256;
		u32 CharSize=CharXSize*CharYSize;//Full px Size

		u16 ReloC=((u16*)P0)[1];
		P0+=4;

		u32** P=(u32**)P0;//Skip header
		P0=nullptr;//Block with the Tile
		u32** End=P+ReloC;

		while(P<=End){
			if(
				**P<=Char&&
				**P+(*P)[1]>=Char
			){
				P0=*((u8**)P);
				break;
			}
			P++;
		}

		if(P0!=nullptr){//If a block contain that Char
			//v Finds the exact Char
			P0+=8+					//Skip block header
				(CharSize+7)/8*		//Get char Byte size
				((u32)Char-((u32*)P0)[0])//Get char position in the block
			;//*/

			u32* IMG=(u32*)malloc(CharSize*4);

			//v Decodes the IMG
			u32* P1=IMG;
			u32* End=P1+CharSize;
			u16 BitBuf=0;
			u8 BufSize=0;
			while(P1<End){
				if(!BufSize){
					BitBuf|=*P0++<<BufSize;
					BufSize+=8;
				}
				*P1++=BitBuf&1?TextColour:BackColour;
				BufSize--;
				BitBuf>>=1;
			}

			DrawBitMap(
				IMG,CharXSize,CharYSize,
				XPos,YPos,
				CharXSize*Resize,CharYSize*Resize,
				0,0,~0,~0
			);
			free(IMG);
			return true;
		}
	}
	//v Tofu on fail
	DrawFullRectangle(
		XPos,YPos,
		XSize_*Resize,YSize_*Resize,
		BackColour
	);
	if(Char>' ')DrawFullRectangle(
		XPos+1*Resize,YPos+1*Resize,
		(XSize_-2)*Resize,(YSize_-2)*Resize,
		TextColour
	);
	return false;
}


bool LBR::Frame::UseTextFont(void* TextFont,u16 Size){
	if(TextFont==nullptr||Size==0)return false;

	if(_TextFont!=nullptr)free(_TextFont);

	u8* TxtFont=(u8*)TextFont;

	/*
	//v Code taken and adapted from Ultrapedit's Editor
	const u8* MagicNum=(const u8*)"VGF1\x12";//Only 16-bit monospace!
	u8 i=0;while(i<6&&MagicNum[i]==TxtFont[i])i++;
	if(i==6&&Size>=12){
		u8* P0=(u8*)TextFont+6;

		u16 IMGXSize=*P0++;//AvgSize
		if(!IMGXSize)IMGXSize=0x100;
		u16 IMGYSize=*P0++;
		if(!IMGYSize)IMGYSize=0x100;
		u32 IMGFullSize=IMGXSize*IMGYSize;

		u8* TFont=(u8*)malloc(256*IMGXSize);
		u8* P2=TFont;

		u32 TMSize=*((u16*)P0)+1;//RelocC
		u16 Count=0;
		P0+=2;
		u16* P1=(u16*)P0;
		P0+=TMSize*4;//Jump header
		u16 BitBuf=0;
		u8 BufSize=0;
		for(u32 i=0;i<TMSize;i++){
			//v Read header
			u16 vAdd=*P1++;	//vAdd
			u16 vSize=*P1++;//vSize
			for(;Count<vAdd;Count++)*P2++=0;
			//v Read body
			for(u32 i=0;i<=vSize&&Count<256;i++){//On all char of that list
				//v Fill IMG header
				*P2++=IMGXSize-1;

				for(u16 X=0;X<IMGXSize-1;X++)P2[X]=0;//Reset char

				//v Flip char x&y
				for(u16 Y=IMGYSize-1;Y!=0xFFFF;Y--){
					for(u16 X=0;X<IMGXSize-1;X++){
						if(!BufSize){
							BitBuf|=*P0++<<BufSize;
							BufSize+=8;
						}
						P2[X]|=(BitBuf&1)<<Y;
						BitBuf>>=1;
						BufSize--;
					}
					BitBuf>>=1;
					BufSize--;
				}
				P2+=IMGXSize-1;
				Count++;
			}
		}
		for(;Count<256;Count++)*P2++=0;
		_TFSize=P2-TFont;
		TFont=(u8*)realloc(TFont,_TFSize);//Realloc the size required
		_TextFont=TFont;

		PrintLn(_TFSize);

		free(TextFont);
	}else{
		_TextFont=TextFont;
		_TFSize=Size;
	}

	return true;
	/*/

	//v Code taken and adapted from Ultrapedit's Editor
	const u8* MagicNum=(const u8*)"VGF1\x12";//Only 16-bit monospace!
	u8 i=0;while(i<6&&MagicNum[i]==TxtFont[i])i++;
	if(i==6&&Size>=12){
		//v Mallocs Font
		u16 FontBlokC=*((u16*)TxtFont+8);//FontBlockCount
		void** Font=(void**)malloc(5+(FontBlokC+1)*sizeof(void*));

		//v Setup ptrs
		u8* P0=TxtFont+6;

		u32 CharSize=(P0[0]?P0[0]:256)*(P0[1]?P0[1]:256);//Full bit Size
		u16 ReloC=((u16*)P0)[1];{
			u8* P1=(u8*)Font;
			*P1++=1;//Monospace

			//Copy AvgSize and ReloC (FontBlokC)
			for(u8 i=0;i<4;i++)*P1++=*P0++;
		}

		//v Fill Font
		u32** P1=(u32**)(((u8*)Font)+5);//Skip header
		for(u32 i=0;i<=ReloC;i++){
			//v Gets Byte Size with header + CharSize*CharCount
			u32 BSize=8+(CharSize+7)/8*(((u16*)P0)[1]+1);
			*P1=(u32*)malloc(BSize);
			//v Fill header
			(*P1)[0]=((u16*)P0)[0];	//vAdd
			(*P1)[1]=((u16*)P0)[1];	//vSize
			P0+=4;P1++;
		}


		//v Fill the Chars
		u16 BitBuf=0;
		u8 BufSize=0;
		u32** P=(u32**)(((u8*)Font)+5);//Skip header
		for(u32 i=0;i<=ReloC;i++){//On all lists
			u32 TC=P[i][1];//Count of chars(-1)
			u8* P1=((u8*)P[i])+8;//Skip header

			for(u32 i=0;i<=TC;i++){//On all char of that list
				//v Dump Char
				for(u16 i=0;i<CharSize/8;i++){
					BitBuf|=*P0++<<BufSize;
					*P1++=BitBuf;
					BitBuf>>=8;
				}
				//v Put remains
				if(CharSize%8){
					if(BufSize<CharSize%8){
						BitBuf|=*P0++<<BufSize;
						BufSize+=8;
					}
					*P1++=BitBuf;
					BitBuf>>=CharSize%8;
					BufSize-=CharSize%8;
				}
			}
		}

		_TFCount++;
		_TextFont=(void***)realloc(_TextFont,_TFCount*sizeof(void**));
		_TextFont[_TFCount-1]=Font;

		free(TextFont);
		return true;
	}
	return false;//*/
}

//v LBR::IOEngine
LBR::IOEngine::IOEngine(){
	WinConnected=false;
	_EventSel=0;
}

u16 LBR::IOEngine::GenerateEvents(){
	return 0;
}

//v LBR::EventEngine
bool LBR::EventEngine::GenerateEvent(LBR::Event &Event){
	_EventNum++;
	_Events=(LBR::Event*)realloc(_Events,_EventNum*sizeof(LBR::Event));
	_Events[_EventNum-1]=Event;
	return true;
}
u16 LBR::EventEngine::PendingEventsNumber(){
	return _EventNum;
}
LBR::Event LBR::EventEngine::NextEvent(){
	LBR::Event Event;
	Event.Type=0;
	if(_EventNum){
		_EventNum--;
		Event=*_Events;
		for(u16 i=0;i<_EventNum;i++){
			_Events[i]=_Events[i+1];
		}
		_Events=(LBR::Event*)realloc(_Events,_EventNum*sizeof(LBR::Event));
	}
	return Event;
}
bool LBR::EventEngine::SelectEvents(u8 Events){
	_EventSel=Events;
	return true;
}

//v LBR::TextEngine
u16 LBR::TextEngine::PendingEventsNumber(){
	if(_Pixel==nullptr)return _EventNum;
	return _Pixel->PendingEventsNumber();
}

bool LBR::TextEngine::Clear(){
	Print("\x1B[48;5;m\x1B[2J");
	return true;
}

u8* LBR::TextEngine::GetPrint(u32 Size){
	if((u64)_TxtBufSize+Size>_TxtBufMSize){
		_TxtBufMSize=(_TxtBufSize+Size+0x3FF)&(~0x3FF);//Round to upper value
		_TxtBuf=(u8*)realloc(_TxtBuf,_TxtBufMSize);
	}
	u8* P=_TxtBuf+_TxtBufSize;
	_TxtBufSize+=Size;
	return P;
}
void LBR::TextEngine::Print(const void* Block,u32 Size){
	if(Block==nullptr)return;

	const u8* P0=(const u8*)Block;
	u8* P1=GetPrint(Size);
	while(Size--)*P1++=*P0++;	//Adds
}
void LBR::TextEngine::Print(const char* Str){
	if(Str==nullptr)return;

	const char* P0=Str;
	while(*P0++);	//P0-Str=StrSize(Str)+1;

	u8* P1=GetPrint(P0-Str-1);
	while(*Str)*P1++=*Str++;//StrCopy(P1,Str);
}
void LBR::TextEngine::Print(u8 Num){
	u8 A=Num;
	u8 S=1;while(A/=10)S++;
	u8* P=GetPrint(S)+S-1;
	do *P--='0'+Num%10;while(Num/=10);
}
void LBR::TextEngine::Print(u16 Num){
	u16 A=Num;
	u8 S=1;while(A/=10)S++;
	u8* P=GetPrint(S)+S-1;
	do *P--='0'+Num%10;while(Num/=10);
}
void LBR::TextEngine::Print(u32 Num){
	u32 A=Num;
	u8 S=1;while(A/=10)S++;
	u8* P=GetPrint(S)+S-1;
	do *P--='0'+Num%10;while(Num/=10);
}
void LBR::TextEngine::Print(u64 Num){
	u64 A=Num;
	u8 S=1;while(A/=10)S++;
	u8* P=GetPrint(S)+S-1;
	do *P--='0'+Num%10;while(Num/=10);
}
void LBR::TextEngine::Print(s16 Num){
	bool Neg=Num<0;
	if(Neg)Num=-Num;
	u16 A=Num;

	u8 S=1;while(A/=10)S++;
	u8* P=GetPrint(S+Neg)+S-1;

	if(Neg)*P--='-';
	do *P--='0'+Num%10;while(Num/=10);
}
void LBR::TextEngine::Print(s32 Num){
	bool Neg=Num<0;
	if(Neg)Num=-Num;
	u16 A=Num;

	u8 S=1;while(A/=10)S++;
	u8* P=GetPrint(S+Neg)+S-1;

	if(Neg)*P--='-';
	do *P--='0'+Num%10;while(Num/=10);
}
void LBR::TextEngine::Print(s64 Num){
	bool Neg=Num<0;
	if(Neg)Num=-Num;
	u16 A=Num;

	u8 S=1;while(A/=10)S++;
	u8* P=GetPrint(S+Neg)+S-1;

	if(Neg)*P--='-';
	do *P--='0'+Num%10;while(Num/=10);
}
void LBR::TextEngine::Print(char Char){
	Print(&Char,1);
}

void LBR::TextEngine::ColourTxt(u8 R,u8 G,u8 B){
	if(u8(R+1)<2&&u8(G+1)<2&&u8(B+1)<2){
		Print(R/255*4+G/255*2+B/255);
	}else{
		Print("8;");
		if(R%0x33==0&&G%0x33==0&&B%0x33==0){//6x6x6 RGB
			Print("5;");
			Print(R/0x33*36+G/0x33*6+B/0x33+16);
		}else{//24-bit RGB
			Print("2;");
			Print(R);Print(';');
			Print(G);Print(';');
			Print(B);
		}
	}
}

void LBR::TextEngine::ColourTxt(u32 TxtColour,u32 BGColour){
	Print("\x1B[3");
	ColourTxt(TxtColour>>16,TxtColour>>8,TxtColour);
	Print(";4");
	ColourTxt(BGColour>>16,BGColour>>8,BGColour);
	Print('m');
}
void LBR::TextEngine::Colour12bTxt(u16 TxtColour,u16 BGColour){
	Print("\x1B[3");
	ColourTxt(((TxtColour>>8)&0xF)*0x11,((TxtColour>>4)&0xF)*0x11,(TxtColour&0xF)*0x11);
	Print(";4");
	ColourTxt(((BGColour>>8)&0xF)*0x11,((BGColour>>4)&0xF)*0x11,(BGColour&0xF)*0x11);
	Print('m');
}

void LBR::TextEngine::Colour12bTxt(u16 Colour){
	Print("\x1B[3");
	ColourTxt(((Colour>>8)&0xF)*0x11,((Colour>>4)&0xF)*0x11,(Colour&0xF)*0x11);
	Print('m');
}
void LBR::TextEngine::Colour12bBG(u16 Colour){
	Print("\x1B[4");
	ColourTxt(((Colour>>8)&0xF)*0x11,((Colour>>4)&0xF)*0x11,(Colour&0xF)*0x11);
	Print('m');
}

void LBR::TextEngine::GrayTxt(u8 TxtGray,u8 BGGray){
	Print("\x1B[3");
	ColourTxt(TxtGray,TxtGray,TxtGray);
	Print(";4");
	ColourTxt(BGGray,BGGray,BGGray);
	Print('m');
}
void LBR::TextEngine::GrayTxt(u8 Gray){
	Print("\x1B[3");
	ColourTxt(Gray,Gray,Gray);
	Print('m');
}
void LBR::TextEngine::GrayBG(u8 Gray){
	Print("\x1B[4");
	ColourTxt(Gray,Gray,Gray);
	Print('m');
}

#define LBRTxEResize 2

LBR::Event LBR::TextEngine::NextEvent(){
	if(_Pixel!=nullptr){
		LBR::Event Event=_Pixel->NextEvent();
		if(Event.Type&LBRWindowRestructurate){
			Event.Window.XSize/=6*LBRTxEResize;
			Event.Window.YSize/=6*LBRTxEResize;
		}
		return Event;
	}
	return EventEngine::NextEvent();
}

bool LBR::TextEngine::UpdateOnEvent(LBR::Event Event){
	if(Event.Type&LBRWindowRestructurate){
		XSize=Event.Window.XSize;
		YSize=Event.Window.YSize;
	}
	if(Event.Type&LBRWindowRestructurate){
		Event.Window.XSize*=6*LBRTxEResize;
		Event.Window.YSize*=6*LBRTxEResize;
	}
	if(_Pixel!=nullptr)_Pixel->UpdateOnEvent(Event);
	return true;
}

void LBR::TextEngine::ClassPrepare(){
	_TxtBufSize=0;
	_TxtBufMSize=0;
	_TxtBuf=nullptr;
	_InBufSize=0;
	_InBufMSize=0;
	_InBuf=nullptr;
	_ScreenBuf=nullptr;

	XSize=40;YSize=25;	//A common grid resolution
	_Pos=0;
	_TxtClr=0xFFFFFF;
	_BakClr=0;
	_Modif=0;

	_Pixel=nullptr;
}

LBR::TextEngine::TextEngine(const LBR::TextEngine &Text){
	ClassPrepare();
	/*_TxtBufSize=Text._TxtBufSize;
	_TxtBuf=Text._TxtBuf;
	_ScreenBuf=Text._ScreenBuf;*/

	XSize=Text.XSize;
	YSize=Text.YSize;
	_Pos=Text._Pos;
	_TxtClr=Text._TxtClr;
	_BakClr=Text._BakClr;
	_Modif=Text._Modif;

	_Pixel=Text._Pixel;
}
LBR::TextEngine::TextEngine(LBR::PixelEngine &Pixel){
	ClassPrepare();

	_Pixel=&Pixel;

	XSize=Pixel.XSize/6/LBRTxEResize;
	YSize=Pixel.YSize/6/LBRTxEResize;

	//v Sets white Unicode text on a background erased on black
	Print("\x1B[?25l\x1B%G\x1B[37;40m\x1B[2J");
}


//v LBR::PixelEngine
LBR::PixelEngine::PixelEngine(u32 XSize_,u32 YSize_){
	//Defaults to 24-bit RGB with 32-bit padding
	ColourPalette=0x0888;
	Padding=32;

	XSize=XSize_;YSize=YSize_;
	PtrXPos=0;PtrYPos=0;

	BitMapSize=((u64)XSize*YSize*Padding+7)/8;
	BitMap=malloc(BitMapSize);

	ConnectWindow(XSize,YSize);
}

bool LBR::PixelEngine::UpdateOnEvent(LBR::Event Event){
	if(Event.Type&LBRWindowRestructurate){
		Resize(Event.Window.XSize,Event.Window.YSize);
	}
	if(Event.Type&(LBRButtonChange|LBRPointerMotion)){
		PtrXPos=Event.Pointer.XPos;
		PtrYPos=Event.Pointer.YPos;
	}
	return true;
}


#include <unistd.h>		//To get read() & write()
#include <sys/stat.h>	//Again, 1 library isn't enough
#include <fcntl.h>		//For terminal input mode control & open()

//v Decodes text too!
bool LBR::TextEngine::Render(){
	if(_Pixel!=nullptr){
		u8 Esc=0;
		u8* EscBeg;

		u32 Chr=0;	//Shortened because widely used
		u8 CharSize=0;

		u8* P=_TxtBuf;
		u8* End=P+_TxtBufSize;
		while(P<End){
			if(*P&0x80){
				if(CharSize){
					Chr<<=6;
					Chr|=*P&0b111111;
					CharSize--;
				}else{
					CharSize=*P&0b100000?*P&0b10000?3:2:1;
					Chr=*P&(0b00111111>>CharSize);
				}
				if(CharSize){
					P++;
					continue;
				}
			}else Chr=*P;
			if(*P==0x1B){
				P++;
				Esc=*P==']'?3:*P=='['?2:1;
				if(Esc==2)P++;
				EscBeg=P;
				continue;
			}

			if(Esc){
				if(Esc&8?Chr=='\a':Chr>0x40){//Needs ST (0x9C) too!
					if((Esc&7)==2){
						if(Chr=='J'){
							if(*EscBeg>='2'){
								_Pixel->Clear();
								_Pos=0;
							}
						}else if(Chr=='C'){
							if(*EscBeg!='C'){
								u64 Int=0;
								bool Neg=*EscBeg=='-';
								if(Neg)EscBeg++;
								while(*EscBeg>='0'&&'9'>=*EscBeg){
									Int=Int*10+*EscBeg++-'0';
								}
								if(Neg){
									_Pos-=MacroMin(Int,_Pos%XSize);
								}else _Pos=MacroMin(Int+_Pos%XSize,XSize-1)+_Pos/XSize*XSize;
							}else _Pos=MacroMin(1+_Pos%XSize,XSize-1)+_Pos/XSize*XSize;
						}else if(Chr=='P'){
							if(*EscBeg!='-'){
								u64 Int=0;
								while(*EscBeg>='0'&&'9'>=*EscBeg){
									Int=Int*10+*EscBeg++-'0';
								}
								while(Int--){
									_Pixel->DrawFullRectangle(
										_Pos%XSize*6*LBRTxEResize,
										_Pos/XSize*6*LBRTxEResize,
										6*LBRTxEResize,6*LBRTxEResize,
										_BakClr
									);
									_Pos++;
									_Pos%=XSize*YSize;
								}
							}
						}else if(Chr=='H'){
							bool Arg=false;
							do{
								u64 Int=0;
								bool Neg=*EscBeg=='-';
								if(Neg)EscBeg++;
								while(*EscBeg>='0'&&'9'>=*EscBeg){
									Int=Int*10+*EscBeg++-'0';
								}
								if(Neg||Int==0)Int=1;
								if(Arg){
									_Pos+=MacroMin(Int,XSize)-1;
								}else{
									_Pos=(MacroMin(Int,YSize)-1)*XSize;
									Arg=true;
								}
							}while(EscBeg++<P);
						}else if(Chr=='G'){
							bool Arg=false;
							u64 Int=0;
							bool Neg=*EscBeg=='-';
							if(Neg)EscBeg++;
							while(*EscBeg>='0'&&'9'>=*EscBeg){
								Int=Int*10+*EscBeg++-'0';
							}
							if(Neg||Int==0)Int=1;
							_Pos=_Pos/XSize*XSize+(MacroMin(Int,XSize)-1);

						}else if(Chr=='d'){
							bool Arg=false;
							u64 Int=0;
							bool Neg=*EscBeg=='-';
							if(Neg)EscBeg++;
							while(*EscBeg>='0'&&'9'>=*EscBeg){
								Int=Int*10+*EscBeg++-'0';
							}
							if(Neg||Int==0)Int=1;
							_Pos=_Pos%XSize+(MacroMin(Int,YSize)-1)*XSize;

						}else if(Chr=='m'){
							u8 Part=0;
							u8 Arg=0;
							u32 Colour=0;
							do{
								u64 Int=0;
								bool Neg=*EscBeg=='-';
								if(Neg)EscBeg++;
								while(*EscBeg>='0'&&'9'>=*EscBeg){
									Int=Int*10+*EscBeg++-'0';
								}
								if(Part==0){
									if(Int<30){

									}else if(Int<50){
										if(Int%10<8){
											Colour=(Int%10)&1?0xFF:0;
											if((Int%10)&2)Colour|=0xFF00;
											if((Int%10)&4){
												Colour|=0xFF0000;
											}else Colour&=0xFFFF;
											if(Int<40){
												_TxtClr=Colour;
											}else _BakClr=Colour;
										}else Part=(Int<40)+1;
									}
								}else if(Part<=2){
									if(Int==2){
										Part+=2;
										Arg=3;
									}else if(Int==5){
										Part+=4;
									}else Part=0;
									Colour=0;
								}else if(Part<=4){//24-bit RGB
									Arg--;
									Colour<<=8;
									if(!Neg)Colour|=MacroMin(Int,255);
									if(!Arg){
										if(Part&1){
											_BakClr=Colour;
										}else _TxtClr=Colour;
										Part=0;
									}
								}else if(Part<=6){//256-Colours
									if(!Neg)Colour=MacroMin(Int,255);
									if(Colour<16){//CGA 16-Colours
										u8 C=Colour;
										Colour=C&1?0xAA:0;
										if(C&2)Colour|=0xAA00;
										if(C&4)Colour|=0xAA0000;
										if(C&8)Colour+=0x555555;
									}else if(Colour<232){//6x6x6 RGB
										u8 C=Colour-16;
										Colour=C%6*0x33;
										Colour|=C/6%6*0x3300;
										Colour|=C/36%6*0x330000;
									}else{//24 Gray
										Colour-=232;
										Colour=((Colour*0xFF/12)+1)/2;//Round
										Colour*=0x010101;
									}

									if(Part&1){
										_BakClr=Colour;
									}else _TxtClr=Colour;
									Part=0;
								}
							}while(EscBeg++<P);
						}
					}else if((Esc&7)==3){
						/*if(Esc){

						}*/
					}
					Esc=0;
				}else if(Chr=='#'&&Esc!=1)Esc|=8;
			}else{
				if(Chr=='\n'||Chr=='\r'){
					_Pos=(_Pos/XSize+(Chr=='\r'?0:1))*XSize;
				}else{
					_Pixel->DrawChar(Chr,
						_Pos%XSize*6*LBRTxEResize,_Pos/XSize*6*LBRTxEResize,
						_TxtClr,_BakClr,
						6,6,LBRTxEResize
					);
					_Pos++;
				}
				_Pos%=XSize*YSize;
			}
			P++;
		}
		_Pixel->Render();
	}else{
		#ifdef LBRPxEngineUseTUI
			write(1,_TxtBuf,_TxtBufSize);
		#else
			return false;
		#endif
	}
	_TxtBufSize=0;
	return true;
}

bool LBR::Frame::UseTextFontFile(const char* FilePath){
	struct stat ST;
	stat(FilePath,&ST);
	s32 FD=open(FilePath,O_RDWR,777);
	if(FD!=-1){
		if(ST.st_size<=0xFFFF&&ST.st_size){
			u64 TFSize=ST.st_size;
			u8* TxtFont=(u8*)malloc(TFSize);
			if(TxtFont!=nullptr){
				read(FD,TxtFont,TFSize);
				close(FD);
				UseTextFont(TxtFont,TFSize);
				return true;
			}else TFSize=0;
		}
		close(FD);
	}
	return false;
}

/*/
 * Functions above ^ are more like algorithmical functions native to C++
 * Functions below v are library dependent
/*/

#ifdef LBRPxEngineUseTUI
#include <sys/ioctl.h>	//For IO mode control
#include <termios.h>	//Same, because 1 library is never enough

LBR::TextEngine::TextEngine(){
	ClassPrepare();

	termios T;

	//v Sets Terminal to catch single chars
	tcgetattr(fileno(stdin),&T);

	T.c_lflag&=~(ICANON|ECHO);//Flags single char without echo

	if(tcsetattr(fileno(stdin),TCSANOW,&T)<0){
		write(1,"Terminal setup failed ('^' )",28);
	}

	//v Sets colour palette for a simpler one to use
	for(u8 i=0;i<16;i++){
		Print("\x1B]4;");
		Print(i);
		Print(";#");
		Print(i&1?i&8?"FF":"AA":"00");
		Print(i&2?i&8?"FF":"AA":"00");
		Print(i&4?i&8?"FF":"AA":"00");
		Print("\a");
	}
	//v Disable cursor and use Unicode
	Print("\x1B[?25l\x1B%G");
	Clear();
	//Print("\x1B[#");
}

LBR::TextEngine::~TextEngine(){
	if(_Pixel==nullptr){
		//v Resets Terminal to normal mode
		termios T;
		tcgetattr(fileno(stdin),&T);
		T.c_lflag|=ICANON|ECHO;

		if(tcsetattr(fileno(stdin),TCSANOW,&T)<0){
			write(1,"Terminal reset failed ('^' )",28);
		}

		fcntl(0,F_SETFL,fcntl(0,F_GETFL)&(~O_NONBLOCK));
		write(1,"\x1B[?25h\n",6);
	}

	if(_InBuf!=nullptr)free(_InBuf);
	if(_TxtBuf!=nullptr)free(_TxtBuf);
	if(_ScreenBuf!=nullptr)free(_ScreenBuf);
}

bool LBR::TextEngine::SelectEvents(u8 Events){
	if(_Pixel==nullptr){
		if((Events^_EventSel)&LBRKeyPress){
			if(_EventSel&LBRKeyPress){
				fcntl(0,F_SETFL,fcntl(0,F_GETFL)&(~O_NONBLOCK));
			}else fcntl(0,F_SETFL,fcntl(0,F_GETFL)|O_NONBLOCK);
		}
	}else _Pixel->SelectEvents(Events);
	_EventSel=Events;
	return true;
}

u16 LBR::TextEngine::GenerateEvents(){//Generates events and return the number of generated events
	if(_Pixel==nullptr){
		u16 OldEventNum=_EventNum;

		if(32&_EventSel){
			struct winsize W;
			ioctl(STDOUT_FILENO,TIOCGWINSZ,&W);

			if(W.ws_col!=XSize||W.ws_row!=YSize){
				LBR::Event Event;

				Event.Window.XSize=W.ws_col;
				Event.Window.YSize=W.ws_row;
				Event.Type=LBRWindowRestructurate;

				LBR::EventEngine::GenerateEvent(Event);
			}
		}

		if(LBRKeyPress&_EventSel){
			char* PA0;

			u8 i;

			u32 Unicode=0;
			u16 KeyCode=0;

			while(1){
				u8 Byte;
				if(read(0,&Byte,1)!=1)break;

				Unicode=Byte;
				KeyCode=0;
				if(_InBufSize){
					if(_InBuf[0]&0x80){
						if(_InBuf[0]&(0x40>>_InBufSize)){
							_InBufSize++;
							_InBuf=(u8*)realloc(_InBuf,_InBufSize);
							_InBuf[_TxtBufSize-1]=Byte;
							continue;
						}else{
							Unicode=0;
							for(u8 j=0;j<_InBufSize;j++){
								i=8;
								while(i--)if(((_InBuf[j]>>i)&1)==0)break;
								while(i--)Unicode=((_InBuf[j]>>i)&1)|(Unicode<<1);
							}
							i=6;
							while(i--)Unicode=((Byte>>i)&1)|(Unicode<<1);

							free(_InBuf);
							_InBufSize=0;
							_InBuf=nullptr;
						}
					}else if(_InBuf[0]==0x1B){
						if(((Byte>>6)==1&&((Byte-1)&0x1F)<0x1A)||Byte=='~'){
							if(Byte=='A'){//Up Y--
								Unicode=0;
								KeyCode=111;
							}else if(Byte=='B'){//Down Y++
								Unicode=0xB;
								KeyCode=116;
							}else if(Byte=='C'){//X++
								Unicode=0;
								KeyCode=114;
							}else if(Byte=='D'){//X--
								Unicode=0x8;
								KeyCode=113;
							}else{
								/*PrintSp("Unknown ESC:");
								for(i=1;i<_InBufSize;i++)Print(_InBuf[i]);
								PrintLn((char)Byte);*/
							}
							free(_InBuf);
							_InBufSize=0;
							_InBuf=nullptr;
						}else{
							_InBufSize++;
							_InBuf=(u8*)realloc(_InBuf,_InBufSize);
							_InBuf[_InBufSize-1]=Byte;
							continue;
						}
					}
				}


				if(Byte==0x1B||(Byte&0x80)){
					if(_InBuf!=nullptr)free(_InBuf);
					_InBufSize=1;
					_InBuf=(u8*)malloc(1);
					_InBuf[0]=Byte;
					continue;
				}


				/*if(Unicode==0x1B){
					EscSize=1;
					Esc=(char*)malloc(1);
					*Esc=0;
					continue;
				}else if(EscSize){
					if(*Esc==0)EscSize=0;
					if(((Unicode>>6)==1&&((Unicode-1)&0x1F)<0x1A)||Unicode=='~'){//Is esc end?

						EscSize=0;
					}else{
						EscSize+=(BNum>>3);
						Esc=(char*)realloc(Esc,EscSize);
						PA0=Esc+EscSize-(BNum>>3);

						for(;BNum;BNum-=8){
							*PA0=UTF8&0xFF;
							UTF8>>=8;
						}
						continue;
					}
				}*/


				LBR::Event Event;
				Event.Type=1;
				Event.Key.State=1;
				Event.Key.Unicode=Unicode;
				Event.Key.Code=KeyCode;

				KeyCode=0;

				LBR::EventEngine::GenerateEvent(Event);
			}
		}
		return _EventNum-OldEventNum;
	}else return 0;
}

bool LBR::TextEngine::ConnectWindow(u32 XSize,u32 YSize){
	//v Resets Terminal to normal mode
	termios T;
	tcgetattr(fileno(stdin),&T);
	T.c_lflag|=ICANON|ECHO;

	if(tcsetattr(fileno(stdin),TCSANOW,&T)<0){
		write(1,"Terminal reset failed ('^' )",28);
	}
	fcntl(0,F_SETFL,fcntl(0,F_GETFL)&(~O_NONBLOCK));
	write(1,"\x1B[?25h\n",6);

	_Pixel=(LBR::PixelEngine*)malloc(sizeof(LBR::PixelEngine));

	_Pixel->ColourPalette=0x0888;
	_Pixel->Padding=32;

	_Pixel->BitMapSize=((u64)XSize*YSize*_Pixel->Padding+7)/8;
	_Pixel->BitMap=malloc(_Pixel->BitMapSize);

	_Pixel->ConnectWindow(XSize,YSize);
	_Pixel->Resize(XSize,YSize);

	_Pixel->SelectEvents(_EventSel);

	//_ScreenBuf=malloc((XSize/8)*(YSize/8)*56);
	if(!_Pixel->UseTextFontFile("Data/TextFont.bin")){
		if(!_Pixel->UseTextFontFile("TextFont.bin")){
			return false;
		}
	}
	return true;
}
#else
LBR::TextEngine::TextEngine(){
	ClassPrepare();
	//v Sets colour palette for a simpler one to use
	for(u8 i=0;i<16;i++){
		Print("\x1B]4;");
		Print(i);
		Print(";#");
		Print(i&1?i&8?"FF":"AA":"00");
		Print(i&2?i&8?"FF":"AA":"00");
		Print(i&4?i&8?"FF":"AA":"00");
		Print("\a");
	}
	//v Disable cursor and use Unicode
	Print("\x1B[?25l\x1B%G");
	Clear();
	//Print("\x1B[#");
}

LBR::TextEngine::~TextEngine(){
	if(_InBuf!=nullptr)free(_InBuf);
	if(_TxtBuf!=nullptr)free(_TxtBuf);
	if(_ScreenBuf!=nullptr)free(_ScreenBuf);
}

bool LBR::TextEngine::SelectEvents(u8 Events){
	_EventSel=Events;
	if(_Pixel!=nullptr){
		_Pixel->SelectEvents(Events);
		return true;
	}
	return false;
}

u16 LBR::TextEngine::GenerateEvents(){
	return 0;
}

bool LBR::TextEngine::ConnectWindow(u32 XSize,u32 YSize){
	_Pixel=(LBR::PixelEngine*)malloc(sizeof(LBR::PixelEngine));

	_Pixel->ColourPalette=0x0888;
	_Pixel->Padding=24;

	_Pixel->BitMapSize=((u64)XSize*YSize*_Pixel->Padding+7)/8;
	_Pixel->BitMap=malloc(_Pixel->BitMapSize);

	_Pixel->ConnectWindow(XSize,YSize);
	_Pixel->Resize(XSize,YSize);
	_Pixel->UseTextFontFile("Data/TextFont.bin");

	_Pixel->SelectEvents(_EventSel);

	//_ScreenBuf=malloc((XSize/8)*(YSize/8)*56);
	return true;
}
#endif // LBRRendererUseTUI

#ifdef LBRPxEngineUseX11
/*/	Troubleshooting:
 * If you experience any issues here with X11, please check the followings:
 * - Check that you're in the right OS with X11 as windowing system
 * - Add -lX11 flag to your compiler to link the library
 * - Install X11
 *		sudo apt install libx11-dev		On Ubuntu-likes
 *		sudo yum install libX11-devel	On Fedora
 *
 * If none of that worked or if you're on an exotic OS:
 * - Go in the top of that file and define LBRRendererUseTUI in favor of LBRRendererUseGUI
 * - Or contact Léolol DB#4584 on Discord
/*/
#include <X11/Xlib.h>		//Provides the Graphical interface
#include <X11/Xutil.h>		//Some utilities
#include <X11/cursorfont.h>	//And Cursor shaping

bool LBR::IOEngine::ConnectWindow(u32 XSize,u32 YSize){
	_Display=XOpenDisplay(0);//Creates a display

	XVisualInfo XVInfo;
	if(!XMatchVisualInfo(_Display,DefaultScreen(_Display),8,PseudoColor,&XVInfo)){
		if(!XMatchVisualInfo(_Display,DefaultScreen(_Display),24,TrueColor,&XVInfo)){
			write(1,"Couldn't setup for 8-bit PseudoColour nor 24-bit TrueColour!\n",61);
			return false;
		}
	}

	XSetWindowAttributes XWinAtt;
	XWinAtt.background_pixel=0;	//Black-ground
	XWinAtt.backing_store=0;	//Meh?
	XWinAtt.bit_gravity=1;		//What the hell is that
	XWinAtt.colormap=XCreateColormap(_Display,DefaultRootWindow(_Display),XVInfo.visual,AllocNone);
	//Changes																			AllocNone

	_Window=XCreateWindow(
		_Display,DefaultRootWindow(_Display),
		0,0,XSize,YSize,0,
		XVInfo.depth,
		InputOutput,
		XVInfo.visual,
		CWColormap|CWBorderPixel|CWBackPixel|CWBitGravity,
		&XWinAtt
	);//Creates a Complex Window

	/*
	if(XVInfo.c_class==PseudoColor&&XVInfo.depth==8){
		XColor colors[256];
		u16	c;
		//v Set the colormap entries
		u8 i=0;do{
			colors[i].pixel = i;
			colors[i].flags = DoRed|DoGreen|DoBlue;
			//^ Those don't need to be set each time

			c=i;colors[i].red=(c<<8)+c;
			c=i;colors[i].green=(c<<8)+c;
			c=i;colors[i].blue=(c<<8)+c;
		}while(i++<255);

		// store the colors to the current colormap
		XStoreColors(_Display,XWinAtt.colormap,colors,256);
	}*/
	XInstallColormap(_Display,XWinAtt.colormap);

	XStoreName(_Display,_Window,LBRDefaultTitle);	//Displays the Window's title

	XMapWindow(_Display,_Window);					//Makes the Window visible
	_GraphicCTX=XCreateGC(_Display,_Window,0,0);	//Creates a Graphics Context

	//v Wait for a MapNotify XEvent for next commands
	XSelectInput(_Display,_Window,StructureNotifyMask);
	while(1){
		XEvent E;
		XNextEvent(_Display,&E);
		if(E.type==MapNotify)break;
	}
	WinConnected=true;
	_ModKeys=0;
	return true;
}

bool LBR::IOEngine::SelectEvents(u8 Events){
	if(WinConnected){
		u32 X11Mask=(Events&15)|
			(Events&LBRPointerMotion?(1<<6):0)|
			(Events&LBRWindowRestructurate?(1<<17):0)|
			(Events&LBRWindowExpose?(1<<15):0);
		XSelectInput(_Display,_Window,X11Mask);
	}else return false;
	return true;
}
u16 LBR::IOEngine::PendingEventsNumber(){
	return (WinConnected?XPending(_Display):0)+_EventNum;
}
LBR::Event LBR::IOEngine::NextEvent(){
	if(WinConnected&&XPending(_Display)){
		LBR::Event Event;
		Event.Type=0;
		XEvent X11Event;
		XNextEvent(_Display,&X11Event);

		if(X11Event.type==KeyPress||X11Event.type==KeyRelease){
			Event.Key.Unicode=XKeycodeToKeysym(
				_Display,
				X11Event.xkey.keycode,
				_ModKeys?1:0
			);
			Event.Key.Code=X11Event.xkey.keycode;
			Event.Pointer.XPos=X11Event.xkey.x;
			Event.Pointer.YPos=X11Event.xkey.y;

			Event.Time=X11Event.xkey.time;

			Event.Key.State=X11Event.type==KeyPress?true:false;
			Event.Type=Event.Key.State?1:2;

			if(Event.Type==2&&XPending(_Display)){//Released but more?
				XEvent NewX11Event;
				XPeekEvent(_Display,&NewX11Event);

				if(NewX11Event.type==KeyPress&&
					NewX11Event.xkey.time==Event.Time&&
					NewX11Event.xkey.keycode==Event.Key.Code
				){
					Event.Key.State=true;
					Event.Type=3;
					XNextEvent(_Display,&NewX11Event);
				}
			}

			if(0xFFE1==Event.Key.Unicode){
				if(Event.Key.State){
					_ModKeys|=1;
				}else _ModKeys&=~1;
			}else if(0xFFE2==Event.Key.Unicode){
				if(Event.Key.State){
					_ModKeys|=2;
				}else _ModKeys&=~2;
			}else if(0xFF0D==Event.Key.Unicode){
				Event.Key.Unicode='\n';
			}else if(0xFF08==Event.Key.Unicode){
				Event.Key.Unicode=0x7F;
			}else if(0xFF1B==Event.Key.Unicode){
				Event.Key.Unicode=0x1B;
			}
		}else if(X11Event.type==ButtonPress||X11Event.type==ButtonRelease){
			//When a button of the mouse is pressed or released
			Event.Pointer.Button=X11Event.xbutton.button;
			Event.Pointer.XPos=X11Event.xbutton.x;
			Event.Pointer.YPos=X11Event.xbutton.y;

			Event.Time=X11Event.xbutton.time;

			Event.Pointer.BtnState=X11Event.type==ButtonPress?true:false;
			Event.Type=Event.Pointer.BtnState?4:8;
		}else if(X11Event.type==MotionNotify){ //When the mouse moves
			Event.Pointer.XPos=X11Event.xmotion.x;
			Event.Pointer.YPos=X11Event.xmotion.y;

			Event.Time=X11Event.xmotion.time;

			Event.Type=16;
		}else if(X11Event.type==ConfigureNotify){	//When the window is restructured
			Event.Window.XPos=X11Event.xconfigure.x;
			Event.Window.YPos=X11Event.xconfigure.y;
			Event.Window.XSize=X11Event.xconfigure.width;
			Event.Window.YSize=X11Event.xconfigure.height;
			Event.Type=32;
		}else if(X11Event.type==Expose){	//When the window is exposed
			Event.Window.XPos=X11Event.xexpose.x;
			Event.Window.YPos=X11Event.xexpose.y;
			Event.Window.XSize=X11Event.xexpose.width;
			Event.Window.YSize=X11Event.xexpose.height;
			Event.Type=64;
		}
		return Event;
	}
	return EventEngine::NextEvent();
}

bool LBR::PixelEngine::Render(){
	//(char* ArrayImage,uint16_t IMGXSize,uint16_t IMGYSize)
	void* ImgCpy=Padding==32?BitMap:ToBitMap(ColourPalette,32);

	Visual* IMGVisual=XDefaultVisual(_Display,0);
	XImage* FinalImage=XCreateImage(_Display,IMGVisual,24,ZPixmap,0,(char*)ImgCpy,XSize,YSize,32,0);
	XPutImage(_Display,_Window,_GraphicCTX,FinalImage,0,0,0,0,XSize,YSize);
	XFlush(_Display);

	if(Padding!=32)free(ImgCpy);
	return true;
}

bool LBR::PixelEngine::SetCursorShape(u32 Shape){
	_Cursor=XCreateFontCursor(_Display,Shape);
	XDefineCursor(_Display,_Window,_Cursor);
	return true;
}

#else

//v Does nothing more than error
bool LBR::IOEngine::ConnectWindow(u32 XSize,u32 YSize){
	return false;
}

bool LBR::IOEngine::SelectEvents(u8 Events){
	return LBR::EventEngine::SelectEvents(Events);
}
u16 LBR::IOEngine::PendingEventsNumber(){
	return _EventNum;
}
LBR::Event LBR::IOEngine::NextEvent(){
	return EventEngine::NextEvent();
}

bool LBR::PixelEngine::Render(){
	return false;
}

bool LBR::PixelEngine::SetCursorShape(u32 Shape){
	return false;
}

#endif
