/**
 * mmap_file.h
 * Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <stdint.h>

namespace framework
{

class mmap_append_file
{
public:
    enum { MMAP_SEGMENT_SIZE = 0x1 << 24 } ;


    static void* add_mmap_segment(int fd,int64_t offset,int64_t size) ;

    static int64_t calc_align_size(int64_t size) ;

public:
    mmap_append_file():m_fd(-1),m_cur_pos(0),m_mapped_offset(0),
        m_max_size(0),m_file_addr(NULL) ,m_flush_pos(0)
    {
    };

    ~mmap_append_file() ;

     
    int init(const char* file_name,int64_t max_size)  ;
        
    void fini() ;

    int flush() ;
   
    int write(const char* data,int size) ;

    int64_t writable_size()  const 
    {
        return m_max_size - m_mapped_offset - m_cur_pos ; 
    } ;

    int64_t total_size() const 
    {
        return m_max_size; 
    } ;
        
private:
    int m_fd ;        // file  descripter
    int m_cur_pos ;   // memory offset of current segment
    int64_t m_mapped_offset ;    // file mmap offset
    int64_t m_max_size ;    // file max size
    void* m_file_addr ;
    int m_flush_pos ;
} ;

class mmap_file
{
public:
    mmap_file():m_file_addr(NULL),m_size(0) { } ;
    ~mmap_file() ;

    int load_file(const char* filename) ;

    int file_size() const { return m_size ; } ;
    int file_time() const { return m_time ; } ;
    void* file_data() { return m_file_addr ; } ;

private:
    void* m_file_addr ;
    int m_size ;
    int m_time ;
};

}


