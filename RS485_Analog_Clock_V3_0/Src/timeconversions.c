/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher
 * Copyright: GPL V2
 * See http://www.gnu.org/licenses/gpl.html
 *
 * functions to convert ntp timestamps into time and date
 *********************************************/
#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h> 
#include <avr/pgmspace.h>

// EPOCH = Jan 1 1970 00:00:00
#define	EPOCH_YR	1970
//(24L * 60L * 60L)
#define	SECS_DAY	86400UL  
#define	LEAPYEAR(year)	(!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define	YEARSIZE(year)	(LEAPYEAR(year) ? 366 : 365)

//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
static const char day_abbrev[] PROGMEM = "SunMonTueWedThuFriSat";

// isleapyear = 0-1
// month=0-11
// return: how many days a month has
//
// We could do this if ram was no issue:
//uint8_t monthlen(uint8_t isleapyear,uint8_t month){
//const uint8_t mlen[2][12] = {
//		{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
//		{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
//	};
//	return(mlen[isleapyear][month]);
//}
//
uint8_t monthlen(uint8_t isleapyear,uint8_t month)
{
	if(month==1)
	{
		return(28 + isleapyear);
	}
	if (month>6)
	{
		month--;
	}
	if (month%2==1)
	{
		return(30);
	}
	return(31);
}
//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
// gmtime -- convert calendar time (sec since 1970) into broken down time
// returns something like Fri 2007-10-19 in day and 01:02:21 in clock
// The return values is the minutes as integer. This way you can update
// the entire display when the minutes have changed and otherwise just
// write current time (clock). That way an LCD display needs complete
// re-write only every minute.
uint8_t gmtime(const uint32_t time,uint8_t show24h,char *day, char *clock, uint32_t *k_year)
{
    char dstr[4]; // week days
    char ampm[3]="pm"; 
    
    uint8_t i;
    uint32_t dayclock;
    uint16_t dayno;
    uint16_t tm_year = EPOCH_YR;
    uint8_t tm_sec,tm_min,tm_hour,tm_wday,tm_mon;

    dayclock = time % SECS_DAY;
    dayno = time / SECS_DAY;

    tm_sec = dayclock % 60UL;
    tm_min = (dayclock % 3600UL) / 60;
    tm_hour = dayclock / 3600UL;
    tm_wday = (dayno + 4) % 7;	/* day 0 was a thursday */
    
    while (dayno >= YEARSIZE(tm_year))
    {
        dayno -= YEARSIZE(tm_year);
        tm_year++;
    }
    tm_mon = 0;
    while (dayno >= monthlen(LEAPYEAR(tm_year),tm_mon))
    {
        dayno -= monthlen(LEAPYEAR(tm_year),tm_mon);
        tm_mon++;
    }
    i=0;
    while (i<3)
    {
        dstr[i]= pgm_read_byte(&(day_abbrev[tm_wday*3 + i])); 
        i++;
    }
    dstr[3]='\0';
    *k_year = tm_year;
    sprintf_P(day,PSTR("%s %u-%02u-%02u"),dstr,tm_year,tm_mon+1,dayno + 1);
    if (show24h)
    {
        sprintf_P(clock,PSTR("%02u:%02u:%02u  "),tm_hour,tm_min,tm_sec);
    }
    else
    {
        if (tm_hour<12)
        {
            ampm[0]='a';;
            ampm[1]='m';
        }
        else
        {
            tm_hour-=12;
        }
        if (tm_hour==0) tm_hour=12;
        sprintf_P(clock,PSTR("%02u:%02u:%02u%s"),tm_hour,tm_min,tm_sec,ampm);
    }
    return(tm_min);
}
//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
uint32_t convertDateToUnixTime(uint32_t year, uint8_t  month, uint8_t  day, uint8_t  hours, uint8_t  minutes, uint8_t  seconds)
{
    uint32_t y;
    uint16_t m;
    uint16_t d;
    uint32_t t;

    //Year
    y = year;
    //Month of year
    m = month;
    //Day of month
    d = day;

    //January and February are counted as months 13 and 14 of the previous year
    if(m <= 2)
    {
        m += 12;
        y -= 1;
    }

    //Convert years to days
    t = (365 * y) + (y / 4) - (y / 100) + (y / 400);
    //Convert months to days
    t += (30 * m) + (3 * (m + 1) / 5) + d;
    //Unix time starts on January 1st, 1970
    t -= 719561;
    //Convert days to seconds
    t *= 86400;
    //Add hours, minutes and seconds
    t += (3600 * hours) + (60 * minutes) + seconds;

    //Return Unix time
    return t;
}

//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
uint8_t computeDayOfWeek(uint16_t y, uint8_t m, uint8_t d)
{
    uint16_t h,j,k;

    //January and February are counted as months 13 and 14 of the previous year
    if(m <= 2)
    {
        m += 12;
        y -= 1;
    }

    //J is the century
    j = y / 100;
    //K the year of the century
    k = y % 100;

    //Compute H using Zeller's congruence
    h = d + (26 * (m + 1) / 10) + k + (k / 4) + (5 * j) + (j / 4);

    //Return the day of the week
    return ((h + 5) % 7) + 1;
}
//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
uint8_t DST_calculator(uint32_t year, uint8_t mon, uint8_t dow, uint8_t day)
{
    int ret;
    int ayin_ilk_gunu;

    ayin_ilk_gunu = computeDayOfWeek(year,mon,1);

    if(ayin_ilk_gunu <= day)
    {
        ret = (day-ayin_ilk_gunu)+1;
    }
    else
    {
        ret = (7+(day-ayin_ilk_gunu))+1;
    }
    return ((dow-1)*7) + ret;
}
//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
uint32_t Get_DST_UnixTime(uint32_t year, uint8_t mon, uint8_t dow, uint8_t day, uint8_t  hours, uint8_t  minutes)
{
    return convertDateToUnixTime(year,mon,DST_calculator(year,mon,dow,day),hours,minutes,0);
}
//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
//*******************************************************************************************************************************************************************************************
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
uint16_t anolog_clock_time(const uint32_t time)
{
    uint32_t dayclock;
    uint16_t time_konum;
    uint8_t  tm_min;
    uint8_t  tm_hour;

    dayclock = time % SECS_DAY;
    tm_min = (dayclock % 3600UL) / 60;
    tm_hour = (dayclock / 3600UL);
    time_konum = (tm_hour*60) + tm_min;
    
    if(time_konum>=720)
    {
        time_konum -= 720;
    }
    return time_konum;
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


