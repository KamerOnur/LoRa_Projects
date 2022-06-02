#ifndef TIMECONVERSIONS_H

#define GETTIMEOFDAY_TO_NTP_OFFSET 2208988800UL

extern uint8_t gmtime(const uint32_t time,uint8_t show24h,char *day, char *clock, uint32_t *k_year);
extern uint16_t anolog_clock_time(const uint32_t time);
extern uint32_t Get_DST_UnixTime(uint32_t year, uint8_t mon, uint8_t dow, uint8_t day, uint8_t  hours, uint8_t  minutes);

#endif

