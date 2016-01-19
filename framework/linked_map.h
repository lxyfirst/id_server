
/**
 * linked_map.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <map>
#include <tr1/unordered_map>

namespace framework
{

/**
 * @brief hashmap with linked list
 */
template <typename K , typename V>
class linked_map
{
public:
    typedef K key_type ;
    typedef V value_type ;

    class node_type
    { 
    friend class linked_map ;
    public:
        value_type& value() { return m_value ;} ;
    private:
        node_type* m_prev ;
        node_type* m_next ;
        value_type m_value ;
    } ;

    typedef std::pair<key_type,node_type > map_value_type ;
    typedef std::tr1::unordered_map<key_type,node_type > node_container ;
    typedef typename node_container::iterator iterator ;
    typedef typename node_container::const_iterator const_iterator ;

public:
    linked_map()
    {
        m_head.m_next = m_head.m_prev = &m_head ;
    };

    iterator get_node(const key_type& key,bool touch=false)
    {
        iterator it = m_container.find(key) ;
        if(it != m_container.end() && touch )
        {
            list_node_remove(&it->second) ;
            list_node_insert(&m_head,&it->second,m_head.m_next) ;
        }

        return it ;
    }

    void erase_node(iterator it)
    {
        node_type& node = it->second ;
        list_node_remove(&node) ;
        m_container.erase(it) ;        
    }



    iterator begin() { return m_container.begin() ; } ;
    const_iterator begin() const { return m_container.begin() ; } ;

    int  size() const { return m_container.size(); } ;

    iterator end() { return m_container.end() ; } ;
    const_iterator end() const { return m_container.end() ; } ;
    
    value_type* get(const key_type& key,bool touch=false)
    {
        iterator it = get_node(key,touch) ;
        if(it!= m_container.end() ) return &it->second.m_value ;
        return NULL ;
    }

    value_type* insert(const key_type& key)
    {
        iterator it = m_container.find(key);
        if(it == m_container.end())
        {
            node_type& node = m_container[key] ;
            list_node_insert(&m_head,&node,m_head.m_next) ;
            return &node.m_value ;
        }
        
        return NULL ;
    }

    value_type* insert(const key_type& key,const value_type& value)
    {
        value_type* obj = insert(key) ;
        if(obj) *obj = value ;
        return obj ;
    }

    void erase(const key_type& key)
    {
        iterator it = m_container.find(key) ;
        if(it != m_container.end() ) erase_node(it);
        
    }

    void clear()
    {
        m_head.m_next = m_head.m_prev = &m_head ;
        m_container.clear() ;
    }
    
    value_type* tail()
    {
        if(&m_head == m_head.m_prev) return NULL ;
        return &m_head.m_prev->m_value ;
    }
    
    value_type* head()
    {
        if(&m_head == m_head.m_next) return NULL ;
        return &m_head.m_next->m_value ;    
    }

private:
    void list_node_insert(node_type* prev,node_type* curr,node_type* next)
    {
        prev->m_next = curr ;

        curr->m_prev = prev ;
        curr->m_next = next ;

        next->m_prev = curr ;
    }

    void list_node_remove(node_type* node)
    {
        node->m_prev->m_next = node->m_next ;
        node->m_next->m_prev = node->m_prev ;

        node->m_next = node->m_prev = NULL ;
    }

private:
    node_container m_container ;
    node_type m_head ;

} ;

}



