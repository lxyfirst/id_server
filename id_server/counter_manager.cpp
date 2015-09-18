/*
 * counter_manager.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include <stdio.h>
#include "framework/time_util.h"
#include "counter_manager.h"

#include "id_server.h"

std::string&  get_counter_key(std::string& data , const char* rule_name,const char* app_name)
{
    char key[256] = {0} ;
    snprintf(key,sizeof(key),"%s_%s",rule_name,app_name) ;
    data.assign(key) ;
    return data ;
}


void Counter::load(const CounterData& data)
{
    m_data = data ;
}

void Counter::init(const string& rule_name,const string& app_name,const RuleConfig& rule_config)
{
    m_data.app_name.assign(app_name) ;
    m_data.rule_name.assign(rule_name) ;
    m_data.node_offset = rule_config.offset ;
    m_data.counter = rule_config.min_counter + rule_config.offset ;
    m_data.update_time = 0 ;
    m_data.saved_counter = m_data.counter ;
}

int Counter::generate_time()
{
    int now = time(0) ;
    return now > m_data.update_time ? now : m_data.update_time ;
}

int Counter::generate_counter()
{
    Rule* rule = get_app().get_rule(m_data.rule_name) ;
    if(rule == NULL || rule->config.offset != m_data.node_offset ) return -1 ;

    RuleConfig& config = rule->config ;

    m_data.counter += config.step ;
    int now = generate_time() ;

    if( (!framework::is_same_cycle(m_data.update_time,now,config.reset_seconds)) ||
             (m_data.counter > config.max_counter)  )
    {
        m_data.counter = config.min_counter + config.offset ;
        m_data.saved_counter = m_data.counter ;
    }

    m_data.update_time = now ;

    if(m_data.counter >= m_data.saved_counter)
    {
        
        m_data.saved_counter = m_data.counter + config.step * batch_save ;
        async_save() ;
    }


    return m_data.counter ;

}

void Counter::async_save()
{
    get_app().data_manager().async_update(m_data) ;
}

CounterManager::CounterManager()
{
    // TODO Auto-generated constructor stub

}

CounterManager::~CounterManager()
{
    // TODO Auto-generated destructor stub
}


Counter* CounterManager::load_counter(const CounterData& data)
{
    std::string key ;
    get_counter_key(key,data.rule_name.c_str(),data.app_name.c_str());

    Counter& counter = m_counter_list[key] ;
    counter.load(data) ;

    return &counter ;

}

Counter* CounterManager::create_counter(const string& rule_name,const string& app_name )
{
    std::string key ;
    get_counter_key(key,rule_name.c_str(),app_name.c_str());

    Counter& counter = m_counter_list[key] ;

    return &counter ;

}

Counter* CounterManager::get_counter(const std::string& rule_name,const std::string& app_name)
{
    std::string key ;
    get_counter_key(key,rule_name.c_str(),app_name.c_str());

    CounterContainer::iterator it = m_counter_list.find(key) ;
    if(it == m_counter_list.end() ) return NULL ;
    return &it->second ;
}

