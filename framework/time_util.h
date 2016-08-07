/**
 * time_util.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>

enum 
{ 
    SECONDS_QUARTER_HOUR = 900, 
    SECONDS_HALF_HOUR = 1800, 
    SECONDS_OF_HOUR = 3600, 
    SECONDS_OF_DAY = 86400 ,
    SECONDS_OF_WEEK = SECONDS_OF_DAY *7 
};

extern long timezone;  

namespace framework
{


//CPU's timestamp counter.
inline uint64_t rdtsc()
{
    uint32_t low, high;
    __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
    return (uint64_t) high << 32 | low;
}

// millisecond , clock_gettime need librt
inline int64_t time_ms()
{
    #if 0

    struct timeval tv = {0} ;
    gettimeofday(&tv,NULL) ;
    return tv.tv_sec * 1000 + tv.tv_usec/1000 ;

    #else

    struct timespec tv = {0} ;
    clock_gettime(CLOCK_MONOTONIC,&tv) ;
    return tv.tv_sec * 1000 + tv.tv_nsec/1000000 ;

    #endif
}



// microsecond 
inline int64_t time_us()
{
    #if 0

    struct timeval tv = {0} ;
    gettimeofday(&tv,NULL) ;
    return tv.tv_sec * 1000000 + tv.tv_usec ;
    
    #else

    struct timespec tv = {0} ;
    clock_gettime(CLOCK_MONOTONIC,&tv) ;
    return tv.tv_sec * 1000000 + tv.tv_nsec/1000 ;

    #endif
}

// get tick counter per microsecond , may block 100 milliseconds
inline int64_t get_tsc_us()
{
    int64_t begin_tsc = rdtsc() ;
    int64_t begin_us = time_us() ;
    int64_t tsc_interval = 0 ;
    int64_t us_interval = 0 ;
    do
    {
        usleep(100000) ;
        tsc_interval = rdtsc() - begin_tsc  ;
        us_interval = time_us() - begin_us ;

    } while(us_interval < 1 ); //cliff : || us_interval  (100000 << 3) ) ;

    return tsc_interval/us_interval ;
}

inline time_t str2time(const char* buf,const char* fmt)
{
    struct tm tmp_tm = {0} ;

    if ( strptime(buf,fmt,&tmp_tm) == NULL ) return 0 ;
    return mktime(&tmp_tm) ;
}

inline int time2str(char* buf,int maxlen ,const char* fmt,time_t t)
{
    struct tm tmp_tm;
    localtime_r(&t,&tmp_tm) ;
    return strftime(buf,maxlen,fmt,&tmp_tm) ;
}

inline int time2str_utc(char* buf,int maxlen ,const char* fmt,time_t t)
{
    struct tm tmp_tm;
    gmtime_r(&t,&tmp_tm) ;
    return strftime(buf,maxlen,fmt,&tmp_tm) ;
}

inline time_t day_begin_time(time_t t)
{
    struct tm tm_time = {0} ;
    localtime_r(&t,&tm_time) ;
    tm_time.tm_hour = 0;
    tm_time.tm_min = 0;
    tm_time.tm_sec = 0;
    return mktime(&tm_time) ;
}

inline time_t cycle_begin_time(time_t t,int cycle_seconds = SECONDS_OF_DAY , bool utc = true)
{
    if(utc && timezone == 0 ) tzset() ;
    return ( (t - timezone)/cycle_seconds ) * cycle_seconds + timezone ;

}

inline bool is_same_cycle(time_t first_t, time_t second_t, int cycle_seconds = SECONDS_OF_DAY)
{
    if (timezone == 0) tzset();
    if (cycle_seconds < 1) return true ;
    return (first_t - timezone) / cycle_seconds == (second_t - timezone )/ cycle_seconds ;
}

}


