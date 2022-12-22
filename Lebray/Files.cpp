/*/Functions used to browse files
 * Go to Files.h to have more documentation
/*///By Léolol DB

#include "Files.h"
#include "Strings.h"
#include "Memo.h"

#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef char c8;
typedef const char cc8;

/** @brief LBR::FPathAdd(Path,File) Is a close to StrAdd(Path,File) but for directories
 *
 * @param	char* &Path	is the directory's path to change
 * @param	cc8* File	is then appended at the end of Path
 *
 * @return	1 on success, 0 on failure.
 *
 * @example
 *	char* Path=StrCopy("/a");	//Use String.h for StrCopy()
 *	cc8* File="b";
 *	LBR::FPathAdd(Path,File)
 *	PrintLn(Path);				//Expects to return /a/b
**///Modified the 1/14/06/2021
bool LBR::FPathAdd(char* &Path,cc8* File){
	char* P=Path;
	u64 Size=0;
	if(Path!=nullptr){
		while(*P)P++;
		Size=P-Path;
	}

	bool Slash=P>Path&&*(P-1)!='/';	//True when Path isn't empty and doesn't end with /

	Path=(char*)realloc(Path,Size+StrSize(File)+1+Slash);	//+1 because we still have a 0 to add
	if(Path==nullptr)return 0;								//May error out if out of RAM
	P=Path+Size;											//Restore P in the case Path was moved

	if(Slash)*P++='/';			//Adds a slash if necessary

	while(*File)*P++=*File++;	//Copies File to P
	*P=0;						//Appends the 0 terminator

	return 1;					//Returns true if everything right
}
bool LBR::GetParent(char* &Path){
	char* P=Path;
	while(*P)P++;						//P is end of Path

	bool Slash=1;
	if(P>Path){
		if(*(P-1)=='/')Slash=0;
		P-=1-Slash;
		if(P-Path<2||(*(P-3)=='/'&&*(P-2)=='.'&&*(P-1)=='.')){	//if /.. is found at end of Path
			if(Slash)StrAdd(Path,'/');
			return StrAdd(Path,"..");
		}
		P+=1-Slash;	//P is end of Path

		while(*P!='/'&&P>Path)P--;
		if(P==Path)P++;

		*P=0;

		Path=(char*)realloc(Path,P-Path+1);	//+1 because we still have a 0 to add
		if(Path==nullptr)return 0;			//Might error out for some reasons
		return 1;							//Else Returns true if everything right
	}else return StrAdd(Path,"..");	//Lighter than OpenFile
}


/** @brief This function reads a file and put it on Data.
 *
 * @fixme xxx Not updated!
 *
 * @param	FilePath is a string with the Path leading to the desired file.
 *
 * @return	1 on success, 0 on failure.
/*///Created the 4/04/03/2021
inline char* GetFileName(char* FilePath){
	if(FilePath==nullptr)return nullptr;
	char* P=FilePath;

	while(*P){
		if(*P=='/'&&P[1])FilePath=P+1;
		P++;
	}
	return FilePath;
}


/** @brief This function just initiates the member with empty parameters
**///Created the 4/04/03/2021 & Modified the 5/05/03/2021 & the 5/04/06/2021
LBR::File::File(){
	Restructurate();
}
/** @brief This function just initiates the member with the FilePath.
 *
 * @param	FilePath is a string with the path of the file use.
 *			The file isn't yet to be red.
**///Created the 5/04/06/2021
LBR::File::File(cc8* FilePath){
	Path=StrCopy(FilePath);
	Name=GetFileName(Path);

	LinkC=(u32*)malloc(4);
	*LinkC=0;

	Files=nullptr;
	Size=0;
	Type=0;
}

/** @brief This is the a constructor?
 *
 * @param	OldFile is the File to to use as pattern.
**///Created the 4/04/03/2021 & Modified the 5/05/03/2021 & the 5/04/06/2021
LBR::File::File(const File &OldFile){
	Size=OldFile.Size;
	Type=OldFile.Type;
	if(OldFile.LinkC==nullptr){
		Path=StrCopy(OldFile.Path);
		Name=OldFile.Name-OldFile.Path+Path;

		if(Type==LBRDirFileType&&Files!=nullptr){
			Files=(File*)malloc(Size*sizeof(File));
			for(u64 i=0;i<Size;i++){
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
}

/** @brief This is the destructor, it frees all data if no longer linked
/*///Created the 6/08/05/2021
LBR::File::~File(){
	Destructurate();
}
/** @brief Destructor that frees all uneeded data
/*/
void LBR::File::Destructurate(){
	if(LinkC!=nullptr){
		if(*LinkC==0){
			if(Path!=nullptr)free(Path);//Frees Name too
			if(Type==LBRDirFileType&&Files!=nullptr){
				while(Size--)Files[Size].Destructurate();
				free(Files);
			}
			if(LinkC!=nullptr)free(LinkC);
		}else (*LinkC)--;
	}
	Restructurate();
}
/** @brief Resets the whole class: use it when allocating memory!
/*/
void LBR::File::Restructurate(){
	Path=nullptr;
	Name=nullptr;
	Files=nullptr;
	LinkC=nullptr;
	Size=0;
	Type=0;
}

bool LBR::File::PathAdd(cc8* File){
	char* NewPath=StrCopy(Path);
	Destructurate();

	bool Ret=LBR::FPathAdd(NewPath,File);
	Path=NewPath;
	Name=GetFileName(Path);

	LinkC=(uint32_t*)malloc(4);
	*LinkC=0;
	return Ret;
}

/** @brief This function sorts the given directory contents.
 *
 * @param	Dir is a directory which had been red.
 *
 * @return	Returns true on success else failure.
/*///Created the 5/05/03/2021
bool LBR::SortDir(LBR::File &Dir){
	if(Dir.Type!=LBRDirFileType||Dir.Size==0)return 0;
	LBR::File* Files=(LBR::File*)malloc(Dir.Size*sizeof(LBR::File));

	LBR::File File;

	u64 i,j,k;
	char* P0;
	char* P1;

	for(i=0;i<Dir.Size;i++)Files[i].Restructurate();

	for(i=0;i<Dir.Size;i++){
		File=Dir.Files[i];j=0;
		for(j=0;j<i;j++){
			if((File.Type&7)>(Files[j].Type&7))break;
			if((File.Type&7)<(Files[j].Type&7))continue;

			P0=File.Name;
			P1=Files[j].Name;

			while(*P0==*P1&&*P0){
				P0++;P1++;
			}
			if(*P0<*P1)break;
		}
		for(k=i;k>j;k--)Files[k]=Files[k-1];
		Files[j]=File;
	}
	for(i=0;i<Dir.Size;i++)Dir.Files[i].Destructurate();
	free(Dir.Files);
	Dir.Files=Files;

	return 1;
}


#include <sys/stat.h>	//File stats
bool LBR::File::ReadStats(){
	struct stat ST;
	if(stat(Path,&ST)==-1){
		Size=0;
		Type=0;
		return false;
	}

	Type=ST.st_mode>>12;
	Type=Type==8?1:	//File
		Type==1?2:	//Pipe
		Type==12?3:	//Socket
		Type==10?4:	//Symbolink
		Type==4?5:	//Dir
		Type==2?6:	//RawDev
		Type==6?7:	//BufDev
				0;	//Unknown
	if(Type!=LBRDirFileType)Size=ST.st_size;
	if(ST.st_blocks==0&&ST.st_size==0)Type|=8;
	return true;
}

#include <errno.h>	//errno var
#include <fcntl.h>	//open();
#include <unistd.h>	//read();write();

/** @brief This function reads a file and put it on Data.
 *
 * @param	FilePath is a string with the path of the file to read from.
 * @return	the Memo block corresponding to the file's data.
/*///Created the 4/04/03/2021 modified the 5/13/05/2022 and 4/14/07/2022
void* LBR::ReadFile(cc8* FilePath){
	struct stat ST;
	stat(FilePath,&ST);
	u64 Size=ST.st_size;

	int FD=open(FilePath,O_RDONLY,0755);
	if(FD==-1)return nullptr;
	u8* Data=nullptr;
	if(Size){
		Data=(u8*)Memo(Size);
		if(Data==nullptr)return Data;
		read(FD,Data,Size);
	}else{
		u64 BS=ST.st_blksize;//BlockSize
		while(1){
			Data=(u8*)Remo(Data,Size+BS);
			s32 R=read(FD,Data+Size,BS);
			if(R){
				if(R==-1){
					if(errno!=EAGAIN)break;
				}else Size+=R;
			}else break;
		}
		Data=(u8*)Remo(Data,Size);
	}
	close(FD);
	return Data;
}
bool LBR::ReadFile(cc8* FilePath,u8* &Data,u64 &Size){
	Data=(u8*)ReadFile(FilePath);
	Size=MemoSize(Data);
	return Size;//This therefore doesn't support 0 byte files
}


#include <sys/syscall.h>
/** @brief This function writes Data onto a file.
 *
 * @param	FilePath is a string with the path of the file to write.
 * @param	Data is the data block to write.
 * @param	Size is the size of the data block to write.
 *
 * @return	1 on success, 0 on failure.
/*///Created the 4/04/03/2021
bool LBR::WriteFile(cc8* FilePath,void* Data,u64 Size){
	int FD=open(FilePath,O_WRONLY|O_CREAT|O_TRUNC,0755);

	//v Build the directory tree if required
	if(FD==-1){
		char* Dir=StrCopy(FilePath);
		char* P=Dir;

		while(1){
			do P++;while(*P&&*P!='/');
			if(*P=='/'){
				*P=0;
				if(mkdir(Dir,0755)&&errno!=EEXIST){//Expects a lot of exist
					free(Dir);
					return 0;
				}
				*P='/';
			}else break;
		}
		free(Dir);
		FD=open(FilePath,O_WRONLY|O_CREAT|O_TRUNC,0755);
	}

	if(FD!=-1){
		u64 WSize=write(FD,Data,Size);
		return WSize==Size;
	}
	return 0;
}


/** @brief This function reads the desired directory's contents.
 *
 * @param	FilePath is a string with the path of the dir to read from.
 *
 * @return	Returns an LBR::Dir containing each STD::File in the directory.
/*///Created the 4/04/03/2021 & Modified the 5/05/03/2021 & Modified the 3/21/04/2021
LBR::File LBR::ReadDir(cc8* DirPath){
	LBR::File Dir(DirPath);

	s32 FD=open(DirPath,O_RDONLY|O_DIRECTORY);
	if(FD==-1)return Dir;
	Dir.Type=LBRDirFileType;
	char* Buf=(char*)malloc(512);
	while(1){
		u64 ReadC=syscall(SYS_getdents,FD,Buf,512);
		if(ReadC+1<=1)break;

		char* P0=Buf;
		char* End=Buf+ReadC;
		while(P0<End){
			char* P1=P0+16;//u64 INO;s64 OFF;u16 DSize;char[] Str;
			u16 Size=*(u16*)P1;
			P1+=2;

			char* Path=StrCopy(DirPath);
			LBR::FPathAdd(Path,P1);

			Dir.Size++;
			Dir.Files=(LBR::File*)realloc(Dir.Files,Dir.Size*sizeof(LBR::File));

			LBR::File &File=Dir.Files[Dir.Size-1];
			File.Path=Path;
			File.Name=GetFileName(Path);

			File.Files=nullptr;
			File.LinkC=(u32*)malloc(4);
			*File.LinkC=0;

			File.Size=0;
			File.ReadStats();

			P0+=Size;
		}
	}
	free(Buf);
	close(FD);

	return Dir;
}
