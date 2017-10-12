/**
 * queue_handler.h
 * Author : lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <sys/eventfd.h>
#include <unistd.h>

#include <functional>
#include <mutex>

#include "io_handler.h"
#include "base_reactor.h"
#include "circular_queue.h"

namespace framework
{

/**
 * @brief eventfd handler for event notification
 */
template<typename T>
class queue_handler : public io_handler 
{
public:
    typedef std::function<void(T*) > callback_type ;
public:
    queue_handler():m_reactor(NULL),m_event_fd(0)  {} ;
    virtual ~queue_handler()  {fini() ;} ;

    int init(base_reactor& reactor,int queue_size,const callback_type& callback) 
    {
        if(m_event_fd >0) return -1 ;
        if(m_queue.init(queue_size) !=0 ) return -1 ;

        m_event_fd = eventfd(0,EFD_NONBLOCK) ;
        if(m_event_fd < 0) return -1 ;
        reactor.add_handler(m_event_fd,this,base_reactor::EVENT_READ) ;
        m_reactor = &reactor ;
        m_callback = callback ;

        return 0 ;

    }



    void fini() 
    {
        if(m_event_fd >0) 
        {
            m_reactor->del_handler(m_event_fd) ;
            close(m_event_fd) ;
            m_event_fd = 0 ;
        }

        m_queue.fini() ;
        
    }

    int send(T* msg) 
    {
        m_lock.lock() ;
        int ret = m_queue.push(msg);
        m_lock.unlock() ;

        if(ret !=0) return -1 ;

        int64_t v =1;
        write(m_event_fd,(char*)&v,sizeof(v)) ;
        return 0 ;
    }

protected:
    void on_read(int fd)
    {
        int64_t v ;
        read(m_event_fd,&v,sizeof(v)) ;
        T* msg ;
        while( m_queue.pop(msg) == 0 )
        {
            m_callback(msg) ;
        }
    } 
    void on_write(int fd) {};
    void on_error(int fd) {};
private:
    std::mutex m_lock ;
    circular_queue<T*> m_queue ;
    callback_type m_callback ;
    base_reactor* m_reactor ;
    int m_event_fd ;
    
} ;

}


