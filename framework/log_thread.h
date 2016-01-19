/**
 * log_thread.h
 * Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include "poll_reactor.h"
#include "thread.h"
#include "pipe_handler.h"
#include "day_roll_logger.h"

namespace framework
{

/**
 * @brief logger thread
 */
class log_thread: public simple_thread
{
public:
    enum
    {
        MESSAGE_TYPE_LOG = 1 ,
        MESSAGE_TYPE_CONFIG =2 ,
    };
public:
    log_thread();
    virtual ~log_thread();

    int write_format(int level,const char* fmt,...) ;

    int init(const char* prefix,int log_level) ;

protected:
    virtual int on_init() ;
    virtual void on_fini() ;
    virtual void run_once() ;

    void on_timeout() ;

    void on_pipe_message(const packet_info* msg);

private:
    poll_reactor m_reactor ;
    day_roll_logger m_logger ;
    pipe_handler m_handler ;
    int m_notify_fd ;
    int m_now ;

};

}

