/*
 * epoll_reactor.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>


#include "epoll_reactor.h"
#include "io_handler.h"

namespace framework
{

epoll_reactor::epoll_reactor():m_maxfd(0),m_epfd(-1),m_events(NULL),m_handlers(NULL)
{


}

epoll_reactor::~epoll_reactor()
{
    fini() ;
}
int epoll_reactor::init(int maxfd)
{


    if ( maxfd < MIN_FD_SIZE || m_epfd >= 0 ) return -1 ;


    m_epfd = epoll_create(maxfd) ;
    if ( m_epfd < 0 ) return -1 ;

    int memsize = sizeof(struct epoll_event) * MAX_EVENTS_ONCE + sizeof(io_handler*) * maxfd  ;
    char* membase = (char*)malloc(memsize) ;


    if (membase == NULL )
    {
        close(m_epfd) ;
        return -1 ;
    }
    memset(membase,0,memsize) ;
    m_events = (struct epoll_event*)membase ;
    m_handlers = (io_handler**)(m_events + MAX_EVENTS_ONCE) ;


    m_maxfd = maxfd ;

    return 0 ;

}
void epoll_reactor::fini()
{
    if ( m_epfd >= 0 )
    {

        close(m_epfd) ;
        m_epfd = -1 ;
        free(m_events) ;
        m_events = NULL ;
        m_handlers = NULL ;


    }

}

io_handler* epoll_reactor::get_handler(int fd)
{
    if(fd <0 || fd >= m_maxfd) return NULL ;
    return m_handlers[fd] ;
}

int epoll_reactor::add_handler(int fd, io_handler* h,int event_type)
{

    if ( h == NULL || m_epfd < 0 ) return -1 ;
    if (   fd < 0 || fd  >= m_maxfd  ) return -2 ;

    struct epoll_event ev = {0} ;
    if ( event_type & EVENT_READ )
    {
        ev.events |= EPOLLIN ;
    }

    if ( event_type & EVENT_WRITE )
    {
        ev.events |= EPOLLOUT ;
    }

    //ev.events |= EPOLLET ;
    ev.data.fd = fd ;
    if ( epoll_ctl(m_epfd , EPOLL_CTL_ADD , fd ,&ev ) != 0 ) return -3 ;


    m_handlers[fd] = h ;

    return 0 ;

}
int epoll_reactor::mod_handler(int fd, io_handler* h ,int event_type )
{

    if ( h == NULL || m_epfd < 0 ) return -1 ;
    if (   fd < 0 || fd  >= m_maxfd  ) return -2 ;

    struct epoll_event ev = {0} ;
    if ( event_type & EVENT_READ )
    {
        ev.events |= EPOLLIN ;
    }

    if ( event_type & EVENT_WRITE )
    {
        ev.events |= EPOLLOUT ;
    }


    ev.data.fd = fd ;
    if ( epoll_ctl(m_epfd , EPOLL_CTL_MOD , fd ,&ev ) != 0 ) return -3 ;
    m_handlers[fd] = h ;

    return 0 ;
}
void epoll_reactor::del_handler(int fd)
{
    if ( m_epfd < 0 ) return  ;
    if (   fd < 0 || fd  >= m_maxfd  ) return ;

    epoll_ctl(m_epfd,EPOLL_CTL_DEL,fd ,NULL) ;

    m_handlers[fd] = NULL;

}


int epoll_reactor::run_once(int msec)
{
    int n = epoll_wait(m_epfd,m_events,MAX_EVENTS_ONCE,msec) ;
    for(int i = 0 ; i < n ; ++i )
    {
        const struct epoll_event* ev = m_events + i ;
        int fd = ev->data.fd ;

        if ( ev->events & (EPOLLPRI | EPOLLERR | EPOLLHUP) )
        {
            if ( m_handlers[fd] != NULL) m_handlers[fd]->on_error(fd) ;
        }
        else
        {
            if ( ev->events & EPOLLOUT )
            {
                if ( m_handlers[fd] != NULL) m_handlers[fd]->on_write(fd) ;
            }

            if ( ev->events & EPOLLIN )
            {
                if ( m_handlers[fd] != NULL) m_handlers[fd]->on_read(fd) ;
            }
        }

    }

    return n ;

}




}
