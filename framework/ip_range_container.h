/**
 * ip_range_container.h
 * Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <arpa/inet.h>
#include <vector>


namespace framework
{


class ip_range_container
{
public:
    struct ip_range_t
    {
        in_addr_t begin_ip ;
        in_addr_t end_ip ;
    };

public:
    ip_range_container();
    virtual ~ip_range_container();

    bool add_ip_mask(const char* str_ip,const char* str_mask) ;
    bool add_ip_mask(in_addr_t ip,in_addr_t mask) ;

    bool add_ip_range(const char* str_begin_ip,const char* str_end_ip) ;
    bool add_ip_range(in_addr_t begin_ip,in_addr_t end_ip) ;

    bool match(in_addr_t ip) const ;
    bool match(const char* str_ip) const ;

    void clear() { m_ip_list.clear() ; } ;
    int size() { return m_ip_list.size() ; } ;

private:
    void resort() ;
    bool inner_match(const ip_range_t* obj) const;

private:
    std::vector<ip_range_t> m_ip_list ;
};

} 

