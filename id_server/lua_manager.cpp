
/**
 *
 * Author : lixingyi (lxyfirst@163.com)
 */

#include "framework/time_util.h"

#include "lua_manager.h"
#include "lua_tinker.h"
#include "counter_manager.h"

using namespace std ;




LuaManager::LuaManager():m_lua(NULL)
{

}

LuaManager::~LuaManager()
{
    if(m_lua) lua_close(m_lua);

}


void LuaManager::create_id(std::string& new_id,Counter* counter,const std::string& salt)
{
    DateTime now(counter->generate_time() ) ;

    char* result = lua_tinker::call<char*>(m_lua,"create_id",counter,&now ,salt.c_str() ) ;
    if(result)
    {
        new_id.assign(result) ;
    }
}

int LuaManager::min_counter()
{
    return lua_tinker::call<int>(m_lua,"min_counter") ;
}

int LuaManager::max_counter()
{
    return lua_tinker::call<int>(m_lua,"max_counter") ;
}

int LuaManager::reset_seconds()
{
    return lua_tinker::call<int>(m_lua,"reset_seconds") ;
}


int LuaManager::load(const char* lua_file)
{
    if(NULL == m_lua) return -1 ;
    lua_tinker::dofile(m_lua,lua_file) ;
    return 0 ;
}



int LuaManager::init()
{
    if(m_lua) return -1 ;

    m_lua = lua_open();
    if(NULL == m_lua) return -1 ;
    luaopen_base(m_lua);
    luaopen_string(m_lua);
    luaopen_os(m_lua) ;

    //lua_tinker::class_add<LuaManager>(m_lua, "LuaManager");
    //lua_tinker::class_def<LuaManager>(m_lua, "register_handler", &LuaManager::register_handler);
    //lua_tinker::set<LuaManager*>(m_lua, "lua_manager", this );

    lua_tinker::class_add<Counter>(m_lua, "Counter");
    lua_tinker::class_def<Counter>(m_lua, "generate_counter", &Counter::generate_counter);

    lua_tinker::class_add<DateTime>(m_lua, "DateTime");
    lua_tinker::class_def<DateTime>(m_lua, "year", &DateTime::year);
    lua_tinker::class_def<DateTime>(m_lua, "month", &DateTime::month);
    lua_tinker::class_def<DateTime>(m_lua, "day", &DateTime::mday);
    lua_tinker::class_def<DateTime>(m_lua, "hour", &DateTime::hour);
    lua_tinker::class_def<DateTime>(m_lua, "minute", &DateTime::minute);
    lua_tinker::class_def<DateTime>(m_lua, "second", &DateTime::second);

    return 0 ;
}

