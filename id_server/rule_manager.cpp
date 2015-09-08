/*
 * rule_manager.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include "rule_manager.h"

RuleManager::RuleManager():m_step(0),m_offset(0)
{
	// TODO Auto-generated constructor stub

}

RuleManager::~RuleManager()
{
	// TODO Auto-generated destructor stub
}

int RuleManager::init(int offset,int step)
{
	if(m_step !=0 || step < 1 ||  offset <0 || offset >= step ) return -1 ;
	m_offset = offset ;
	m_step = step ;
	return 0 ;
}

int RuleManager::load_rule_config(pugi::xml_node& node)
{
	if(m_step == 0 ) return -1 ;

	RuleConfig config ;
	LuaManager lua_manager ;
	lua_manager.init() ;
	lua_manager.load(node.attribute("lua_file").value()) ;
	config.step = m_step;
	config.offset = m_offset ;
	//config.width_counter = lua_manager.width_counter();
	config.min_counter = lua_manager.min_counter();
	config.max_counter = lua_manager.max_counter();
	config.reset_seconds = lua_manager.reset_seconds();
	if(config.min_counter >= config.max_counter || config.min_counter < 0) return -1;


	Rule& rule = m_rule_list[node.attribute("name").value()] ;

	rule.lua_manager.init() ;

	rule.config = config ;
	rule.lua_manager.load(node.attribute("lua_file").value());




	return 0 ;

}
