/**
 * buffer.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

namespace framework
{

/**
 * @brief data buffer which have independent read and write pointer ,
 * caller must keep data and pointer correct
 */
class buffer
{
public:
    buffer();
    ~buffer();

    /**
     * @brief initialize and alloc memory
     * @param [in] memory size
     * @return 0 on success , -1 on failed
     */
    int init(int size) ;

    /**
     * @brief free memroy
     */
    void fini() ;

    /**
     * @brief resize memory
     */
    int resize(int size) ;

    /**
     * @brief move data when read pointer > 1/4 total memory
     */
    void adjust() ;

    int capacity() { return m_end - m_begin ; } ;

    /**
     * @brief data pointer for read
     */
    char* data() { return m_data ; } ;
    int data_size() { return m_space - m_data ; } ;

    /**
     * @brief data ponter for write
     */
    char* space() { return m_space ; } ;
    int space_size() { return m_end - m_space ; } ;

    /**
     * @brief move write pointer after shift in data
     */
    int push_data(int count)
    {
        if(count < 1 || count > space_size() ) return -1 ;
        m_space += count ;
        return 0 ;
    }

    /**
     * @brief move read pointer after shift out data
     */
    int pop_data(int count)
    {
        if(count < 1 || count > data_size() ) return -1 ;
        m_data += count ;
        if(m_data == m_space )  m_data = m_space = m_begin ;
        return 0 ;
    }

    /**
     * @brief clean up
     */
    void clear() { m_data = m_space = m_begin ; } ;



private:
    buffer(const buffer&) ;
    buffer& operator=(const buffer&) ;

private:
    char* m_begin ;
    char* m_end ;
    char* m_data ;
    char* m_space ;
};

}

