/*/
 * Functions used to debug especially segmentation faults
 * See all comments to know what function does what or guess yourself! :P
 *
 * All of the library's code is provided by Léolol DB.
 *
 * Version 0.0.0.1
 * In development since the 5/07/02/2020
/*///By Léolol DB

#ifndef LBRSegFault
#define LBRSegFault

#include <cstdarg>	//For VaList
namespace LBR{
	namespace DBG{//Debug
		void TraceLine(unsigned long Ln);
		void InitSegFault();

		struct Tracker{
			Tracker(const char* Func,...);
			~Tracker();
		};
	}
}


#define LBRTraceLn() LBR::DBG::TraceLine(__LINE__)
#define LBRTraceFunc(...) LBR::DBG::TraceLine(__LINE__);LBR::DBG::Tracker LiLtRaCkErBrO(__VA_ARGS__)

#endif
