/**
 * unix_config.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

namespace framework
{

class unix_config
{
public:
    enum
    {
        BUCKETS_BITS = 10 ,
        BUCKETS_MASK = ~((~0x0) << BUCKETS_BITS ) ,
        BUCKETS_SIZE = ( 0x1 << BUCKETS_BITS ) ,
        MAX_LINE_SIZE = 2048 ,
    };

    struct config_node_type
    {
        config_node_type* next ;
        unsigned int hash_value ;
        unsigned int key_size ;
        char key[0] ;
    };

public:
    const char* get(const char* key,const char* default_value = 0) ;

    int get(const char* key,int default_value) ;


    int set(const char* key,const char* value) ;

    int set(const char* key,int key_size,const char* value,int value_size) ;

    void remove(const char* key) ;

    int load(const char* file) ;

    int load(const char* data,int size) ;

    int save(const char* file) ;
    void dump(int fd) ;
    void clear() ;

public:
    unix_config();
    ~unix_config();

private:
    unix_config(const unix_config&) ;
    unix_config& operator=(const unix_config&) ;

private:
    config_node_type* get_node(const char* key,config_node_type** pre_node_return=0) ;
    int parse_line(const char* data) ;
    unsigned int hash(const char *p) ;
    unsigned int bucket(unsigned hash_value) {return hash_value & BUCKETS_MASK ;} ;
private:
    config_node_type* m_buckets[BUCKETS_SIZE] ;

};

}

