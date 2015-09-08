/*
 * buffer.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include <stdlib.h>
#include <string.h>

#include "buffer.h"

namespace framework
{

buffer::buffer():m_begin(NULL),m_end(NULL),m_data(NULL),m_space(NULL)
{
    // TODO Auto-generated constructor stub

}

buffer::~buffer()
{
    fini() ;
}

int buffer::init(int size)
{
    if(size < 8 || m_begin != NULL ) return -1 ;
    m_begin = (char*)malloc(size) ;
    if(m_begin == NULL ) return -2 ;
    m_end = m_begin + size ;
    m_data = m_space = m_begin ;

    return 0 ;
}

void buffer::fini()
{
    if(m_begin != NULL )
    {
        free(m_begin) ;
        m_begin = m_end = m_data = m_space = NULL ;
    }
}

int buffer::resize(int size)
{

    if( m_space - m_begin > size ) return -1 ;

    char* new_buffer = (char*)realloc(m_begin,size) ;
    if(new_buffer == NULL ) return -2 ;
    if(new_buffer == m_begin )
    {
        m_end = m_begin + size ;
    }
    else
    {
        m_data = new_buffer + (m_data - m_begin) ;
        m_space = new_buffer + (m_space - m_begin) ;
        m_begin = new_buffer ;
        m_end = m_begin + size ;
    }

    return 0 ;

}

void buffer::adjust()
{
    if(m_begin != NULL && (m_data - m_begin ) > (m_end - m_begin)/2 )
    {
        int count = m_space - m_data ;
        memmove(m_begin,m_data,count ) ;
        m_data = m_begin ;
        m_space = m_begin + count ;
    }
}

}
