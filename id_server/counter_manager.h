/**
 * counter_manager.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <string>
#include <tr1/unordered_map>

#include "framework/timer_manager.h"

using std::string ;

struct CounterData
{
    string rule_name ;
    string app_name ;
    int node_offset ;
    int counter ;
    int update_time ;
    int saved_counter ;
};

struct RuleConfig ;

class Counter
{
public:
    Counter() ;
    ~Counter() ;
    /**
     * @brief load data from database
     */
    void load(const CounterData& data) ;

    /**
     * @brief init data
     */
    void init(const string& rule_name,const string& app_name,const RuleConfig& rule_config);

    /**
     * @brief create auto increment counter
     * @return new counter
     */
    int generate_counter() ;

    /**
     * @brief get timestamp never rollback
     * @return timestamp
     */
    int generate_time() ;


    const CounterData& data() { return m_data ; } ;

     /**
     * @brief save data and auto try again if failed
     */
    void async_save(framework::timer_manager* manager = NULL) ;
private:
    Counter(const Counter& c) ;
    Counter& operator=(const Counter& c) ;

private:
    framework::template_timer<Counter,&Counter::async_save> m_timer ;
    CounterData m_data ;

};

std::string& get_counter_key(string& data,const char* rule_name,const char* app_name) ;


class CounterManager
{
public:
    typedef std::tr1::unordered_map<string,Counter*> CounterContainer ;

    Counter* get_counter(const string& rule_name,const string& app_name);


    /**
     * @brief  create counter and init
     */
    Counter* create_counter(const string& rule_name,const string& app_name,const RuleConfig& config);

    /**
     * @brief load counter data from database
     */
    Counter* load_counter(const CounterData& data) ;

public:
    CounterManager();
    ~CounterManager();


private:
    CounterManager(const CounterManager& o) ;
    CounterManager& operator=(const CounterManager& o) ;

    Counter* inner_create_counter(const string& rule_name,const string& app_name) ;
private:
    CounterContainer m_counter_list ;

};

