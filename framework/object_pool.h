/**
 * object_pool.h
 * Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <set>
#include <list>

namespace framework
{

template<typename T>
class object_pool
{
public:
    typedef std::set<T*> object_container ;
    typedef typename object_container::iterator iterator ;
    typedef std::list<T*> free_container ;

public:
    T* create()
    {
        T* obj = NULL ;
        if(m_free_container.size() >0)
        {
            obj = m_free_container.back() ;
            m_free_container.pop_back() ;
        }
        else
        {
            obj = new T ;
        }

        if(obj == NULL) return NULL ;
        m_object_container.insert(obj) ;
        return obj ;
    }

    void release(T* obj)
    {
        m_object_container.erase(obj) ;

        if( m_free_container.size()< m_cache_count)
        {

            m_free_container.push_back(obj) ;
        }
        else
        {
            delete obj ;
        }
    }

    void clear()
    {
        for(typename free_container::iterator it=m_free_container.begin();it!=m_free_container.end();++it)
        {
            delete *it ;
        }

        m_free_container.clear() ;

        for(iterator it=m_object_container.begin();it!=m_object_container.end();++it)
        {
            delete *it ;
        }

        m_object_container.clear() ;
    }

    object_pool(int cache_count = 16):m_cache_count(cache_count) {} ;

    ~object_pool()
    {
        clear() ;
    }

    iterator begin() { return m_object_container.begin(); } ;
    iterator end() { return m_object_container.end() ; } ;

private:
    object_pool(const object_pool& o) ;
    object_pool& operator=(const object_pool& o) ;

private:
    free_container m_free_container ;
    object_container m_object_container ;
    int m_cache_count ;
};

}


