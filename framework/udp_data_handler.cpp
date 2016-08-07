
/*
 *
 * Author : lixingyi (lxyfirst@163.com)
 */
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "udp_data_handler.h"

namespace framework
{

udp_data_handler::udp_data_handler():m_reactor(NULL),m_fd(-1),m_options(0)
{
}

udp_data_handler::~udp_data_handler()
{
    fini() ;
}

int udp_data_handler::init(base_reactor* reactor,const char* host,int port)
{
    if(m_fd >= 0 || reactor == NULL || host == NULL ) return -1 ;
    sa_in_t service_addr ;
    init_sa_in(&service_addr,host,port) ;
    int sfd = create_udp_service(&service_addr) ;
    if(sfd <0 ) return -2 ;

    set_socket_option(sfd,SO_RCVBUF,MAX_BUF_SIZE * 1024) ;
    set_socket_option(sfd,SO_SNDBUF,MAX_BUF_SIZE * 1024) ;

    if( reactor->add_handler(sfd, this, base_reactor::EVENT_READ)!=0 )
    {
        close(sfd) ;
        return -3 ;
    }

    m_reactor = reactor ;
    m_fd = sfd ;

    return 0 ;

}

void udp_data_handler::fini()
{
    if(m_fd >= 0 )
    {
        m_reactor->del_handler(m_fd) ;
        close(m_fd) ;
        m_fd = -1 ;

    }
}

void udp_data_handler::handle_error(int error_type)
{
    on_event(error_type) ;
}

void udp_data_handler::on_read(int fd)
{
    udp_packet* p = (udp_packet*)m_buf ;
    static const int UDP_RECV_SIZE = MAX_BUF_SIZE-sizeof(udp_packet) -1 ;
    do
    {
        socklen_t  addrlen = sizeof(p->addr);
        p->data_size = recvfrom(fd,p->data,UDP_RECV_SIZE,0,(sa_t*)&p->addr,&addrlen) ;
        if(p->data_size <= 0 )
        {
            if (errno != EAGAIN &&  errno != EINTR)
            {
                handle_error(ERROR_TYPE_SYSTEM)  ;
                return ;
            }

            break ;
        }
        else
        {
            p->data[p->data_size] = 0 ;
            process_packet(p) ;
        }

    }while(m_options & OPTION_READALL) ;

}

void udp_data_handler::on_write(int fd)
{
}

void udp_data_handler::on_error(int fd)
{
    handle_error(ERROR_TYPE_SYSTEM) ;
}

void udp_data_handler::on_event(int type)
{
    fini() ;
}

int udp_data_handler::send(const sa_in_t* to_addr,const char* data,int size) 
{
    socklen_t addrlen = sizeof(sa_in_t) ;
    if( sendto(m_fd,data,size,0,(const sa_t*)to_addr,addrlen)==size) return 0;
    return -1 ;
}

int udp_data_handler::send(const char* host,int port,const char* data,int size) 
{
    sa_in_t service_addr ;
    init_sa_in(&service_addr,host,port) ;
    return send(&service_addr,data,size);
}

int udp_data_handler::send(const sa_in_t* to_addr, packet *p)
{
    //static const int UDP_BUF_SIZE = MAX_BUF_SIZE-sizeof(udp_packet) -1 ;
    int size = p->encode_size() ;
    if(size < 1 || size >sizeof(m_send_buf) ) return -1 ;

    size = p->encode(m_send_buf,sizeof(m_send_buf)) ;
    if ( size < 1 ) return -1 ;

    return send(to_addr,m_send_buf,size);

}

void udp_data_handler::set_option(int options,bool flag)
{
    if(flag)
    {
        m_options |= options ;
    }
    else
    {
        m_options &= ~options ;
    }

}


}

