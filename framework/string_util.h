/**
 * string_util.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <vector>
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

namespace framework
{

std::string& int2str(std::string& str,long int value) ;

int bin2hex(char* hex,const unsigned char* bin,int size) ;
int hex2bin(unsigned char* bin,const char* hex,int size) ;

inline int base64_encode_size(int len)
{
    return (((len + 2) / 3) * 4) ;
}

inline int base64_decode_size(int len)
{
    return (((len + 3) / 4) * 3) ;
}

int base64_encode(unsigned char* dst,const unsigned char* src,int src_size) ;
int base64_decode(unsigned char* dst,const unsigned char* src,int src_size) ;

struct md5_context
{
    uint64_t  bytes;
    uint32_t  a, b, c, d;
    unsigned char buffer[64];
} ;

void md5_init(md5_context *ctx);
void md5_update(md5_context *ctx, const void *data, size_t size);
void md5_final(unsigned char result[16], md5_context *ctx);

void md5(std::string& digest,const void *data, int size);


int sql_escape_string(char* buf,int size,const char* data,int data_size) ;

typedef std::vector<std::string> string_vector ;

/*
 * @brief split string by seperator
 * @return  count
 */
int split(string_vector& dst,const char* src,int size,char sep=' ') ;
int split(string_vector& dst,const char* src,int size,const char* sep,bool ignore_empty=true) ;


}

