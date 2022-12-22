/*/Functions used to browse files
 * See all comments to know what function does what or guess yourself! :P
 *
 * This library isn't meant to be cross-platform.
 * All of the library's code is provided by Léolol DB.
 * This library has no license so you're free to do what you wants with the code!
 * If you have used the code in one of your project, please credit, that would be nice!
 *
 * Version 0.1.5.9
 * In development since the 4/04/02/2021 (18662)
/*///By Léolol DB

#ifndef FilesH
#define FilesH

#include <cstdint>	//Provides more precise int sizes
#include <malloc.h>	//Memory allocation

#define LBRRegFileType			1
#define LBRPipeFileType			2
#define LBRSocketFileType		3
#define LBRSymboLinkFileType	4
#define LBRDirFileType			5
#define LBRRawDevFileType		6
#define LBRBufDevFileType		7

#include "IO.h"

namespace LBR{
	bool FPathAdd(char* &Path,const char* File);	//Similar to StrAdd(Path,File) but may simplify path
	bool GetParent(char* &Path);
	/*/
	 *  Types:
	 * 0.Invalid or unrecognized file (error)
	 * 1.Regular  File
	 * 2.Pipe
	 * 3.Socket
	 * 4.SymboLink
	 * 5.Directory
	 * 6.Raw Device
	 * 7.Buffer Device
	/*/

	struct File{
		File();
		File(const char* FilePath);

		File(const File &NewFile);	//Copy
		~File();	//Destroy & Clear

		void Restructurate();//Reset
		void Destructurate();//Clear

		bool ReadStats();
		bool PathAdd(const char* File);

		File& operator =(const File &OldFile){
			if(this==&OldFile)return *this;

			Destructurate();

			Size=OldFile.Size;
			Type=OldFile.Type;
			if(OldFile.LinkC==nullptr){
				{
					char* P0=OldFile.Path;
					while(*P0++);
					Path=(char*)malloc(P0-OldFile.Path);

					P0=OldFile.Path;
					char* P1=Path;
					do *P1++=*P0;while(*P0++);//+0Term
				}
				Name=OldFile.Name-OldFile.Path+Path;

				if(Type==LBRDirFileType&&OldFile.Files!=nullptr){
					Files=(File*)malloc(Size*sizeof(File));
					for(uint64_t i=0;i<Size;i++){
						Files[i].Restructurate();
						Files[i]=OldFile.Files[i];
					}
				}else Files=nullptr;
				LinkC=(uint32_t*)malloc(4);
				*LinkC=0;
			}else{
				Path=OldFile.Path;
				Name=OldFile.Name;
				LinkC=OldFile.LinkC;
				Files=OldFile.Files;
				(*LinkC)++;
			}

			return *this;
		}
		operator char*(){
			return Path;
		}
		File operator [](int Number){
			File* F=(File*)malloc(sizeof(File));
			*F=Files[Number];
			return *F;
		}

		char* Name=nullptr;	//Should always point to somewhere in the var Path
		char* Path=nullptr;

		File* Files=nullptr;	//The files in the directory if it is one

		uint64_t Size=0;	//FileSize in Bytes
		uint8_t Type=0;		//FileType

		uint32_t* LinkC;	//How many time the data is linked (to know if to free)
	};

	void* ReadFile(const char* FilePath);
	bool ReadFile(const char* FilePath,uint8_t* &Data,uint64_t &Size);
	inline bool ReadFile(const char* FilePath,void** Data,uint64_t* Size){
		return ReadFile(FilePath,(uint8_t* &)(*Data),*Size);
	}
	bool WriteFile(const char* FilePath,void* Data,uint64_t Size);

	LBR::File ReadDir(const char* DirPath);
	bool SortDir(LBR::File &Dir);
}
#endif
