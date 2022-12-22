/*/Functions used to make waveforms
* Go to Time.h to have more documentation
/*///By Léolol DB

#include "Time.h"
#include <stdint.h>//Provides more int types

#include <time.h>//Provides the current time

uint64_t GetNanoTime(){
	struct timespec TS;
	if(clock_gettime(CLOCK_REALTIME,&TS))return 0;
	return TS.tv_sec*1000000000+TS.tv_nsec;
}
uint64_t GetMicroTime(){
	struct timespec TS;
	if(clock_gettime(CLOCK_REALTIME,&TS))return 0;
	return TS.tv_sec*1000000+TS.tv_nsec/1000;
}
uint64_t GetMilliTime(){
	struct timespec TS;
	if(clock_gettime(CLOCK_REALTIME,&TS))return 0;
	return TS.tv_sec*1000+TS.tv_nsec/1000000;
}
uint64_t GetSecTime(){
	struct timespec TS;
	if(clock_gettime(CLOCK_REALTIME,&TS))return 0;
	return TS.tv_sec;
}

bool TimeStampToDate(uint8_t* Day,uint8_t* Month,uint16_t* Year,uint64_t TimeStamp){
	*Month=1;*Year=1970;//Reset date
	/*if(TimeStamp>=730){
		*Year=1972;
		TimeStamp-=730;
		while(1){
			//uint16_t DIn4Year=(*Year%100?1:(*Year%400==0))?1461:1460);
			//if(DIn4Year>TimeStamp)break;
			(*Year)+=4;
			//TimeStamp-=DIn4Year;
		}
	}*/
	while(DaysInYear(*Year)<=TimeStamp)TimeStamp-=DaysInYear((*Year)++);
	while(DaysInMonth(*Month,*Year)<=TimeStamp)TimeStamp-=DaysInMonth((*Month)++,*Year);
	*Day=TimeStamp+1;
	return true;
}

bool TimeStampToVersion(uint8_t* Day,uint8_t* Month,uint16_t* Year,uint64_t TimeStamp){
	*Month=0;*Year=0;
	while(DaysInLTYear(*Year)<=TimeStamp)TimeStamp-=DaysInLTYear((*Year)++);
	while(DaysInLTMonth(*Month,*Year)<=TimeStamp)TimeStamp-=DaysInLTMonth((*Month)++,*Year);
	*Day=TimeStamp+1;
	return true;
}
