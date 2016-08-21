/*
 * day_roll_logger.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "day_roll_logger.h"

namespace framework
{

static const char* log_level_str[] =
{
    "" ,
    "error" ,
    "warn" ,
    "info" ,
    "trace" ,
    "debug" ,
    ""
} ;

day_roll_logger::day_roll_logger():m_fd(-1),m_filedate(0)
{
    //memset(m_prefix,0,sizeof(m_prefix)) ;

}

day_roll_logger::~day_roll_logger()
{
   fini() ;
}

int day_roll_logger::init(const char* prefix,int log_level)
{
    if ( prefix == NULL ) return -1 ;
    if ( log_level < LOG_LEVEL_MIN) log_level = LOG_LEVEL_MIN;
    if( log_level > LOG_LEVEL_MAX) log_level = LOG_LEVEL_MAX;

    m_level = log_level ;
    strncpy(m_prefix,prefix,sizeof(m_prefix)-1) ;

    tzset() ;
    return prepare() ;

}
void day_roll_logger::fini()
{

    if ( m_fd >= 0 )
    {
        close(m_fd) ;
        m_fd = -1 ;
        m_level = LOG_LEVEL_NONE ;
        m_filedate = 0 ;
        m_prefix[0] = '\0' ;

    }

}
int day_roll_logger::prepare()
{
    time_t t = time(NULL);
    if ( localtime_r(&t, &m_now) == NULL ) return -1 ;
    m_now.tm_year += 1900 ;
    m_now.tm_mon += 1 ;

    int curdate = ( m_now.tm_mon << 8 ) | m_now.tm_mday ;
    if (m_filedate != curdate )
    {
        if(strcmp(m_prefix,"/dev/null")==0)  return 0 ;
        char filename[MAX_PREFIX_SIZE*2] = {0} ;
        snprintf(filename,sizeof(filename),"%s.%04d%02d%02d.log",
            m_prefix ,m_now.tm_year,m_now.tm_mon,m_now.tm_mday) ;
        int fd = open(filename,O_APPEND|O_WRONLY|O_CREAT,0777) ;
        if ( fd < 0 ) return -1 ;
        if ( m_fd >= 0 )
        {
            close(m_fd) ;
        }
        m_fd = fd ;
        m_filedate = curdate ;
    }

    return 0 ;
}

int day_roll_logger::write_format(int ll,const char* fmt,...)
{

    if ( m_fd < 0 || ll > m_level || ll < LOG_LEVEL_MIN) return -1 ;

    if ( prepare() != 0 ) return -1 ;

    int head_size = sprintf(m_buf ,"%04d-%02d-%02d %02d:%02d:%02d,%s," ,
            m_now.tm_year,m_now.tm_mon,m_now.tm_mday,m_now.tm_hour,m_now.tm_min,m_now.tm_sec,
            log_level_str[ll] ) ;

    if(head_size < 0 ) return -2 ;

    va_list ap ;
    va_start(ap, fmt);
    int writable_size = sizeof(m_buf) - head_size -1 ;
    int content_size = vsnprintf(m_buf + head_size,writable_size,fmt,ap) ;
    va_end(ap);

    if(content_size < 1 ) return -2 ;
    else if ( content_size >= writable_size ) content_size = writable_size -1 ;

    content_size += head_size ;
    if ( m_buf[content_size -1] !='\n')
    {
        m_buf[content_size] = '\n' ;
        m_buf[++content_size] = '\0' ;
    }

    return write(m_fd,m_buf,content_size) ;

}


}
