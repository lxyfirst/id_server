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

day_roll_logger::day_roll_logger():m_fd(-1),m_filedate(0),m_buf(NULL)
{
    memset(m_prefix,0,sizeof(m_prefix)) ;

}

day_roll_logger::~day_roll_logger()
{
   fini() ;
}

int day_roll_logger::init(const char* prefix,int log_level,int buf_size)
{
    if ( prefix == NULL ) return -1 ;
    if ( log_level < LOG_LEVEL_MIN) log_level = LOG_LEVEL_MIN;
    if( log_level > LOG_LEVEL_MAX) log_level = LOG_LEVEL_MAX;

    m_level = log_level ;
    strncpy(m_prefix,prefix,sizeof(m_prefix)-1) ;

    m_delay = 1 ;
    if(buf_size < MIN_IOBUF_SIZE )
    {
        m_delay = 0 ;
        buf_size = MIN_IOBUF_SIZE ;
    }
    else if ( buf_size > MAX_IOBUF_SIZE )
    {
        buf_size = MAX_IOBUF_SIZE ;
    }

    m_buf = (memory_buffer*)realloc(m_buf,buf_size) ;
    if ( m_buf == NULL ) return -2 ;
    m_buf->size = buf_size - sizeof(memory_buffer) ;
    m_buf->pos = 0 ;

    //tzset() ;
    return prepare() ;

}
void day_roll_logger::fini()
{


    if ( m_fd >= 0 )
    {
        flush() ;
        close(m_fd) ;
        m_fd = -1 ;
        m_level = LOG_LEVEL_NONE ;
        m_filedate = 0 ;
        m_prefix[0] = '\0' ;

    }

    if(m_buf != NULL)
    {
        free(m_buf) ;
        m_buf = NULL ;
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
            flush() ;
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

    char *buf_data = m_buf->data + m_buf->pos ;
    int length = sprintf(buf_data ,"%04d-%02d-%02d %02d:%02d:%02d,%s," ,
            m_now.tm_year,m_now.tm_mon,m_now.tm_mday,m_now.tm_hour,m_now.tm_min,m_now.tm_sec,
            log_level_str[ll] ) ;

    if(length < 0 ) return -2 ;
    buf_data += length ;

    static int MAX_LINE_CONTENT = MAX_LINE_SIZE - 64;
    va_list ap ;
    va_start(ap, fmt);
    length = vsnprintf(buf_data,MAX_LINE_CONTENT,fmt,ap) ;
    va_end(ap);

    if(length < 0 ) return -2 ;
    else if ( length >MAX_LINE_CONTENT) length =MAX_LINE_CONTENT ;
    buf_data += length ;

    if ( *(buf_data -1) !='\n')
    {
        *buf_data = '\n' ;
        *(++buf_data) = '\0' ;
    }
    
    

    m_buf->pos = buf_data - m_buf->data ;

    if( (m_buf->size - m_buf->pos < MAX_LINE_SIZE) && (flush() !=0) ) return -3 ;

    return 0 ;

}


int day_roll_logger::write_string(int ll,const char* content)
{
    if ( m_fd < 0 || ll > m_level || ll < LOG_LEVEL_MIN) return -1 ;

    if ( prepare() != 0 ) return -1 ;
    
    static int MAX_LINE_CONTENT = MAX_LINE_SIZE - 64;
    char *buf_data = m_buf->data + m_buf->pos ;
    int length = snprintf(buf_data,MAX_LINE_SIZE,"%04d-%02d-%02d %02d:%02d:%02d,%s,%s" ,
            m_now.tm_year,m_now.tm_mon,m_now.tm_mday,m_now.tm_hour,m_now.tm_min,m_now.tm_sec,
            log_level_str[ll],content) ;

    if(length < 0 ) return -2 ;
    else if ( length >MAX_LINE_CONTENT) length =MAX_LINE_CONTENT ;
    buf_data += length ;

    if ( *(buf_data -1) !='\n')
    {
        *buf_data = '\n' ;
        *(++buf_data) = '\0' ;
    }

    m_buf->pos = buf_data - m_buf->data ;

    if( (m_buf->size - m_buf->pos < MAX_LINE_SIZE) && (flush() !=0) ) return -3 ;

    return 0 ;
}

/*
int day_roll_logger::write_bin(log_level_type ll,const char* data,int len)
{
    if ( m_fd <= 0 || ll > m_level || ll < LOG_LEVEL_MIN ) return -1 ;

    if ( prepare() != 0 ) return -1 ;

    char buf[1024] ;
    int length = sprintf(buf, "%02d:%02d:%02d,%s," , m_now.tm_hour,m_now.tm_min,m_now.tm_sec,log_level_str[ll]) ;
    for(int i = 0 ; i < len ; ++i)
    {
        length += snprintf(buf+length,sizeof(buf)-length,"%02x ",(unsigned char)data[i]) ;
    }
    buf[length] = '\n' ;
    buf[++length] = '\0' ;
    if ( write(m_fd,buf,length) != length ) return -1;
    return 0 ;
}
*/

int day_roll_logger::flush()
{
    if(m_fd < 0 || m_buf->pos < 1 ) return -1 ;

    int len = write(m_fd,m_buf->data,m_buf->pos) ;
    if(len == m_buf->pos)
    {
        m_buf->pos = 0 ;
    }
    else if(len < m_buf->pos)
    {
        memmove(m_buf->data,m_buf->data + len , m_buf->pos - len) ;
        m_buf->pos -= len ;
    }
    else if ( len < 0 )
    {
        return -1 ;
    }

    return 0 ;

}

}
