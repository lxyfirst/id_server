/*
 * log_thread.cpp
 * Author: lixingyi (lxyfirst@163.com)
 */

#include <unistd.h>
#include <functional>

#include "system_util.h"
#include "network_util.h"
#include "packet.h"

#include "log_thread.h"

namespace framework 
{


log_thread::log_thread():m_notify_fd(0),m_now(0)
{
    // TODO Auto-generated constructor stub

}

log_thread::~log_thread()
{
    // TODO Auto-generated destructor stub
}

int log_thread::on_init()
{
    if(m_reactor.init(2)!=0) error_return(-1,"init reactor failed") ;

    int pipe_fd[2] = {0} ;
    if(socketpair(AF_UNIX,SOCK_SEQPACKET,0,pipe_fd)!=0)
    {
        error_return(-1,"create pipe failed") ;
    }

    pipe_handler::callback_type callback = std::bind(&log_thread::on_pipe_message,this,std::placeholders::_1) ;
    if(m_handler.init(m_reactor,pipe_fd[1],callback )!=0 )
    {
        error_return(-1,"init pipe failed") ;
    }

    m_notify_fd = pipe_fd[0] ;


    return 0;
}

int log_thread::init(const char* prefix,int log_level)
{
    if(m_notify_fd < 1)
    {
        return m_logger.init(prefix,log_level) ;
    }
    else
    {
        int name_size = strlen(prefix) ;
        char* buf = new char[name_size +1] ;
        if(buf == NULL) return -1 ;
        memcpy(buf,prefix,name_size) ;
        buf[name_size] = '\0' ;

        packet_info msg = {0} ;
        msg.type = (MESSAGE_TYPE_CONFIG << 8) | log_level ;
        msg.size = time(0) ;
        msg.data = buf ;

        if( write(m_notify_fd,&msg,sizeof(msg)) != sizeof(msg) )
        {
            delete[] buf ;
            return -2 ;
        }
    }

    return 0 ;
}

void log_thread::on_fini()
{
    close(m_notify_fd) ;
    m_notify_fd = 0 ;

    run_once();

    m_handler.fini() ;

    m_reactor.fini() ;
}

void log_thread::on_timeout()
{
    //m_logger.prepare() ;
}

void log_thread::run_once()
{

    int now = time(0) ;
    if (now - m_now >= 60)
    {
        m_now = now;
        on_timeout();
    }

    m_reactor.run_once(2000) ;

}

void log_thread::on_pipe_message(const packet_info* msg)
{
    int msg_type = msg->type >> 8 ;
    int log_level = msg->type & 0xFF ;
    switch(msg_type)
    {
    case MESSAGE_TYPE_LOG:
        //todo: log time stored in msg->size
        m_logger.write_string(log_level,msg->data) ;
        delete[] msg->data ;
        break ;

    case MESSAGE_TYPE_CONFIG:
        m_logger.fini() ;
        m_logger.init(msg->data,log_level) ;
        delete[] msg->data ;
        break ;

    }

}

int log_thread::write_format(int ll,const char* fmt,...)
{
    if( !is_run() ) return -1 ;

    if ( m_notify_fd < 1 || ll < 0 || ll > m_logger.get_level() ) return -1 ;


    char* buf = new char[day_roll_logger::MAX_LINE_SIZE] ;
    if(buf == NULL) return -1 ;

    const int WRITABLE_SIZE = day_roll_logger::MAX_LINE_SIZE-1 ;
    va_list ap ;
    va_start(ap, fmt);
    int length = vsnprintf(buf,WRITABLE_SIZE,fmt,ap) ;
    va_end(ap);

    if(length < 0 ) return -2 ;
    else if ( length >= WRITABLE_SIZE ) length = WRITABLE_SIZE -1 ;

    if( buf[length-1] !='\n')
    {
        buf[length]= '\n' ;
        buf[++length]= '\0' ;
    }

    packet_info msg = {0} ;
    msg.type = (MESSAGE_TYPE_LOG <<8) | ll ;
    msg.size = time(0) ;
    msg.data = buf ;

    if( write(m_notify_fd,&msg,sizeof(msg)) != sizeof(msg) )
    {
        delete[] buf ;
        return -2 ;
    }

    return 0 ;
}


}

