/**
 * udp_data_handler.h
 * Author : lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <errno.h>
#include <unistd.h>

#include "io_handler.h"
#include "buffer.h"
#include "network_util.h"
#include "base_reactor.h"
#include "packet.h"

namespace framework
{

struct udp_packet
{
    sa_in_t addr ;
    int data_size ;
    char data[0] ;
} ;

class udp_data_handler : public io_handler
{
public:
    enum { MAX_BUF_SIZE = 20480};
    enum
    {
        OPTION_READALL= 0x1 << 1 ,
    } ;

public:
    udp_data_handler();
    virtual ~udp_data_handler();

    /*
     * @brief create listen socket and add to reactor
     * @return 0 if success
     */
    int init(base_reactor& reactor,const char* host,int port) ;


    void fini() ;


    int fd() { return m_fd ; } ;

    int get_errno() const { return get_socket_error(m_fd) ; } ;

    /*
     * @brief send data
     * @return 0 on success , -1 on failure
     */
    int send(const sa_in_t* to_addr,const char* data,int size) ;
    int send(const char* host,int port,const char* data,int size) ;
    int send(const sa_in_t* to_addr,packet *p);

    int get_sock_addr(sa_in_t* addr) const ;

    void set_option(int option , bool flag) ;

    int get_options() const { return m_options;} ;

protected:

    /*
     * @brief handle exceptional event , implemented by concrete class
     * @param [in] exception type
     */
    virtual void on_event(int type);

    virtual int process_packet(const udp_packet* p) = 0 ;


protected:
    virtual void on_read(int fd) ;
    virtual void on_write(int fd) ;
    virtual void on_error(int fd) ;

    void handle_error(int error_type) ;

protected:
    base_reactor* m_reactor ;
    int m_fd ;
    int m_options;
    char m_buf[MAX_BUF_SIZE] ;
    char m_send_buf[MAX_BUF_SIZE] ;
};

}


