
/**
 *
 * Author : lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <string>
#include <time.h>

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
};

#include "lua_tinker.h"

class Counter ;

class LuaManager
{
public:
    LuaManager();
    ~LuaManager();
    
    bool is_init() const  { return m_lua != NULL ; } ;

    int init() ;

    /*
     * @brief load lua script
     * @return 0 on success
     */
    int load(const char* lua_file) ;

    /*
     * @brief create unique id
     *
     */
    void create_id(std::string& new_id,Counter* counter,const std::string& salt);

    int min_counter() ;
    int max_counter() ;
    int reset_seconds() ;

private:
    lua_State* m_lua ;

} ;


class DateTime
{
public:
    int year() const {return tm_now.tm_year ;};
    int month() const{return tm_now.tm_mon ;};
    int mday() const { return tm_now.tm_mday ;} ;
    int yday() const { return tm_now.tm_yday ;} ;
    int hour() const { return tm_now.tm_hour;} ;
    int minute() const { return tm_now.tm_min ;} ;
    int second() const { return tm_now.tm_sec ;} ;

    DateTime(time_t t)
    {
        set_time(t) ;
    }

    void set_time(time_t t)
    {
        localtime_r(&t,&tm_now) ;
        tm_now.tm_year += 1900 ;
        tm_now.tm_mon += 1 ;
    }

private:
    struct tm tm_now ;
};

