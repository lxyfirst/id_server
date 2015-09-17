/*
 * udp_handler.cpp
 * Author: lixingyi (lxyfirst@163.com)
 */

#include <stdio.h>
#include <string>

#include "jsoncpp/json.h"
#include "udp_handler.h"

#include "id_server.h"

using namespace framework ;


UdpHandler::UdpHandler()
{
    // TODO Auto-generated constructor stub

}

UdpHandler::~UdpHandler()
{
    // TODO Auto-generated destructor stub
}


int UdpHandler::send_response(const sa_in_t& to_addr,int code,const char* message,const char* seq,const char* data)
{
    char buffer[256] ;

    int len = snprintf(buffer,sizeof(buffer),
            "{\"code\":%d,\"message\":\"%s\",\"seq\":\"%s\",\"data\":\"%s\"}",
            code , message ,seq,data );

    return this->send(&to_addr,buffer,len) ;
}

int UdpHandler::process_packet(const udp_packet* pi)
{

    if(pi->data[0] != '{')
    {
        return send_response(pi->addr,-1,"invalid format") ;
    }

    Json::Reader reader ;
    Json::Value request ;
    if(! reader.parse(pi->data,pi->data + pi->data_size,request,false) )
    {
        return send_response(pi->addr,-1,"invalid format") ;
    }
    //request : {action,app_name,rule_name,salt}
    if(!request["app_name"].isString()) return send_response(pi->addr,-1,"need app_name") ;

    if(!request["rule_name"].isString()) return send_response(pi->addr,-1,"need rule_name");

    if(!request["action"].isString()) return send_response(pi->addr,-1,"need action");

    string action = request["action"].asString() ;
    string app_name = request["app_name"].asString() ;
    string rule_name = request["rule_name"].asString() ;
    string seq = request["seq"].isString() ? request["seq"].asString() : "" ;

    if(action.compare("get")==0 || action.compare("create")==0)
    {
        std::string salt ;
        if(request["salt"].isString() ) salt = request["salt"].asString();

        return process_action_create(rule_name,app_name,seq,salt,pi->addr);

    }
    else
    {
        return process_action_monitor(rule_name,app_name,seq,pi->addr) ;
    }


}

int UdpHandler::process_action_create(const string& rule_name,const string& app_name,const string& seq,const string& salt,const sa_in_t& from_addr)
{
    std::string new_id ;

    if( get_app().create_id(new_id,rule_name,app_name,salt)== 0 && (new_id.size() > 1) )
    {
        send_response(from_addr,0,"success",seq.c_str(),new_id.c_str()) ;
    }
    else
    {
        send_response(from_addr,-1,"system error",seq.c_str()) ;
    }

    debug_log_format(get_app().logger(),"create_id rule_name:%s app_name:%s salt:%s id:%s",
            rule_name.c_str(),app_name.c_str(),salt.c_str(),new_id.c_str() );

    return 0 ;
}

int UdpHandler::process_action_monitor(const string& rule_name,const string& app_name,const string& seq,const sa_in_t& from_addr)
{
    Counter* counter = get_app().get_counter(rule_name,app_name) ;
    if(counter == NULL )
    {
        send_response(from_addr,-1,"not exist",seq.c_str()) ;
    }
    else
    {
        char buffer[256] = {0} ;
        int len = snprintf(buffer,sizeof(buffer),
                "{\"code\":0,\"message\":\"ok\",\"message\":\"%s\",\"data\":{\"counter\":%d,\"node_offset\":%d}}",
                    seq.c_str(),counter->data().counter , counter->data().node_offset);

            return this->send(&from_addr,buffer,len) ;
    }

    return 0 ;
}

