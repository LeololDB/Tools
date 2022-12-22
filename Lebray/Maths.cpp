/*/Functions used to make mathematical operations
 * Go to Maths.h to have more documentation
 * Version 0.0.0.1
/*///By Léolol DB

#include "Maths.h"
#include <stdlib.h> //To get more memory control


bool ArrayObjectGet(void* Array,u32 ArraySize,void* Object,u8 &ObjectSize,u8 ObjectPos){
	u8* P0=(u8*)Array,*P1=(u8*)Object;
	u32 Pos=0;
	u8 Count=0;
	u8 i;
	//v Got to Object's position
	while(Count<ObjectPos){
		Pos+=*P0+1;
		if(Pos>=ArraySize)return false;//Pointer overflow error
		Count++;P0+=*P0+1;
	}
	ObjectSize=*P0++;
	//v Copy Object
	for(i=0;i<ObjectSize;i++){
		Pos++;
		if(Pos>=ArraySize)return false;//Pointer overflow error
		*P1++=*P0++;
	}
	while(i<8){
		*P1++=0;
		i++;
	}
	return true;
}

bool ArrayObjectSet(void* Array,u32 ArraySize,void* Object,u8 ObjectSize,u8 ObjectPos){ //Assumes which the ArraySize is the allocated Size of Array
	u8* P0=(u8*)Array,*P1;
	u32 Pos=0;
	u8 Count=0;
	u8 i,A;
	while(Count<ObjectPos){
		Pos+=*P0+1;
		if(Pos>=ArraySize)return false;//Pointer overflow error
		Count++;P0+=*P0+1;
	}
	if(ObjectSize>*P0){
		//v Shifts every elements
		A=ObjectSize-*P0;
		P1=(u8*)Array+ArraySize-1;
		while(P1!=P0){
			*P1=*(P1-A);
			P1--;
		}
		//v Copies Object to Array
		P1=(u8*)Object;
		Pos++;*P0++=ObjectSize;
		for(i=0;i<ObjectSize;i++){
			if(Pos>=ArraySize)return false;//Pointer overflow error
			Pos++;*P0++=*P1++;
		}
	}else{
		//v Copies Object to Array
		P1=(u8*)Object;
		A=*P0-ObjectSize;
		Pos++;*P0++=ObjectSize;
		for(i=0;i<ObjectSize;i++){
			if(Pos>=ArraySize)return false;//Pointer overflow error
			Pos++;*P0++=*P1++;
		}
		//v Shifts every elements
		if(A>0){
			Pos+=A;
			while(Pos<ArraySize){//Until a pointer overflow
				*P0=*(P0+A);
				Pos++;P0++;
			}
		}
	}

	return true;
}
//*/

bool IsHittingBox(u16 HBXPos,u16 HBYPos,u16 HBXSize,u16 HBYSize,
		u16 OXPos,u16 OYPos){ //Checks if Object (O) touches HitBox (HB)
	return OXPos>=HBXPos&&OYPos>=HBYPos&&OXPos<=HBXPos+HBXSize&&OYPos<=HBYPos+HBYSize;
}

bool GetBit(u8 Number,u8 Pos){
	return (Number>>Pos)&1;
}
bool GetBit(void* Number,u8 Pos){
	return (((u8*)Number)[Pos>>3]>>(Pos&7))&1;
}

u8 SetBit(u8 Number,u8 Pos,bool Bit){
	if(Bit)return Number|(1<<Pos);
	else return Number&(~(1<<Pos));
}
u8 SetBit(void* Number,u8 Pos,bool Bit){
	if(Bit)return ((u8*)Number)[Pos>>3]|=(1<<(Pos&7));
	else return ((u8*)Number)[Pos>>3]&=(~(1<<(Pos&7)));
}

u8 GetSUInt(u8 Number,u8 Size,u8 Pos){ //Gets a uint with a uncommon binary size
	return (Number>>Size*Pos)%(1<<Size);
}

u8 SetSUInt(u8 &Number,u8 Size,u8 Pos,u8 SUInt){ //Sets a uint of an uncommon binary size
	return ((Number>>Size*(Pos+1))<<Size*(Pos+1))|(Number%(1<<Size*Pos))|(SUInt<<Size*Pos);
}


//v After 6/06/03/2020

u8 LBR::Decimals(u64 Num){
	Num/=10;
	u8 Size=1;
	while(Num){
		Size++;
		Num/=10;
	}
	return Size;
}

//v After 1/12/04/2020

s64 StrToInt(const char* Str){
	s64 Int=0;
	bool Neg=(*Str=='-');
	while(*Str){
		if(*Str>='0'&&'9'>=*Str){
			Int*=10;
			Int+=*Str-'0';
		}
		Str++;
	}
	if(Neg){
		return -Int;
	}else return Int;
}
char* IntToStr(u64 Int){
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
char* SIntToStr(s64 Int){
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
char* IntToStr(u64 Int,char Separator){
	u64 A=Int;
	u8 S=0;do S++;while(A/=10);		//Digit count of Number
	S+=(S-1)/3;		//Counts separators

	char* Str=(char*)malloc(S+1);	//+1 for terminator
	if(Str==nullptr)return Str;
	Str+=S;
	*Str--=0;	//Appends the 0 terminator

	S=0;do{
		if(S++==3){
			S=0;
			*Str--=Separator;
		}
		*Str--=Int%10+'0';	//Puts the digit
		Int/=10;
	}while(Int);

	return Str+1;
}


void* CopyBlock(const void* Block,u64 Size){
	u8* NewBlock=(u8*)malloc(Size);
	u8* P0=NewBlock;
	const u8* P1=(const u8*)Block;
	while(Size--)*P0++=*P1++;
	return (void*)NewBlock;
}
