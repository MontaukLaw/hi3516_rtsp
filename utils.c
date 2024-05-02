#include "utils.h"
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/vfs.h>

#define APP_VERVISON "V1.0.0"

int strtime_to_inttime(const char *strtime, int *hour, int *minute)
{
    if (!strtime)
    {
        *hour = 0;
        *minute = 0;
        return -1;
    }

    sscanf(strtime, "%d:%d", hour, minute);
    return 0;
}

int get_current_time(char *psDateTime)
{

    time_t nSeconds;
    struct tm *pTM;

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    /* 系统日期和时间,格式: yyyymmddHHMMSS */
    sprintf(psDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

    return 0;
}

/*
int set_current_time(const char *psDateTime)
{
    int y,m,d,h,mm,s;
    struct tm tm;
    struct timeval tv;
    time_t timep;
    rtc_time_t  rtc_time;
    sscanf(psDateTime, "%d-%d-%d %d:%d:%d", &y, &m, &d,&h,&mm, &s);
    tm.tm_sec = s;
    tm.tm_min = mm;
    tm.tm_hour = h;
    tm.tm_mday = d;
    tm.tm_mon = m - 1;
    tm.tm_year = y - 1900;

    timep = mktime(&tm);
    tv.tv_sec = timep;
    tv.tv_usec = 0;
    if(settimeofday (&tv, (struct timezone *) 0) < 0)   {
        printf("Set system datatime error!\n");
        return -1;
    }
    rtc_time.year = y;
    rtc_time.month = m;
    rtc_time.date = d;
    rtc_time.hour = h;
    rtc_time.minute = mm;
    rtc_time.second = s;
    rtc_time.weekday =0;
#if 0
    if(set_rtc_time(&rtc_time) !=0) {
        printf("set rtc time failed\n");
        return -1;
    }
#else
    int err = system("hwclock -w");
#endif
    return err;
}
*/

int strcpy_ss2(char *strDestination, int numberOfElements, const char *strSource)
{
    char *p = strDestination;
    size_t available = numberOfElements;
    if (!strDestination || !strSource)
        return -1;
    if (strDestination == strSource)
        return 0;
    while ((*p++ = *strSource++) != 0 && --available > 0)
    {
        ;
        ;
    }
    if (available == 0)
        strDestination[numberOfElements - 1] = 0;
    else
        *p = 0;
    return 0;
}

const char *get_app_version()
{
    return APP_VERVISON;
}

const char *get_sys_version()
{
    static char buffer[16];
    FILE *fp = fopen("/version", "r");
    if (!fp)
    {
        return "V1.1.0";
    }
    fwrite(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    return buffer;
}
