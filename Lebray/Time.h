/*/░░░░░░░░░░
*   ██████████░░                         ▄▄▀▀▀▀▄▄
*     ░░██      ██░░░░░░░░      ░░      ▄▀  █   ▀▄
*     ░░██    ░░  ░░████████  ░░  ██    █   ██▄▄ █
*     ░░██    ░░██░░██░░██░░██  ██░░██  ▀▄      ▄▀
*       ██      ██  ██  ██  ██    ████   ▀▀▄▄▄▄▀▀
*
*
* Functions used to make time operations
* See all comments to know what function does what or guess yourself! :P
*
* This library is cross-platform.
* All of the library's code is provided by Léolol DB.
* This library has no license so you're free to do what you wants with the code!
* If you have used the code in one of your project, please credit, that would be nice!
*
* Version 0.0.12.1
* In development since the 5/07/02/2020
/*///By Léolol DB

#ifndef Time_h
#define Time_h

#include <stdint.h>

uint64_t GetNanoTime();
uint64_t GetMicroTime();
uint64_t GetMilliTime();//Return the millisecond time-stamp
uint64_t GetSecTime();

inline uint64_t GetDayTimeStamp();//Return the day time-stamp
inline uint32_t GetDayMilliTime();//Return the millisecond count of the day
inline uint8_t GetDayHourTime();

inline bool IsLeapYear(uint16_t Year);//Return true if Year is a leap year

inline uint16_t DaysInYear(uint16_t Year);//Returns the number of days in Year
inline uint8_t DaysInMonth(uint8_t Month,uint16_t Year);//Returns the number of days in Month
inline uint16_t DaysInLTYear(uint16_t Year);//Returns the number of days in LIYear
inline uint8_t DaysInLTMonth(uint8_t Month,uint16_t Year);//Returns the number of days in LIMonth

bool TimeStampToDate(uint8_t* Day,uint8_t* Month,uint16_t* Year,uint64_t TimeStamp);
bool TimeStampToVersion(uint8_t* Day,uint8_t* Month,uint16_t* Year,uint64_t TimeStamp);

//v Inlines

inline uint64_t GetDayTimeStamp(){
	return GetSecTime()/86400;
}
inline uint32_t GetDayMilliTime(){
	return GetMilliTime()%86400000;
}
inline uint8_t GetDayHourTime(){
	return GetSecTime()/3600%24;
}

inline bool IsLeapYear(uint16_t Year){//Is Year a Leap Year?
	return Year%4?0:(Year%100?1:(Year%400==0));
}

inline uint16_t DaysInYear(uint16_t Year){
	return IsLeapYear(Year)+365;
}
inline uint8_t DaysInMonth(uint8_t Month,uint16_t Year){
	return Month==2?(IsLeapYear(Year)?29:28):30+((Month&1)^(Month>=8));
}
inline uint16_t DaysInLTYear(uint16_t Year){//Arbitrary time measure
	return (uint64_t)Year*97%400<97?366:365;
}
inline uint8_t DaysInLTMonth(uint8_t Month,uint16_t Year){//Arbitrary time measure
	return ((uint64_t)Year*12+Month)*2097%4800<2097?31:30;
}

#endif

