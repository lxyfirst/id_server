/**
 * rule_manager.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <stdint.h>
#include <string>
#include <tr1/memory>
#include <tr1/unordered_map>
#include "lua_manager.h"

struct RuleConfig
{
    int8_t step ;
    int8_t offset ;
    int16_t batch_save ;
    int min_counter ;
    int max_counter ;
    int reset_seconds ;
    std::string lua_file ;
} ;

struct Rule
{
    RuleConfig config ;
    LuaManager lua_manager ;
};

class RuleManager
{
public:
    typedef std::tr1::unordered_map<std::string,Rule > RuleContainer ;

public:
    RuleManager() ;
    ~RuleManager() ;


    int load_rule_config(const std::string& name,const std::string& lua_file,int batch_save) ;

    bool have_rule(const std::string& rule_name) const
    {
        return m_rule_list.count(rule_name) >0 ;
    }

    Rule* get_rule(const std::string& rule_name)
    {
        RuleContainer::iterator it = m_rule_list.find(rule_name) ;
        if(it == m_rule_list.end() ) return NULL ;
        return &it->second ;
    }

    int get_offset() const { return m_offset ; } ;
    int get_step() const { return m_step ; } ;

    int init(int offset,int step) ;

private:
    RuleManager(const RuleManager& o) ;
    RuleManager& operator=(const RuleManager& o) ;

private:
    RuleContainer m_rule_list ;
    int m_step ;
    int m_offset ;
};

