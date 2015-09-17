/*
 * tcp_connection.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
//#include <stdio.h>

#include <time.h>

#include "base_reactor.h"
#include "packet_processor.h"
#include "tcp_connection.h"


namespace framework
{

tcp_connection::tcp_connection():m_reactor(NULL),m_processor(NULL),
    m_max_write_size(MAX_WRITE_SIZE),
    m_connect_status(STATUS_CLOSED),
    m_options(0)
{
    // TODO Auto-generated constructor stub

    m_id.fd = -1 ;
    m_id.timestamp = 0 ;
}

tcp_connection::~tcp_connection()
{
    inner_fini(true) ;
}

int tcp_connection::init(base_reactor* reactor,const char* host,int port,packet_processor* processor)
{
    if(m_id.fd >= 0 || host == NULL || processor == NULL ) return -1 ;

    if( m_rbuf.resize(INIT_BUF_SIZE) != 0 ) return -2;
    if( m_sbuf.resize(INIT_BUF_SIZE) != 0 ) return -2;

    sa_in_t service_addr ;
    init_sa_in(&service_addr,host,port) ;

    int fd = create_tcp_client(&service_addr,0) ;
    if(fd < 0 ) return -3 ;

    if( reactor->add_handler(fd,this,base_reactor::EVENT_WRITE)!=0 )
    {
        close(fd) ;
        return -3 ;
    }

    m_reactor = reactor ;
    m_processor = processor ;

    m_id.fd = fd ;
    m_id.timestamp = time(0) ;

    update_status(STATUS_CONNECTING);


    return 0 ;
}

int tcp_connection::init(base_reactor* reactor,int fd,packet_processor* processor)
{
    if(m_id.fd >= 0 || fd < 0 || reactor == NULL ||processor ==NULL) return -1 ;
    set_nonblock(fd) ;
    //set_socket_nodelay(fd) ;
    if( m_rbuf.resize(INIT_BUF_SIZE) != 0 ) return -2;
    if( m_sbuf.resize(INIT_BUF_SIZE) != 0 ) return -2;

    if( reactor->add_handler(fd,this,base_reactor::EVENT_READ)!=0 )
    {
        return -3 ;
    }

    m_reactor = reactor ;
    m_processor = processor ;

    m_id.fd = fd ;
    m_id.timestamp = time(0) ;

    update_status(STATUS_CONNECTED);

    return 0 ;
}

int tcp_connection::attach_reactor(base_reactor* reactor)
{
    if( (m_reactor!= NULL) || (reactor==NULL) ) return -1 ;
    
    if(reactor->add_handler(m_id.fd,this,base_reactor::EVENT_READ|base_reactor::EVENT_READ)!=0)
    {
        return -2 ;
    }
    
    m_reactor = reactor ;
    
    return 0 ;
}

void tcp_connection::detach_reactor()
{
    if(m_reactor) 
    {
        m_reactor->del_handler(m_id.fd) ;
        m_reactor = NULL ;
    }
    
}

void tcp_connection::inner_fini(bool release)
{
    if(m_id.fd >= 0 )
    {
        if(m_reactor) 
        {
            m_reactor->del_handler(m_id.fd) ;
            m_reactor = NULL ;
        }
        
        close(m_id.fd) ;
        m_id.fd = -1 ;
        m_id.timestamp = 0 ;
        
        m_rbuf.clear() ;
        m_sbuf.clear() ;
    }

    if(release)
    {
        m_rbuf.fini() ;
        m_sbuf.fini() ;
    }

}

void tcp_connection::fini(bool release)
{
    inner_fini(release) ;
    update_status(STATUS_CLOSED) ;

}


void tcp_connection::on_read(int fd)
{

    do
    {
        int recv_len = recv(fd,m_rbuf.space(),m_rbuf.space_size(),0) ;
        if(recv_len >0)
        {
            m_rbuf.push_data(recv_len) ;
            while(m_rbuf.data_size() > 0)
            {
                packet_info pi = {0} ;
                int ret = 0 ;
                if((ret=m_processor->get_packet_info(this,m_rbuf.data(),m_rbuf.data_size(),&pi))!=0)
                {
                    handle_error(ret);
                    return ;
                }
                     
                if( pi.size < 1 || pi.size > 4194304 )
                {
                    handle_error(ERROR_TYPE_REQUEST) ;
                    return ;
                }

                if(m_rbuf.data_size() >= pi.size )
                {
                    if((ret=m_processor->process_packet(this,&pi)) !=0 )
                    {
                        handle_error(ret) ;
                        return ;
                    }

                    m_rbuf.pop_data(pi.size) ;
                }
                else
                {
                    if(m_rbuf.space_size() < pi.size - m_rbuf.data_size())
                    {
                        if(m_rbuf.resize(m_rbuf.capacity() + pi.size )!=0)
                        {
                            handle_error(ERROR_TYPE_MEMORY) ;
                            return ;
                        }
                    }

                    break ;
                }

            }
            m_rbuf.adjust() ;

        }
        else if(recv_len == 0)
        {
            //peer close
            handle_error(ERROR_TYPE_PEER_CLOSE) ;
            return ;
        }
        else 
        {
            if (errno != EAGAIN &&  errno != EINTR)
            {
                handle_error(ERROR_TYPE_SYSTEM)  ;
                return ;
            }

            break ;
        }

    }while(m_options & OPTION_READALL) ;


}

void tcp_connection::on_write(int fd)
{
    if(m_connect_status == STATUS_CONNECTING) 
    {
        if(m_reactor) m_reactor->mod_handler(fd,this,base_reactor::EVENT_READ) ;
        update_status(STATUS_CONNECTED) ;
        return ;
    }

    if(m_sbuf.data_size() > 0 )
    {
        int to_send = m_sbuf.data_size() > m_max_write_size ? m_max_write_size : m_sbuf.data_size();
        int send_size = ::send(fd,m_sbuf.data(),to_send,0) ;
        if(send_size >0)
        {
            m_sbuf.pop_data(send_size) ;
            m_sbuf.adjust() ;

        }
        else if ( send_size < 0)
        {
            
            if (errno != EAGAIN &&  errno != EINTR)
            {
                handle_error(ERROR_TYPE_SYSTEM) ;
                return ;
            }
            
        }
    }

    if(m_sbuf.data_size() == 0)
    {
        if(m_reactor) m_reactor->mod_handler(fd,this,base_reactor::EVENT_READ) ;

    }

}

void tcp_connection::on_error(int fd)
{

    handle_error(ERROR_TYPE_SYSTEM) ;

}

int tcp_connection::send(const char* data,int size,int delay_flag)
{
    if(m_id.fd < 0 || m_connect_status != STATUS_CONNECTED ) return -1 ;
    if(m_sbuf.space_size() < size &&(m_sbuf.resize(m_sbuf.capacity() + size )!=0) )
    {
        return -1 ;
    }

    int send_size = 0 ;
    //try send data directly
    if(m_sbuf.data_size()==0 && delay_flag ==0)
    {
        send_size = ::send(m_id.fd,data,size,0) ;
        if(send_size <0)
        {
            if (errno != EAGAIN &&  errno != EINTR)
            {
                return -1 ;
            }
            send_size = 0 ;
        }
        if(send_size == size ) return 0 ;
    }

    //push remaining data to send buffer
    size -= send_size ;

    memcpy(m_sbuf.space(),data + send_size , size) ;
    m_sbuf.push_data(size) ;
    if(m_reactor)
    {
        m_reactor->mod_handler(m_id.fd,this,base_reactor::EVENT_READ | base_reactor::EVENT_WRITE) ;
    }
    

    return 0 ;
}

int tcp_connection::send( packet *p,int delay_flag)
{
    if(m_id.fd < 0 || m_connect_status != STATUS_CONNECTED ) return -1 ;
    int size = p->encode_size() ;
    if(size < 1 ) return -1 ;
    if(m_sbuf.space_size() < size && m_sbuf.resize(m_sbuf.capacity() + size )!=0)
    {
        return -1 ;
    }

    size = p->encode(m_sbuf.space(),m_sbuf.space_size()) ;
    if ( size < 1 ) return -1 ;
    m_sbuf.push_data(size) ;
    if(m_reactor)
    {
        m_reactor->mod_handler(m_id.fd,this,base_reactor::EVENT_READ | base_reactor::EVENT_WRITE) ;
    }
    
    if(delay_flag == 0) on_write(m_id.fd) ;

    return 0 ;
}

void tcp_connection::handle_error(int error_type)
{
    //on_event(error_type) ;
    int error_no = get_errno() ;
    inner_fini(false) ;
    update_status(STATUS_CLOSED,error_type,error_no);

}

void tcp_connection::update_status(int status,int error_type,int error_no)
{
    int old_status = m_connect_status ;
    m_connect_status = status ;
    if(old_status != STATUS_CONNECTED && status == STATUS_CONNECTED )
    {
        m_processor->on_connected(this) ;
        return ;
    }
    else if( old_status != STATUS_CLOSED && status == STATUS_CLOSED )
    {
        m_processor->on_closed(this,error_type,error_no) ;
        return ;
    }
    

}


int tcp_connection::get_sock_addr(sa_in_t* addr) const
{
    if(m_id.fd < 0 ) return -1 ;
    socklen_t addr_size = sizeof(*addr) ;
    return getsockname(m_id.fd,(struct sockaddr*)addr,&addr_size) ;

}

int tcp_connection::get_remote_addr(sa_in_t* addr) const
{
    if(m_id.fd < 0 ) return -1 ;
    socklen_t addr_size = sizeof(*addr) ;
    return getpeername(m_id.fd,(struct sockaddr*)addr,&addr_size) ;
}

void tcp_connection::set_option(int options,bool flag)
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
