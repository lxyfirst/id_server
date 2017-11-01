/**
 * udp_handler.h
 * Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <string>
#include "framework/udp_data_handler.h"
#include "jsoncpp/json_value.h"

using std::string ;
using framework::sa_in_t ;

class UdpHandler: public framework::udp_data_handler
{
public:
    UdpHandler();
    virtual ~UdpHandler();

    int process_packet(const framework::udp_packet* p);

    int send_response(const sa_in_t& to_addr,Json::Value& response,int code =0,const char* message = NULL);
protected:
    /**
     * @brief process create id request
     */
    int process_action_create(Json::Value& request,const sa_in_t& from_addr);

    /**
     * @brief process monitor request
     */
    int process_action_monitor(Json::Value& request,const sa_in_t& from_addr);

};

