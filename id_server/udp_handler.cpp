/**
 * udp_handler.cpp
 * Author: lixingyi (lxyfirst@163.com)
 */

#include <stdio.h>
#include <string>

#include "jsoncpp/json_util.h"
#include "udp_handler.h"

#include "id_server.h"

using namespace framework ;


UdpHandler::UdpHandler()
{

}

UdpHandler::~UdpHandler()
{
}



int UdpHandler::process_packet(const udp_packet* pi)
{

    debug_log_format(get_app().logger(),"recv:%s",pi->data) ;
    if(pi->data[0] != '{') return -1 ;
    
    Json::Value request ;
    if(!json_decode(request,pi->data,pi->data_size)) return -1 ;

    static const JsonFieldInfo request_field_list{
        {"action",Json::stringValue},
        {"app_name",Json::stringValue },
        {"rule_name",Json::stringValue },
    } ;

    if(!json_check_field(request,request_field_list)) return -1 ;

    string action = json_get_value(request,"action","") ;

    if(action.compare("get")==0 || action.compare("create")==0)
    {
        std::string salt = json_get_value(request,"salt","") ;
        return process_action_create(request,pi->addr);

    }
    else
    {
        return process_action_monitor(request,pi->addr) ;
    }


}

int UdpHandler::process_action_create(Json::Value& request,const sa_in_t& from_addr)
{
    string app_name = json_get_value(request,"app_name","") ;
    string rule_name = json_get_value(request,"rule_name","") ;
    string salt = json_get_value(request,"salt","") ;

    std::string new_id ;
    if( get_app().create_id(new_id,rule_name,app_name,salt)== 0 && (new_id.size() > 0) )
    {
        trace_log_format(get_app().logger(),"create_id rule_name:%s app_name:%s salt:%s id:%s",
            rule_name.c_str(),app_name.c_str(),salt.c_str(),new_id.c_str() );

        request["data"] = new_id ;
        return send_response(from_addr,request) ;
    }
    else
    {
        return send_response(from_addr,request,-1,"system error") ;
        
    }


}

int UdpHandler::process_action_monitor(Json::Value& request,const sa_in_t& from_addr)
{
    string app_name = json_get_value(request,"app_name","") ;
    string rule_name = json_get_value(request,"rule_name","") ;

    Counter* counter = get_app().get_counter(rule_name,app_name) ;
    if(counter == NULL )
    {
        return send_response(from_addr,request,-1,"not exist") ;
    }
    else
    {
        Json::Value data(Json::objectValue) ;
        data["counter"] = counter->data().counter ;
        data["node_offset"] = counter->data().node_offset;
        request["data"] = data ;
        return send_response(from_addr,request) ;
    }

    return -1 ;

}

int UdpHandler::send_response(const sa_in_t& to_addr,Json::Value& response,int code,const char* message)
{
    response["code"] = code ;
    if(message !=NULL) response["message"] = message ;

    std::string buffer ;
    json_encode(buffer,response) ;
    return this->send(&to_addr,buffer.data(),buffer.size() ) ;
}

