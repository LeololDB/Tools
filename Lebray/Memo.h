/*/This library provide stupid memory management that check block integrity.
 * See the comments to know what function does what or guess yourself! :P
 *
 *  -=[ Compiling ]=-
 * This library only uses Lebray/Maths.h and Lebray/IO.h provided by Léolol DB
 * 
 * You need to compile Memo.cc using CoCoder to link the result.
 *
 * You can optionally define
 *	LBRMemoDebug to print every Memo and Remo calls
 *	LBRMemoUnprotect to disable memory protection if your code is perfect
 *
 *	-=[ License ]=-
 * This library is under the terms of the Lebray Open-Source License NW:
 *
 * This library was made in the hope to be useful WITHOUT ANY WARRANTY,
 * not even the warranty for the library to work nor to fit any purpose.
 * The user is responsible for using that library.
 *
 * Please credit that would be nice ('v' )
 *
 *	-=[ Credits ]=-
 * All of the library's code is provided by Léolol DB.
 *
 * Version 0.0.0.31
 * In development since the 3/18/05/2022
/*///By Léolol DB
#include "Maths.h"
void* Memo(u64 Size);
#define Remo(...) LBR::RemoLn(__LINE__,__VA_ARGS__)
#define MemoCheck(P) LBR::MemoCheckLn(__LINE__,P)
inline u64 MemoSize(const void* P){
	return P==nullptr?0:((u64*)P)[-1];
}
namespace LBR{
	void* RemoLn(u64 Ln,void* P,u64 Size=0,s64 Shift=0);
	void MemoCheckLn(u64 Ln,void* P);//exit(0) if invalid+Message

	inline void* RemoLn(u64 Ln,s64 Shift,void* P,u64 Size){
		return RemoLn(Ln,P,Size,Shift);
	}
	inline void* RemoLn(u64 Ln,s64 Shift,void* P){
		if(Shift<=0||u64(Shift)<MemoSize(P))return RemoLn(Ln,P,MemoSize(P)-Shift,Shift);
		else return nullptr;
	}
}

