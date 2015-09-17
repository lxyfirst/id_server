
/*
 *
 * Author : lixingyi (lxyfirst@163.com)
 */

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "mmap_file.h" 


namespace framework
{

void* mmap_append_file::add_mmap_segment(int fd,int64_t offset,int64_t size)
{
    if( ftruncate(fd,offset + size )!=0) return NULL ;
    void* file_addr = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,offset) ;
    if(file_addr == MAP_FAILED ) return NULL ;
    return file_addr ;
}

int64_t mmap_append_file::calc_align_size(int64_t size)
{

    return  size - size % MMAP_SEGMENT_SIZE ;
}


mmap_append_file::~mmap_append_file()
{
    fini() ;
}

int mmap_append_file::init(const char* file_name,int64_t max_size)
{
    if(file_name == NULL  || max_size < MMAP_SEGMENT_SIZE ) return -1 ;
    int fd = open(file_name,O_CREAT|O_RDWR,0755) ;
    if(fd < 1 ) return -2 ;
    struct stat st = {0};
    if( fstat(fd, &st)!=0)  
    {
        close(fd) ;
        return -1 ;
    }
   
    max_size = calc_align_size(max_size) ;
    if( st.st_size > max_size)
    {
        close(fd) ;
        return -1 ;
    }
    
    int64_t align_size = calc_align_size(st.st_size) ;

    void* file_addr = add_mmap_segment(fd,align_size,MMAP_SEGMENT_SIZE) ;
    if(file_addr == NULL )
    {
        close(fd) ;
        return -1 ;
    }
    
    m_fd = fd ;
    m_mapped_offset = align_size ;
    m_max_size = max_size ;
    m_cur_pos = st.st_size % MMAP_SEGMENT_SIZE ;
    m_file_addr = file_addr ;
    m_flush_pos = m_cur_pos ;
    return 0 ;

    
}

void mmap_append_file::fini()
{
    if(m_file_addr) munmap(m_file_addr,MMAP_SEGMENT_SIZE) ;
    if(m_fd >0) 
    {
        ftruncate(m_fd,m_mapped_offset + m_cur_pos) ;
        close(m_fd) ;
    }

    m_fd = -1 ;
    m_file_addr = NULL ;
} 

int mmap_append_file::flush()
{
    if(m_file_addr == NULL ) return -1 ;

    int ret = msync((char*)m_file_addr + m_flush_pos,m_cur_pos - m_flush_pos,MS_ASYNC) ;
    if(ret == 0 ) m_flush_pos = m_cur_pos ;

    return ret ;
}

int mmap_append_file::write(const char* data,int size)
{
    if( size < 1 || size > MMAP_SEGMENT_SIZE || data == NULL ) return -1 ;
    if(m_file_addr == NULL ) return -2 ;

    int overflow_size = m_cur_pos + size - MMAP_SEGMENT_SIZE ;
    if(overflow_size >0) //overflow
    {
        if( writable_size() < size ) return -3 ;

        m_mapped_offset += MMAP_SEGMENT_SIZE ;
        void* new_file_addr = add_mmap_segment(m_fd,m_mapped_offset,MMAP_SEGMENT_SIZE) ;
        if(new_file_addr == NULL ) return -1 ;

        memcpy((char*)m_file_addr + m_cur_pos , data, MMAP_SEGMENT_SIZE - m_cur_pos) ;
        memcpy((char*)new_file_addr , data + size - overflow_size  , overflow_size) ;

        munmap(m_file_addr,MMAP_SEGMENT_SIZE) ;
        m_file_addr = new_file_addr ;
        m_cur_pos = overflow_size ;
        m_flush_pos = 0 ;
        
    }
    else
    {
        memcpy((char*)m_file_addr + m_cur_pos,data,size) ;
        m_cur_pos += size ;
    }

    return 0 ;
} 

mmap_file::~mmap_file()
{
    if(m_file_addr)
    {
        munmap(m_file_addr,m_size) ;
    }
}

int mmap_file::load_file(const char* file_name)
{
    int fd = open(file_name,O_RDWR,0) ;
    if(fd < 1 ) return -2 ;
    struct stat st = {0};
    fstat(fd, &st) ;
    if(st.st_size <1)
    {
        close(fd) ;
        return -1 ;
    }


    void* file_addr = mmap(NULL,st.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0) ;
    close(fd) ;

    if(file_addr == MAP_FAILED ) return -1 ;

    m_file_addr = file_addr ;
    m_size = st.st_size ;
    m_time = st.st_mtime ;

    return 0 ;
}

}

