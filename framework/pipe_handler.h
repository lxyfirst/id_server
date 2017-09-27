/**
 * pipe_handler.h
 * Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <stdint.h>
#include <tr1/functional>
#include "io_handler.h"

namespace framework
{

class base_reactor ;
struct packet_info ;

class pipe_handler : public io_handler
{
public:
    typedef std::tr1::function<void (const packet_info*) > callback_type ;
    
public:
    pipe_handler() ;

    virtual ~pipe_handler();
public:
    /*
     * @return: 0 on success
     */
    int init(base_reactor& reactor,int pipe_fd,const callback_type& callback) ;

    void fini() ;
    /*
     @ @brief send packet_info message to pipe 
     * @return: 0 on success
     */
    int send_pipe_message(const packet_info* msg) ;

protected:
    void on_read(int fd) ;
    void on_write(int fd) ;
    void on_error(int fd) ;

private:
    callback_type m_callback ;
    base_reactor* m_reactor ;
    int m_pipe_fd ;
};

}
