/**
 * circular_queue.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once
#include <cstdlib>
#include <new>

namespace framework
{
/**
 * @brief spsc circular queue , push elements at the head and pop elements at the tail
 */
template<typename T>
class circular_queue
{
public:
    /**
     * @brief initialize the queue , allocate space
     */
    int init(int maxsize)
    {
        if ( m_itemlist != NULL || maxsize < 4 ) return -1 ;

        m_itemlist = new T[maxsize] ;
        if ( m_itemlist == NULL ) return -1 ;

        m_maxsize = maxsize ;
        return 0 ;
    } ;

    /**
     * @brief clean up
     */
    void fini()
    {
        if ( m_itemlist != NULL )
        {
            delete[] m_itemlist ;
            m_itemlist = NULL ;
            m_front = m_back = 0 ;

        }

    } ;

    /**
     * @brief get the queue capacity size
     */
    int capacity() const { return m_maxsize ;} ;
    /**
     * @brief check the queue is empty
     * @return true if empty
     */
    bool empty() const { return m_front == m_back ;} ;
    /**
     * @brief check the queue is full
     * @return true if full
     */
    bool full() const {return (m_front +1) % m_maxsize == m_back ; } ;

    /**
     * @brief push element at the head
     */
    int push(const T& element)
    {
        if ( m_itemlist == NULL || full() ) return -1 ;
        m_itemlist[m_front] = element ;
        m_front = (m_front +1) % m_maxsize ;
        return 0 ;
    }

    /**
     * @brief pop element at the tail
     */
    int pop(T& element)
    {
        if ( m_itemlist == NULL || empty() ) return -1 ;
        element = m_itemlist[m_back]  ;
        m_back = (m_back +1) % m_maxsize ;
        return 0 ;
    }


    /**
     * @brief get the tail element pointer
     */
    T* back()
    {
        if (m_itemlist == NULL || empty() ) return NULL ;
        return m_itemlist + m_back ;
    } ;


public:
    circular_queue():m_maxsize(1),m_itemlist(NULL),m_front(0),m_back(0){};
    ~circular_queue() { fini(); } ;

private:
    circular_queue(const circular_queue&) ;
    circular_queue& operator=(const circular_queue&) ;

private:
    int m_maxsize ;
    T* m_itemlist ;
    volatile int m_front ;
    volatile int m_back ;

};

}

