/**
 * packet_processor.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

namespace framework
{

class tcp_connection ;
struct packet_info ;

class packet_processor
{
public:
    virtual ~packet_processor() { } ;


    /*
     * @brief get packet info , implemented by concrete class
     * @param [in] buffer pointer
     * @param [in] buffer size
     * @param [out] packet info
     * @return 0 on success , -1 on failure
     */
    virtual int get_packet_info(tcp_connection* conn,const char* data,int size,packet_info* pi) = 0 ;


    /*
     * @brief process packet callback , implemented by concrete class
     * @param [in] connection pointer
     * @param [in] packet info
     * @return 0 on success , -1 on failure
     */
    virtual int process_packet(tcp_connection* conn,const packet_info* pi) = 0 ;

    /*
     * @brief connect event callllback , implemented by concrete class
     * @param [in] connection pointer
     */
    virtual void on_connected(tcp_connection* conn) = 0 ;
    

    /*
     * @brief disconnect event callllback , implemented by concrete class
     * @param [in] connection pointer
     * @param [in] io_error_type
     */
    virtual void on_closed(tcp_connection* conn,int error_type) = 0 ;

};

}

