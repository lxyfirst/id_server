/**
 * tcp_acceptor.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <tr1/functional>

#include <errno.h>
#include <unistd.h>
#include "io_handler.h"
#include "network_util.h"

namespace framework
{

class base_reactor ;

class tcp_acceptor : public io_handler
{
public:
    enum
    {
        MAX_ACCEPT_ONCE = 32 ,
    };

    typedef std::tr1::function<int(int,sa_in_t*)> callback_type ;

public:
    tcp_acceptor();
    virtual ~tcp_acceptor();

    /*
     * @brief create listen socket and add to reactor
     * @return 0 if success
     */
    int init(base_reactor& reactor,const char* host,int port,const callback_type& callback) ;


    void fini() ;


    int fd() { return m_fd ; } ;

protected:
    virtual void on_read(int fd) ;
    virtual void on_write(int fd) ;
    virtual void on_error(int fd) ;



protected:
    base_reactor* m_reactor ;
    callback_type m_callback ;
    int m_fd ;
};


}

