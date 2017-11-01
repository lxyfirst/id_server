/*
 * json_util.cpp
 *
 *  Created on: 2012-4-1
 *      Author: lxyfirst@163.com
 */

#include <string.h>
#include <stdlib.h>

#include "json.h"
#include "json_util.h"

bool json_brace_closed(const char* begin,int size)
{
    bool quotation = false ;
    int left_brace = 0 , right_brace = 0 ;
    for(const char* end = begin +size ;begin != end;++begin)
    {
        if(*begin == '"') quotation = !quotation ;
        switch(*begin)
        {
        case '\\':
            if(end == begin+1) return false ;
            if(*(begin+1) == '"' ) ++begin ;
            break ;
        case '"' :
            quotation = !quotation ;
            break ;
        case '{' :
            if(!quotation) left_brace +=1 ;
            break ;
        case '}' :
            if(!quotation) right_brace +=1 ;
            break ;
        case ' ' :
            break ;
        default:
            if(right_brace > left_brace) return false ;
        }
    }

    if(left_brace == right_brace) return true ;
    return false ;
}

bool json_decode(Json::Value& value,const char* begin,int size,Json::ValueType type)
{
    Json::Reader reader ;
    if(!reader.parse(begin,begin +size,value,false)) return false ;
    if(!value.isConvertibleTo(type) ) return false ;
    return true ;
}

bool json_decode(Json::Value& value,const std::string& data,Json::ValueType type)
{
    Json::Reader reader ;
    if(!reader.parse(data,value,false)) return false ;
    if(!value.isConvertibleTo(type) ) return false ;
    return true ;
}

bool json_encode(std::string& data,const Json::Value& value)
{
    Json::FastWriter writer ;
    writer.omitEndingLineFeed();
    data = writer.write(value) ;
    return true ;
}

bool json_check_field(const Json::Value& value,const JsonFieldInfo& field_list)
{
    for(auto& kv : field_list)
    {
        if(!value.isMember(kv.first) ) return false ;
        if(value[kv.first].type() != kv.second) return false ;
    }

    return true ;
}

int json_get_value(const Json::Value& value,const char* key,int default_value)
{
    if(!value.isMember(key) ) return default_value ;
    const Json::Value& v = value[key] ;
    return v.isInt() ? v.asInt() : default_value ;

}

const char* json_get_value(const Json::Value& value,const char* key,const char* default_value)
{
    if(!value.isMember(key) ) return default_value ;
    const Json::Value& v = value[key] ;
    return v.isString() ? v.asCString() : default_value ;

}



