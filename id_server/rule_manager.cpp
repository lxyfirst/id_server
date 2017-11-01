/**
 * rule_manager.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include "rule_manager.h"

RuleManager::RuleManager():m_step(0),m_offset(0)
{

}

RuleManager::~RuleManager()
{
}

int RuleManager::init(int offset,int step)
{
    if(m_step !=0 || step < 1 ||  offset <0 || offset >= step ) return -1 ;
    m_offset = offset ;
    m_step = step ;
    return 0 ;
}

int RuleManager::load_rule_config(const std::string& name,const std::string& lua_file,int batch_save)
{
    if(m_step == 0 ) return -1 ;

    RuleConfig config ;
    LuaManager lua_manager ;
    lua_manager.init() ;
    lua_manager.load(lua_file.c_str()) ;
    config.step = m_step;
    config.offset = m_offset ;
    config.batch_save = batch_save ;
    config.min_counter = lua_manager.min_counter();
    config.max_counter = lua_manager.max_counter();
    config.reset_seconds = lua_manager.reset_seconds();
    if(config.min_counter >= config.max_counter || config.min_counter < 0) return -1;
    if(config.batch_save < 1 ) config.batch_save = 1 ;
    if(config.batch_save > 10000 ) config.batch_save =10000 ;


    Rule& rule = m_rule_list[name] ;

    rule.config = config ;
    rule.lua_manager.init() ;
    rule.lua_manager.load(lua_file.c_str());




    return 0 ;

}
