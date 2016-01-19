/*
 * poll_reactor.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>


#include "poll_reactor.h"
#include "io_handler.h"

namespace framework
{

poll_reactor::poll_reactor():m_events(NULL),m_handlers(NULL),m_max_count(0),m_cur_count(0),m_prepare_flag(0)
{


}

poll_reactor::~poll_reactor()
{
    fini() ;
}

int poll_reactor::init(int max_count)
{


    if ( max_count < 1 || max_count > MAX_FD_COUNT || m_events ) return -1 ;


    int memsize = (sizeof(struct pollfd)  + sizeof(poll_data) ) * max_count ;
    char* membase = (char*)malloc(memsize) ;


    if (membase == NULL )
    {
        return -1 ;
    }
    memset(membase,0,memsize) ;
    m_events = (struct pollfd*)membase ;
    m_handlers = (poll_data*)(m_events + max_count) ;

    m_max_count = max_count ;
    m_cur_count = 0 ;

    return 0 ;

}

void poll_reactor::fini()
{
    if ( m_events )
    {
        free(m_events) ;
        m_events = NULL ;
        m_handlers = NULL ;

    }

}
 
int poll_reactor::comp_poll_data(const void *m1, const void *m2)
{
    return ((poll_data*)m2)->event.fd - ((poll_data*)m1)->event.fd ;
}

io_handler* poll_reactor::get_handler(int fd)
{
    poll_data* poll_data = get_poll_data(fd) ;
    if(poll_data) return poll_data->handler ;
    return NULL ;

}


poll_reactor::poll_data* poll_reactor::get_poll_data(int fd)
{
    if(m_events == NULL || m_cur_count < 1 ) return NULL ;

    poll_data key ;
    key.event.fd = fd ;
    poll_data* result = (poll_data*)bsearch(&key,m_handlers,m_cur_count,sizeof(key),poll_reactor::comp_poll_data) ;

    return result ;
}

void poll_reactor::resort_poll_data()
{    
    if(m_cur_count >1 && m_events )
    {
        qsort(m_handlers,m_cur_count,sizeof(*m_handlers),comp_poll_data) ;
    }

}

int poll_reactor::add_handler(int fd, io_handler* h,int event_type)
{

    if ( h == NULL || m_events ==NULL ) return -1 ;
    if (   fd < 0 || m_cur_count  >= m_max_count ) return -2 ;

    if( get_poll_data(fd)!= NULL ) return -3 ;

    poll_data* poll_data = m_handlers + m_cur_count;
    poll_data->event.fd = fd ;

    if ( event_type & EVENT_READ )
    {
        poll_data->event.events |= POLLIN ;
    }

    if ( event_type & EVENT_WRITE )
    {
        poll_data->event.events |= POLLOUT ;
    }

    poll_data->handler = h ;

    ++m_cur_count ;
    resort_poll_data() ;

    m_prepare_flag =1 ;

    return 0 ;

}

int poll_reactor::mod_handler(int fd, io_handler* h ,int event_type )
{

    if ( h == NULL || m_events ==NULL ) return -1 ;

    if (   fd < 0  ) return -2 ;

    poll_data* poll_data = get_poll_data(fd) ;
    if(poll_data == NULL ) return -3 ;

    if ( event_type & EVENT_READ )
    {
        poll_data->event.events |= POLLIN ;
    }

    if ( event_type & EVENT_WRITE )
    {
        poll_data->event.events |= POLLOUT ;
    }

    poll_data->handler = h ;

    m_prepare_flag =1 ;

    return 0 ;
}
void poll_reactor::del_handler(int fd)
{
    if (   fd < 0 || m_events == NULL  ) return ;

    poll_data* poll_data = get_poll_data(fd) ;
    if(poll_data == NULL ) return  ;

    poll_data->handler = NULL ;
    poll_data->event.fd = -1 ;

    resort_poll_data() ;
    --m_cur_count ;
    m_prepare_flag =1 ;

}


int poll_reactor::run_once(int msec)
{
    if(m_events == NULL ) return -1 ;

    if(m_prepare_flag && m_cur_count >0)
    {
        for(int i = 0 ; i < m_cur_count ; ++i)
        {
            m_events[i] = m_handlers[i].event ;
        }

        m_prepare_flag = 0 ;
    }

    int n = poll(m_events,m_cur_count,msec) ;
    
    poll_data* poll_data = NULL ;

    for(int i = 0 ; i < m_cur_count ; ++i )
    {
        const struct pollfd* ev = m_events + i ;

        if ( ev->revents & (POLLPRI | POLLERR | POLLHUP) )
        {
            poll_data = get_poll_data(ev->fd) ;
            if ( poll_data != NULL && poll_data->handler !=NULL) 
            {
                poll_data->handler->on_error(ev->fd) ;
            }
        }
        else
        {
            if ( ev->revents & POLLOUT )
            {
                poll_data = get_poll_data(ev->fd) ;
                if ( poll_data != NULL && poll_data->handler !=NULL) 
                {
                    poll_data->handler->on_write(ev->fd) ;
                }

            }

            if ( ev->revents & POLLIN )
            {

                poll_data = get_poll_data(ev->fd) ;
                if ( poll_data != NULL && poll_data->handler !=NULL) 
                {
                    poll_data->handler->on_read(ev->fd) ;
                }
            }
        }

    }

    return n ;

}




}
