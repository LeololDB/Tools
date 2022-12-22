/*/	-=[ License ]=-
 * This library is under the terms of the Lebray Open-Source License NW:
 *
 * This library was made in the hope to be useful WITHOUT ANY WARRANTY,
 * not even the warranty for the library to work nor to fit any purpose.
 * The user is responsible for using that library.
/*///By Léolol DB
#include "Memo.h"
#include "IO.h"

//v Created the 3/18/05/2022
#include <malloc.h>
void* Memo(u64 Size){
	void* P=nullptr;
	#ifndef LBRMemoUnprotect
		u8* Mem=(u8*)malloc(Size+16);
		if(Mem!=nullptr){
			*((u64*)Mem)=Size;
			*((u8**)(Mem+Size+8))=Mem;
			P=(void*)(Mem+8);
		}
	#else
		P=malloc(Size);
	#endif
	#ifdef LBRMemoDebug
		Print("Memo(",Size,")=x");
		PrintHex((u64)P);
		PrintLn();
	#endif
	return P;
}

//v Created the 3/18/05/2022
#include <stdlib.h>
void* LBR::RemoLn(u64 Ln,void* P,u64 Size,s64 Shift){
	if(P!=nullptr){
		void* Ret=nullptr;
		#ifndef LBRMemoUnprotect
			u8* Mem=(u8*)P-8;
			if(*((u8**)(Mem+*((u64*)Mem)+8))!=Mem){
				Print("	Block Corruption!\nLine ",Ln,": Remo(x");
				PrintHex(u64(P));
				Print(',',Size,
					");\n\nBlock integrity check failed, u64[-1]P says that the block is ",
					*((u64*)Mem),
					" bytes,\nso u8[P+Size]u64 should point to P-8 (=x");
				PrintHex(u64(Mem));
				Print(") but x");
				PrintHex(*(u64*)(Mem+*((u64*)Mem)+8));
				PrintLn("\n\nBlock's extremities were surely over-written after its last Remo() or Memo().");
				exit(0);
			}
			if(Size){
				Mem=(u8*)realloc(Mem,Size+16);
				if(Mem!=nullptr){
					*((u64*)Mem)=Size;
					*((u8**)(Mem+Size+8))=Mem;
					Ret=(void*)(Mem+8);
				}
			}else free(Mem);
		#else
			if(Size)Ret=realloc(P,Size);
			else free(P);
		#endif
		#ifdef LBRMemoDebug
			Print("Remo(x");
			PrintHex((u64)P);
			Print(',',Size,")=x");
			PrintHex((u64)Ret);
			PrintLn();
		#endif
		return Ret;
	}else return Memo(Size);
}
