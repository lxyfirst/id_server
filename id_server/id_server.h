/**
 * id_server.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <set>
#include <tr1/unordered_map>
#include <list>
#include <string>
#include <memory>
#include <tr1/memory>

#include "framework/application.h"
#include "framework/day_roll_logger.h"
#include "framework/log_thread.h"
#include "jsoncpp/json_util.h"

#include "data_thread_manager.h"

#include "udp_handler.h"
#include "rule_manager.h"
#include "counter_manager.h"

using std::string ;

class IdServer: public framework::application
{
public:
    IdServer();
    virtual ~IdServer();
    
public:

    framework::day_roll_logger& logger() { return m_logger ; } ;
    DataThreadManager& data_manager() { return m_thread_manager ; } ;

    Rule* get_rule(const string& rule_name)
    {
        return m_rule_manager.get_rule(rule_name) ;
    } ;

    Counter* get_counter(const string& rule_name,const string& app_name)
    {
        return m_counter_manager.get_counter(rule_name,app_name) ;
    } ;


    int create_id(string& new_id,const string& rule_name,const string& app_name,const string& salt);

    void create_format_id(string& new_id,const string& format,Counter* counter,int width_counter);
protected:
    const char* version() { return "version 1.0 compiled at " __TIME__ " "  __DATE__   ; } ;

    int on_init() ;
    
    int on_reload() ;

    void on_fini() ;

    void on_delay_stop() ;

    void on_timer() ;

    int load_config(Json::Value& config);

    int init_logger(const Json::Value& config);
    int load_rules(const Json::Value& config);


private:
    framework::day_roll_logger m_logger ;
    framework::log_thread m_log_thread ;
    DataThreadManager m_thread_manager ;
    UdpHandler m_client_handler ;

    CounterManager m_counter_manager ;
    RuleManager  m_rule_manager ;

    int m_request_count ;
    int m_total_time ;

};

inline IdServer& get_app() { return framework::singleton<IdServer>() ; };
