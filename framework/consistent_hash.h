/**
 * consistent_hash.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <vector>
#include <map>

namespace framework
{


/**
 * @brief  consistent hash dispatcher
 */
class consistent_hash
{
public :

    consistent_hash(int mask= 0xFFFF,int vnode= 3):m_mask(mask),m_vnode(vnode){};
    /**
     * @brief load config
     */
    int set_node_list(const std::vector<int>& node_list)
    {
        if(node_list.size() < 1 || m_vnode < 1 || m_mask < 1 ) return -1 ;
        clear() ;

        for(auto& node_value : node_list) add_node(node_value);

        return 0 ;
    
    }

    void clear()
    {
        m_circle_nodes.clear() ;
    }

    int add_node(int node_value)
    {
        if(m_vnode < 1 || m_mask < 1 ) return -1 ;
        for(int i = 0 ; i < m_vnode ; ++i)
        {
            int circle_value = (((int64_t)node_value * 156899) + i*m_mask/m_vnode) % m_mask ;
            m_circle_nodes[circle_value] = node_value ;
        }
        return 0 ;

    }

    void dump()
    {
        for(auto& kv : m_circle_nodes) printf("%d => %d\n",kv.first,kv.second) ;
    }

    /**
     * @brief  
     */
    inline int dispatch(int key) 
    {
        if(m_circle_nodes.size() ==0) return -1 ;
        auto it = m_circle_nodes.lower_bound(key % m_mask) ;
        if( it == m_circle_nodes.end()) it = m_circle_nodes.begin() ;
        return it->second ;
    }


private:
    std::map<int,int> m_circle_nodes;
    int m_mask ;
    int m_vnode ;

};

}


