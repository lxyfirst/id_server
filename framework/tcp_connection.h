/*
 * tcp_connection.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#ifndef TCP_CONNECTION_H_
#define TCP_CONNECTION_H_

#include "io_handler.h"
#include "network_util.h"
#include "buffer.h"
#include "packet.h"

namespace framework
{

class packet_processor ;
class base_reactor ;

struct connection_id
{
    int fd ;
    int timestamp ;

    bool operator!=(const connection_id& o)
    {
        return ( fd!=o.fd || timestamp != o.timestamp) ;
    }

    bool operator==(const connection_id& o)
    {
        return ( fd==o.fd && timestamp == o.timestamp) ;
    }

}  ;

class tcp_connection : public io_handler
{
public:


    enum
    {
        INIT_RECV_BUF_SIZE = 40960 ,
        INIT_SEND_BUF_SIZE = 40960 ,
        INIT_BUF_SIZE = 40960 ,
    };

    enum
    {
        MIN_WRITE_SIZE = 0x1 << 10 ,
        MAX_WRITE_SIZE = 0x1 << 24 ,
    };

    //connection status
    enum
    {
        STATUS_CLOSED = 0 , 
        STATUS_CONNECTING = 1 ,
        STATUS_CONNECTED = 2 ,
    } ;

    //connection options
    enum
    {
        OPTION_NODELAY = 0x1 ,      // not implemented
        OPTION_READALL = 0x1 << 1 ,
    } ;

public:
    tcp_connection();
    virtual ~tcp_connection();


public:
    /*
     * @brief initialize connection , for active connection
     * @param [in] event-driven engine which monitor read/write events
     * @param [in] remote address to connect
     * @param [in] remote port to connect
     * @param [in] packet processor
     * @return 0 on sucess , nagtive value on failure
     */
    int init(base_reactor* reactor,const char* host,int port,packet_processor* processor) ;
    /*
     * @brief initialize connection ,for passive connection
     * @param [in] event-driven engine which monitor read/write events
     * @param [in] fd for monitor
     * @param [in] packet processor
     * @return 0 on sucess , nagtive value on failure
     */
    int init(base_reactor* reactor,int fd,packet_processor* processor ) ;
    
    void detach_reactor() ;
    int attach_reactor(base_reactor* reactor) ;

    /*
     * @brief clear connection and buffer
     * @param [in] release buffer memroy flag
     */
    void fini(bool release=false) ;

    /*
     * @brief get connection_id
     */
    const connection_id& get_id() const { return m_id ; } ;

    //int8_t get_status() const { return m_connect_status ; } ;
    bool is_connected() const { return m_connect_status == STATUS_CONNECTED;} ;
    bool is_closed() const { return m_connect_status == STATUS_CLOSED ; } ;

    int8_t get_options() const { return m_options ; } ;

    int get_errno() const { return get_socket_error(m_id.fd) ; } ;

    /*
     * @brief send data
     * @return 0 on success , -1 on failure
     */
    int send(const char* data,int size,int delay_flag) ;

    /*
     * @brief send packet
     * @return 0 on success , -1 on failure
     */
    int send(packet * p,int delay_flag = 0) ;

    /*
     * @brief local address
     */
    int get_sock_addr(sa_in_t* addr) const ;

    /*
     * @brief remote address
     */
    int get_remote_addr(sa_in_t* addr) const ;

    void set_option(int option , bool flag) ;

    void set_max_write_size(int max_size) 
    { 
        if(max_size < MIN_WRITE_SIZE) max_size = MIN_WRITE_SIZE ;
        m_max_write_size = max_size ; 
    } ;
    
protected:

    /*
     * @brief called after connection established 
     */
    virtual void on_connected() { } ;
    
    /*
     * @brief called after connection closed actively or passively
     */
    virtual void on_closed(int error_type,int error_no) { } ;

    /*
     * @brief get packet info , implemented by concrete class
     * @param [in] buffer pointer
     * @param [in] buffer size
     * @param [out] packet info
     * @return 0 on success , -1 on failure
     */
    virtual int get_packet_info(const char* data,int size,packet_info* pi) = 0 ;

    /*
     * @brief process packet callback , implemented by concrete class
     * @param [in] packet info
     * @return 0 on success , -1 on failure
     */
    virtual int process_packet(const packet_info* pi) = 0 ;


private:
    virtual void on_read(int fd) ;
    virtual void on_write(int fd) ;
    virtual void on_error(int fd) ;

    void handle_error(int error_type) ;
    void update_status(int status,int error_type = ERROR_TYPE_NONE,int errno_no = 0) ;
    void inner_fini(bool release) ;
    
private:
    buffer m_rbuf ;
    buffer m_sbuf ;
    connection_id m_id ;
    base_reactor* m_reactor ;
    packet_processor* m_processor ;
    int m_max_write_size ;
    int8_t m_connect_status ;
    int8_t m_options ;

};




}

#endif /* TCP_DATA_HANDLER_H_ */
