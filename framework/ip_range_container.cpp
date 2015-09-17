/*
 * ip_range_container.cpp
 * Author: lixingyi (lxyfirst@163.com)
 */

#include <stdlib.h>

#include "packet.h"
#include "ip_range_container.h"

namespace framework
{

ip_range_container::ip_range_container()
{
    // TODO Auto-generated constructor stub

}

ip_range_container::~ip_range_container()
{
    // TODO Auto-generated destructor stub
}


bool ip_range_container::add_ip_mask(const char* str_ip,const char* str_mask)
{
    in_addr_t ip = inet_addr(str_ip) ;
    in_addr_t mask = inet_addr(str_mask) ;
    return add_ip_mask(ip,mask) ;
}

bool ip_range_container::add_ip_mask(in_addr_t ip,in_addr_t mask)
{
    ip = ip & mask ;
    return add_ip_range(ip,ip | (~mask) ) ;

}

bool ip_range_container::add_ip_range(const char* str_begin_ip,const char* str_end_ip)
{
    in_addr_t begin_ip = inet_addr(str_begin_ip) ;
    in_addr_t end_ip = inet_addr(str_end_ip) ;
    return add_ip_range(begin_ip,end_ip) ;
}

bool ip_range_container::add_ip_range(in_addr_t begin_ip,in_addr_t end_ip)
{
    if(begin_ip == INADDR_NONE || end_ip == INADDR_NONE)
    {
        return false ;
    }

    begin_ip = ntoh_int32(begin_ip) ;
    end_ip = ntoh_int32(end_ip) ;
    if(begin_ip > end_ip)
    {
        return false ;
    }

    ip_range_t ip_range = { begin_ip , end_ip } ;
    if(inner_match(&ip_range))
    {
        return false ;
    }

    m_ip_list.push_back(ip_range) ;
    resort() ;

    return true ;
}

bool ip_range_container::match(const char* str_ip) const
{
    in_addr_t ip = inet_addr(str_ip) ;
    return match(ip) ;
}

bool ip_range_container::match(in_addr_t ip) const
{
    ip = ntoh_int32(ip) ;
    ip_range_t obj = { ip,ip } ;
    return inner_match(&obj) ;
}

static int cmp_ip_range(const void* a,const void* b)
{
    const ip_range_container::ip_range_t* ip_a = (const ip_range_container::ip_range_t*)a ;
    const ip_range_container::ip_range_t* ip_b = (const ip_range_container::ip_range_t*)b ;

    if ( ip_a->begin_ip > ip_b->end_ip) return 1 ;
    if ( ip_a->end_ip < ip_b->begin_ip) return -1 ;
    return 0 ;
}



bool ip_range_container::inner_match(const ip_range_t* obj) const
{
    void* result=bsearch(obj,m_ip_list.data(),m_ip_list.size(),sizeof(ip_range_t),cmp_ip_range) ;
    return  result !=NULL ;
}

void ip_range_container::resort()
{
    qsort(m_ip_list.data(),m_ip_list.size(),sizeof(ip_range_t),cmp_ip_range) ;
}



} /* namespace framework */
