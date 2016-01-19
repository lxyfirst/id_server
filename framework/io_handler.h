/**
 * io_handler.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

namespace framework
{

    enum io_error_type
    {
        ERROR_TYPE_NONE = 0 ,
        ERROR_TYPE_SYSTEM = 1 ,
        ERROR_TYPE_MEMORY = 2 ,
        ERROR_TYPE_REQUEST = 3 ,
        ERROR_TYPE_TIMEOUT = 4 ,
        ERROR_TYPE_PEER_CLOSE = 5 ,
        ERROR_TYPE_ACTIVE_CLOSE = 6 ,
    };


/**
 * @brief  interface for event driven handler
 */
class io_handler
{
public:
    io_handler(){ } ;
    virtual ~io_handler() { } ;


public :

    /**
     * @brief error events callback , implemented by concrete class
     */
    virtual void on_error(int fd) = 0 ;


    /**
     * @brief read events callback , implemented by concrete class
     */
    virtual void on_read(int fd) = 0 ;


    /**
     * @brief write events callback , implemented by concrete class
     */
    virtual void on_write(int fd) = 0 ;

private:
    io_handler(const io_handler& ) ;
    io_handler& operator=(const io_handler&) ;


};

}


