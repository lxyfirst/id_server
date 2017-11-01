/*
 * server_packet.h
 *
 *  Created on: 2012-4-1
 *      Author: lxyfirst@163.com
 */

#pragma once

#include "json_value.h"

/*
 * @brief check brace close in json string
 * @param [in] buffer
 * @param [in] size
 * @return true on success
 */
bool json_brace_closed(const char* begin,int size);

/*
 * @brief parse json string to object
 * @param [out] value , json object
 * @param [in] buffer
 * @param [in] size
 * @return true on success
 */
bool json_decode(Json::Value& value,const char* begin,int size,Json::ValueType type = Json::objectValue) ;
bool json_decode(Json::Value& value,const std::string& data,Json::ValueType type = Json::objectValue) ;

/*
 * @brief encode json object to string
 * @param [out] data buffer
 * @param [in] value ,json object
 * @return true on success
 */
bool json_encode(std::string& data,const Json::Value& value) ;

typedef std::map<std::string,int > JsonFieldInfo ;

/*
 *  @brief check field and value type of json object
 *  @return true on success
 **/
bool json_check_field(const Json::Value& value,const JsonFieldInfo& field_list) ;

/**
 * @brief get int value from json object
 * @param [in] value json object
 * @param [in] key
 * @param [in] default_value
 * @return value
 **/
int json_get_value(const Json::Value& value,const char* key,int default_value);

/**
 * @brief get string value from json object
 * @param [in] value json object
 * @param [in]key
 * @param [in] default_value
 * @return value
 **/
const char* json_get_value(const Json::Value& value,const char* key,const char* default_value);

