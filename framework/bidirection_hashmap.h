/**
 * bidirection_hashmap.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <tr1/unordered_map>


namespace framework
{

/**
 * @brief didirection hashmap 
 */
template<typename T1,typename T2>
class bidirection_hashmap
{
public:
    typedef std::pair<T1,T2> value_type ;
    typedef std::tr1::unordered_map<T1,value_type*> first_index ;
    typedef std::tr1::unordered_map<T2,value_type*> second_index ;

public:

    ~bidirection_hashmap()
    {
        for(typename first_index::iterator it = m_first_index.begin();it!=m_first_index.end();++it)
        {
            delete it->second ;
        }
    }

    bool insert(const T1& v1,const T2& v2)
    {
        if( m_first_index.count(v1) >0  || m_second_index.count(v2) > 0 )
        {
            return false ;
        }

        value_type* value = new value_type(v1,v2) ;
        if(value == NULL ) return false ;

        m_first_index[v1] = value ;
        m_second_index[v2] = value ;

        return true ;
    }

    void erase_by_first(const T1& v1)
    {
        typename first_index::iterator it = m_first_index.find(v1) ;
        if(it != m_first_index.end() )
        {
            value_type* value = it->second ;
            m_second_index.erase(value->second) ;
            m_first_index.erase(it) ;
            delete value ;
        }
    }

    void erase_by_second(const T2& v2)
    {
        typename second_index::iterator it = m_second_index.find(v2) ;
        if(it != m_second_index.end() )
        {
            value_type* value = it->second ;
            m_first_index.erase(value->second) ;
            m_second_index.erase(it) ;
            delete value ;
        }

    }

    const T2* find_by_first(const T1& v1) const
    {
        typename first_index::const_iterator it = m_first_index.find(v1) ;
        if(it != m_first_index.end() ) return &it->second->second ;
        return NULL ;
    }

    const T1* find_by_second(const T2& v2) const
    {

        typename second_index::const_iterator it = m_second_index.find(v2) ;
        if(it != m_second_index.end() ) return &it->second->first ;
        return NULL ;
    }

private:
    first_index  m_first_index ;
    second_index m_second_index ;

} ;


}

