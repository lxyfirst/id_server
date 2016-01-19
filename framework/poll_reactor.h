/**
 * poll_reactor.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <stdint.h>
#include <poll.h>

#include "base_reactor.h"

namespace framework
{

class io_handler ;

class poll_reactor : public base_reactor
{

public:

    typedef struct
    {
        struct pollfd  event ;
        io_handler*   handler ;
    } poll_data ;

    enum
    {
        MAX_FD_COUNT = 20480 ,
    } ;

    poll_reactor();
    ~poll_reactor();

public:
    /*
     * @brief initialize , create epoll fd and alloc memory
     * @param [in] max fd count 
     * @return 0 on success , nagtive on failed
     */
    int init(int max_count) ;

    void fini() ;

public:
    int add_handler(int fd , io_handler* handler,int event_type) ;

    int mod_handler(int fd , io_handler* handler ,int event_type ) ;

    void del_handler(int fd ) ;

    io_handler* get_handler(int fd)  ;

    int run_once(int msec) ;

    
private:
    static int comp_poll_data(const void *m1, const void *m2) ;
    poll_data* get_poll_data(int fd) ;
    void resort_poll_data() ;

private:
    struct pollfd* m_events ;
    poll_data* m_handlers ;
    int16_t m_max_count;
    int16_t m_cur_count;
    int8_t m_prepare_flag ;

};

}

