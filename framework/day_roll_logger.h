/**
 * day_roll_logger.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <time.h>

namespace framework
{

/**
 * @brief logger file rolled by day
 */
class day_roll_logger
{
public:
    enum
    {
        LOG_LEVEL_NONE = 0 ,
        LOG_LEVEL_ERROR = 1 ,
        LOG_LEVEL_WARN = 2 ,
        LOG_LEVEL_INFO = 3 ,
        LOG_LEVEL_TRACE= 4 ,
        LOG_LEVEL_DEBUG = 5 ,
        LOG_LEVEL_MIN = LOG_LEVEL_ERROR ,
        LOG_LEVEL_MAX = LOG_LEVEL_DEBUG ,
    } ;

    enum
    {
        MAX_PREFIX_SIZE = 1024 ,
        MAX_LINE_SIZE = 2048 ,
    };

public:
    /**
     * @brief initialize the logger
     * @param [in]: prefix - logger file prefix , prefix-xxxx-xx-xx.log
     * @param [in]: log level
     * @return 0 if success
     */
    int init(const char* prefix,int log_level) ;

    /**
     * @brief clean up
     */
    void fini() ;

    /**
     * @brief write formatted log data
     * @param [in] log level
     * @param [in] format string
     * @param [in] write data size
     */
    int write_format(int log_level,const char* fmt,...) ;

    int write_string(int log_level,const char* content)
    {
        return this->write_format(log_level,"%s",content) ;
    }

    short get_level() { return m_level ; } ;
    const char* get_prefix() { return m_prefix ; } ;

public:
    day_roll_logger();
    ~day_roll_logger();

private:
    day_roll_logger(const day_roll_logger& ) ;
    day_roll_logger& operator=(const day_roll_logger&) ;

private:
    int prepare() ;

private:
    struct tm m_now ;
    int m_fd ;
    int m_filedate ;
    char m_level ;
    char m_prefix[MAX_PREFIX_SIZE] ;
    char m_buf[MAX_LINE_SIZE] ;

};

#ifdef NDEBUG
#define write_log_format(logger,level,fmt,args...) \
    do{ logger.write_format(level,fmt,##args);} while(0)
#define write_log_string(logger,level,content) \
    do{ logger.write_string(level,content);} while(0)
#else
#define write_log_format(logger,level, fmt,args...) \
    do{ logger.write_format(level,"[%s:%d]" fmt , __FILE__,__LINE__,##args);} while(0)
#define write_log_string(logger,level,content) \
    do{ logger.write_format(level,"[%s:%d]%s",__FILE__,__LINE__,content);} while(0)
#endif


#define error_log_format(logger, fmt ,args...) write_log_format(logger,framework::day_roll_logger::LOG_LEVEL_ERROR, fmt,##args)
#define warn_log_format(logger, fmt ,args...) write_log_format(logger,framework::day_roll_logger::LOG_LEVEL_WARN, fmt,##args)
#define info_log_format(logger, fmt ,args...) write_log_format(logger,framework::day_roll_logger::LOG_LEVEL_INFO, fmt,##args)
#define trace_log_format(logger, fmt ,args...) write_log_format(logger,framework::day_roll_logger::LOG_LEVEL_TRACE, fmt,##args)

#define error_log_string(logger,content) write_log_string(logger,framework::day_roll_logger::LOG_LEVEL_ERROR,content)
#define warn_log_string(logger,content) write_log_string(logger,framework::day_roll_logger::LOG_LEVEL_WARN,content)
#define info_log_string(logger,content) write_log_string(logger,framework::day_roll_logger::LOG_LEVEL_INFO,content)
#define trace_log_string(logger,content) write_log_string(logger,framework::day_roll_logger::LOG_LEVEL_TRACE,content)

#ifdef NDEBUG
#define debug_log_format(logger,fmt,args...)
#define debug_log_string(logger,content)
#else
#define debug_log_format(logger,fmt,args...) write_log_format(logger,framework::day_roll_logger::LOG_LEVEL_DEBUG,fmt,##args)
#define debug_log_string(logger,content) write_log_format(logger,framework::day_roll_logger::LOG_LEVEL_DEBUG,content)
#endif

}

