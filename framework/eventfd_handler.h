/**
 * eventfd_handler.h
 * Author : lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <tr1/functional>

#include "io_handler.h"

namespace framework
{

class base_reactor ;

/**
 * @brief eventfd handler for event notification
 */
class eventfd_handler : public io_handler
{
public:
    typedef std::tr1::function<void (int64_t) > callback_type ;
public:
    eventfd_handler()  ;
    virtual ~eventfd_handler()  ;

    int init(base_reactor& reactor,const callback_type& callback) ;

    void fini() ;


    int notify(int64_t count=1) ;
        

protected:
    void on_read(int fd) ;
    void on_write(int fd) ;
    void on_error(int fd) ;
private:
    callback_type m_callback ;
    base_reactor* m_reactor ;
    int m_event_fd ;
} ;

}


