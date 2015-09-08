/*
 * unix_config.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 #include <fcntl.h>

#include "unix_config.h"

namespace framework
{

static const char* node_delimiter_string = " \t\r=\n" ;

unsigned int unix_config::hash(const char *p)
{
    unsigned int h = 0;
    while(*p)
    {
        h = h * 11 + (*p << 4) + (*p >> 4);
        p++;
    }
    return h;
}


unix_config::unix_config()
{
    memset(m_buckets,0,sizeof(m_buckets)) ;

}

unix_config::~unix_config()
{
    clear() ;
}

unix_config::config_node_type* unix_config::get_node(const char* key,config_node_type** pre_node_return)
{
    if ( key == NULL ) return NULL ;

    unsigned int hash_value = hash(key) ;
    unsigned int key_size = strlen(key) ;

    if ( key_size > MAX_LINE_SIZE ) return NULL ;

    config_node_type* cur_node = m_buckets[ bucket(hash_value)  ] ;
    config_node_type* pre_node = NULL ;
    while( cur_node!=NULL )
    {
        if ( hash_value == cur_node->hash_value &&
                key_size == cur_node->key_size &&
                strncmp(cur_node->key,key,key_size)==0  )
        {
            if ( pre_node_return != NULL) *pre_node_return = pre_node ;
            return cur_node ;
        }

        pre_node = cur_node ;

        cur_node = cur_node->next ;
    }

    return NULL ;
}

const char* unix_config::get(const char* key,const char* default_value)
{
    config_node_type* node = get_node(key) ;
    if ( node != NULL ) return node->key + node->key_size + 1 ;

    return default_value ;
}

int unix_config::get(const char* key,int default_value)
{
    const char* v = get(key) ;
    if(v == NULL ) return default_value ;
    return atoi(v) ;
}

void unix_config::remove(const char* key)
{
    config_node_type* pre_node = NULL ;
    config_node_type* node = get_node(key,&pre_node) ;
    if ( node != NULL )
    {
        if ( pre_node != NULL )
        {
            pre_node->next = node->next ;
        }
        else
        {
            unsigned int bucket_index = bucket(hash(key)) ;
            m_buckets[ bucket_index] = node->next ;
        }

        free(node) ;


    }

}


int unix_config::set(const char* key,const char* value)
{
    if ( key == NULL || value == NULL ) return -1 ;

    unsigned int key_size = strlen(key) ;
    unsigned int value_size = strlen(value) ;
    return set(key,key_size,value,value_size) ;

}

int unix_config::set(const char* key,int key_size,const char* value,int value_size)
{

    if ( key_size > MAX_LINE_SIZE || value_size > MAX_LINE_SIZE ) return -1 ;

    unsigned int node_size = sizeof(config_node_type) + key_size + 1 + value_size + 1 ;

    config_node_type* node = (config_node_type*)malloc(node_size) ;
    if ( node == NULL ) return -1 ;


    node->key_size = key_size ;

    strncpy(node->key,key,key_size) ;
    node->key[key_size] = '\0' ;

    char* node_value = node->key + key_size + 1 ;
    strncpy(node_value,value,value_size) ;
    node_value[value_size] = '\0' ;

    config_node_type* pre_node = NULL ;
    config_node_type* old_node = get_node(node->key,&pre_node) ;
    if ( old_node != NULL)
    {
         node->hash_value = old_node->hash_value ;
         node->next = old_node->next ;

         if ( pre_node == NULL )
         {
             m_buckets[ bucket(node->hash_value)] = node ;
         }
         else
         {
             pre_node->next = node ;
         }

         free(old_node) ;
    }
    else
    {
        node->hash_value = hash(node->key) ;
        unsigned int bucket_index = bucket(node->hash_value) ;
        node->next = m_buckets[bucket_index] ;
        m_buckets[ bucket_index] = node ;

    }


    return 0 ;
}

int unix_config::load(const char* filename)
{

    if ( filename == NULL ) return -1 ;

    char line[MAX_LINE_SIZE] = {0} ;

    FILE* fp = fopen(filename,"r") ;
    if ( fp == NULL ) return -1 ;

    while(fgets(line,MAX_LINE_SIZE-1,fp))
    {
       parse_line(line) ;
    }
    fclose(fp) ;
    return 0 ;
}

int unix_config::parse_line(const char* data)
{
    const char *key = NULL ;
    const char *val = NULL ;
    int key_size = 0 ;
    int val_size = 0 ;
    const char* p = data ;
    p += strspn(p,node_delimiter_string);
    if ( *p == '\0' || *p == '#' || *p=='\n' ) return -1 ;
    key = p ; /* start of key */
    key_size = strcspn(p,node_delimiter_string);
    //strncpy(key_data,key,key_size) ;
    p += key_size ;
    p += strspn(p,node_delimiter_string); /* start of value */
    if ( *p == '\0' || *p == '#' || *p=='\n' ) return -1 ;
    val = p ;
    val_size = strcspn(p,node_delimiter_string);
    //strncpy(val_data,val,val_size) ;
    p += val_size ;

    return set(key,key_size,val,val_size) ;

}

int unix_config::load(const char* data,int size)
{
    char line[MAX_LINE_SIZE] = {0} ;
    const char* begin = data ;
    const char* end = data + size -1 ;
    int line_size = 0 ;
    for(const char* p = data ; p<= end ; ++p)
    {
        if(*p == '\n' || p == end )
        {
            line_size = p - begin +1  ;
            strncpy(line,begin,line_size) ;
            line[line_size] = '\0' ;
            parse_line(line) ;

            begin = p +1 ;
        }
    }
    return 0 ;
}

int unix_config::save(const char* filename)
{
    int fd = open(filename,O_CREAT | O_WRONLY,0755) ;
    if ( fd < 0 ) return -1 ;
    dump(fd) ;
    close(fd) ;
    return 0 ;
}
void unix_config::dump(int fd)
{
    char line[MAX_LINE_SIZE] = {0} ;
    for(int i = 0 ; i < BUCKETS_SIZE ; ++i )
    {

        config_node_type* node = m_buckets[i] ;
        while(node!=NULL)
        {
            snprintf(line,sizeof(line),"%s=%s\n",node->key,node->key+node->key_size+1) ;
            write(fd,line,strlen(line)) ;
            node = node->next ;
        }
    }

}
void unix_config::clear()
{

    for(int i = 0 ; i < BUCKETS_SIZE ; ++i )
    {
        config_node_type* node = m_buckets[i] ;
        m_buckets[i] = NULL ;

        while(node!=NULL)
        {
            config_node_type* next_node = node->next ;
            free(node) ;
            node = next_node ;
        }
    }



}


}
