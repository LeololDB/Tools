#include "Filer.h"
#include "Strings.h"
#include "Maths.h"
#include <unistd.h>//usleep()

/*/Operation
 * 0 Select a file
 * 1 Select a file or directory
/*/
bool LBR::Filer(LBR::TextEngine &Text,LBR::File &Dir,u8 Operation){
	Dir=LBR::ReadDir(Dir.Path);
	if((Dir.Type&7)!=LBRDirFileType)return 1;
	LBR::SortDir(Dir);

	u32 ViewYPos=0;
	u32 CursorYPos=2;
	if(Operation==0){
		if(CursorYPos+3>ViewYPos+Text.YSize-2){
			ViewYPos=CursorYPos+5-Text.YSize;
			if(CursorYPos+3>Dir.Size)ViewYPos=Dir.Size+2-Text.YSize;
		}
	}else CursorYPos=0xFFFFFFFF;

	u8 Refresh=0xFF;

	while(1){
		Text.GenerateEvents();
		while(Text.PendingEventsNumber()){
			LBR::Event Event=Text.NextEvent();
			if(Event.Type&LBRKeyPress){
				u16 KeyCode=Event.Key.Code;
				if(
					Event.Key.Unicode==0x7F||//Uppper
					Event.Key.Unicode=='.'||
					Event.Key.Unicode=='/'||
					Event.Key.Unicode=='\n'
				){
					if(Event.Key.Unicode=='\n'&&CursorYPos>=Dir.Size)return 1;{
						char* Str;
						if(Event.Key.Unicode==0x7F){
							Str=StrCopy(Dir.Path);
							LBR::GetParent(Str);
						}else if(Event.Key.Unicode=='/')Str=(char*)"/";
						else if(Event.Key.Unicode=='.')Str=(char*)".";
						else Str=Dir.Files[CursorYPos].Path;//Always in range

						Dir=LBR::ReadDir((const char*)Str);

						if(Event.Key.Unicode==0x7F)free(Str);
					}
					if((Dir.Type&7)!=LBRDirFileType)return 1;//Not Dir

					LBR::SortDir(Dir);
					ViewYPos=0;
					if(Operation==0){
						CursorYPos=2;
						if(CursorYPos+3>ViewYPos+Text.YSize-2){
							ViewYPos=CursorYPos+5-Text.YSize;
							if(CursorYPos+3>Dir.Size)ViewYPos=Dir.Size+2-Text.YSize;
						}
					}else CursorYPos=0xFFFFFFFF;
					Refresh|=0xFF;
				}else if(Event.Key.Unicode==0x1B||Event.Key.Unicode==0x18){
					return 0;//Quit
				}else if(Event.Key.Unicode=='r'){//Refresh
					Refresh=0xFF;
				}else if(KeyCode==111||KeyCode==81){//Y--
					if(CursorYPos==0xFFFFFFFF){
						CursorYPos=Dir.Size;
						Refresh|=3;
						if(CursorYPos+5>Text.YSize+ViewYPos){
							ViewYPos=CursorYPos+5-Text.YSize;
							if(ViewYPos+Text.YSize-2>Dir.Size)ViewYPos=Dir.Size+2-Text.YSize;
						}
					}
					if(CursorYPos){
						if(KeyCode==81){
							CursorYPos=CursorYPos>Text.YSize-2?
								CursorYPos+2-Text.YSize:0;
						}else CursorYPos--;
						Refresh|=2;
						if(CursorYPos<ViewYPos+2){
							ViewYPos=CursorYPos-2;
							if(CursorYPos<2)ViewYPos=0;
						}
					}
				}else if(KeyCode==116||KeyCode==89){//Y++
					if(CursorYPos==0xFFFFFFFF){
						CursorYPos=1;
						Refresh|=3;
					}
					if(CursorYPos+1<Dir.Size){
						if(KeyCode==89){
							CursorYPos=Text.YSize-2+CursorYPos>=Dir.Size?
								Dir.Size-1:Text.YSize-2+CursorYPos;
						}else CursorYPos++;
						Refresh|=2;
						if(CursorYPos+5>Text.YSize+ViewYPos){
							ViewYPos=CursorYPos+5-Text.YSize;
							if(ViewYPos+Text.YSize-2>Dir.Size)ViewYPos=Dir.Size+2-Text.YSize;
						}
					}
				}
			}
			if(Event.Type&(LBRWindowRestructurate|LBRWindowExpose))Refresh|=0xFF;
			Text.UpdateOnEvent(Event);
		}
		if(Refresh){
			if(Refresh&0x80)Text.Clear();
			Text.GrayTxt(0xAA,0);
			if(Refresh&1){//Window borders
				Text.Print("\x1B[1;1H╒[Filer @ ");
				if(CursorYPos==0xFFFFFFFF)Text.GrayBG(0x33);
				Text.Print(Dir.Path);
				if(CursorYPos==0xFFFFFFFF)Text.GrayBG(0);

				Text.Print(']');

				u16 i=12+StrSizeUTF8(Dir.Path);
				while(i++<Text.XSize)Text.Print("═");
				Text.Print("╕│");

				i=Text.YSize-2;
				while(i--){
					Text.Print("\x1B[");
					Text.Print(Text.XSize-2);
					Text.Print(i?"C││":"C│╘");
				}
				for(i=2;i<Text.XSize;i++)Text.Print("═");
				Text.Print("╛");
			}

			if(Refresh&2){
				Text.Print("\x1B[2;2H");
				u16 N=ViewYPos;

				u16 End=Text.YSize+N-2;
				if(End>Dir.Size)End=Dir.Size;

				u8 Margin=3;{
					u64 Num=Dir.Size/100;
					while(Num/=10)Margin++;
				}
				while(N<End){
					LBR::File &File=Dir.Files[N];

					const u16 Clr[]={
						0x444,0x555,
						0x800,0x830,
						0x064,0x460,
						0x055,0x455,

						0x999,0xAAA,
						0xF00,0xF50,
						0x0C8,0x8C0,
						0x0BB,0x8AA
					};

					Text.Colour12bTxt(
						Clr[File.Type>15?8:File.Type|8],
						CursorYPos==N?0x444:0x222
					);

					{
						u16 Num=N;
						u8 i=1;
						while(Num/=10)i++;
						while(i++<Margin)Text.Print(' ');
					}
					Text.Print(N);
					Text.Print(' ');

					Text.Colour12bTxt(
						Clr[(File.Type>15?0:File.Type&7)|(File.Name[0]=='.'?0:8)],
						CursorYPos==N?0x333:0
					);

					u16 i=0;
					u16 XSize=Text.XSize-Margin-19;
					char* P=File.Name;
					while(*P){
						if(*P&0x80?*P&0x40:1)i++;
						if(i>=XSize)break;
						Text.Print(*P++);
					}
					if(*P!=0){
						//v Looks if there is only 1 char left
						bool Under=true;
						char* PS=P++;
						while(*P&&Under){
							if(*P&0x80?*P&0x40:1)Under=false;
							P++;
						}
						if(Under){
							while(*PS)Text.Print(*PS++);//Adds the last char
						}else Text.Print("…");
					}
					if(false){
						Text.Print("\x1B[");
						Text.Print(XSize-i+1);
						Text.Print('C');
					}else for(;i<=XSize;i++)Text.Print(' ');

					const char* Names[]={
						"File?","File",
						"Pipe","Socket",

						"SymboLink","Dir",
						"RawDev","BufDev"
					};
					i=File.Type>15?0:File.Type&7;
					if(File.Type&8)Text.Print("Pseudo");
					Text.Print(Names[i]);
					i=StrSize(Names[i])+(File.Type&8?6:0);
					if(File.Type!=LBRDirFileType&&!(File.Type&8)){
						u64 Size=File.Size;
						u8 j=0;if(Size>>19)for(;Size>>16;j++)Size>>=10;
						Text.Print(' ');
						char* Str=IntToStr(Size,'\'');
						Text.Print(Str);
						Text.Print(' ');
						if(j){
							Text.Print("kMGTPEZY"[j-1]);
							Text.Print('i');
							i+=2;
						}
						Text.Print('B');
						i+=StrSize(Str)+3;
						free(Str);
					}
					if(true)for(;i<15;i++)Text.Print(' ');

					Text.Print("\n\x1B[C");
					N++;
				}

				if(CursorYPos>=Dir.Size&&CursorYPos&&CursorYPos!=0xFFFFFFFF){
					CursorYPos=Dir.Size-1;
					Refresh=2;
				}else Refresh=0;
			}else Refresh=0;

			Text.Render();
		}
		usleep(16666);//Good enough for the purpose
	}
	return 0;
}
