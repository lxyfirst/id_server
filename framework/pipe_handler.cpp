/*
 * pipe_handler.cpp
 * Author: lixingyi (lxyfirst@163.com)
 */

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "network_util.h"
#include "packet.h"

#include "base_reactor.h"
#include "pipe_handler.h"

namespace framework
{

pipe_handler::pipe_handler():m_pipe_fd(0)
{
    // TODO Auto-generated constructor stub

}

pipe_handler::~pipe_handler()
{
    fini() ;
}

int pipe_handler::init(base_reactor& reactor,int pipe_fd,const callback_type& callback)
{
    if ( m_pipe_fd > 0) return -1 ;
    int sock_type = get_socket_option(pipe_fd,SO_TYPE);
    if(sock_type != SOCK_DGRAM && sock_type !=SOCK_SEQPACKET) return -1 ;
       
    set_nonblock(pipe_fd) ;

    if(reactor.add_handler(pipe_fd,this,base_reactor::EVENT_READ)!=0)
    {
        return -2 ;
    }

    m_callback = callback ;
    m_reactor = &reactor ;
    m_pipe_fd = pipe_fd ;

    return 0 ;
}

void pipe_handler::fini()
{
    if(m_pipe_fd >0)
    {
        m_reactor->del_handler(m_pipe_fd) ;
        close(m_pipe_fd) ;
        m_pipe_fd = -1 ;
    }

}

void pipe_handler::on_read(int fd)
{
    for(;;)
    {
        packet_info msg  ;
        int ret = read(fd,&msg,sizeof(msg)) ;
        if(ret == sizeof(msg) )
        {
            m_callback(&msg) ;
        }
        else 
        {
            if (errno != EAGAIN &&  errno != EINTR)
            {
                on_error(fd)  ;
                return ;
            }

            break ;
        }
    } 


}

void pipe_handler::on_write(int fd)
{

}

void pipe_handler::on_error(int fd)
{
    close(fd) ;
}


int pipe_handler::send_pipe_message(const packet_info* msg)
{
    if( write(m_pipe_fd,(void*)msg,sizeof(*msg)) == sizeof(*msg) ) return 0 ;
    return -1 ;
}


}

