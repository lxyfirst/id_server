/*
 * 
 * Author : lixingyi (lxyfirst@163.com)
 */

#include <sys/eventfd.h>
#include <unistd.h>

#include "base_reactor.h"
#include "eventfd_handler.h"



namespace framework 
{


eventfd_handler::eventfd_handler():m_reactor(NULL),m_event_fd(-1)
{

}

eventfd_handler::~eventfd_handler() 
{ 
    fini();
} 

int eventfd_handler::init(base_reactor& reactor,const callback_type& callback)
{
    m_event_fd = eventfd(0,EFD_NONBLOCK) ;
    if(m_event_fd < 0) return -2 ;
    reactor.add_handler(m_event_fd,this,base_reactor::EVENT_READ) ;
    m_reactor = &reactor ;
    m_callback = callback ;
    return 0 ;

}

void eventfd_handler::fini()
{
    if(m_reactor) m_reactor->del_handler(m_event_fd) ;

    if(m_event_fd >0)
    {
        close(m_event_fd) ;
        m_event_fd = -1 ;
    }
}


int eventfd_handler::notify(int64_t v)
{
    return write(m_event_fd,(char*)&v,sizeof(v)) ;
}




void eventfd_handler::on_read(int fd)
{
    uint64_t v ;
    read(m_event_fd,&v,sizeof(v)) ;
    m_callback(v) ;
}

void eventfd_handler::on_write(int fd)
{
}

void eventfd_handler::on_error(int fd)
{

}


}


