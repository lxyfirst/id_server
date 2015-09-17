/*
 * data_thread_manager.h
 * Author: lixingyi (lxyfirst@163.com)
 */

#ifndef DATA_THREAD_MANAGER_H_
#define DATA_THREAD_MANAGER_H_

#include <vector>
#include <string>

#include "framework/pipe_handler.h"
#include "data_thread.h"
#include "counter_manager.h"

int load_counter_data(CounterManager& counter_manager,int node_offset,const ThreadConfig& config) ;

class DataThreadManager
{
public:
    typedef std::vector<DataThread*> ThreadContainer ;
    typedef std::vector<framework::pipe_handler*> HandlerContainer ;
public:
    DataThreadManager();
    virtual ~DataThreadManager();

    int init(int thread_count,const ThreadConfig& thread_config,framework::log_thread& logger) ;

    void fini() ;

    int async_update(const CounterData& data);

    void on_pipe_message(const framework::packet_info* msg);

    void on_sql_update_response(const framework::packet_info* msg) ;

private:
    DataThread* get_thread_by_key(int32_t key)
    {
        return m_thread_list[key % m_thread_list.size()] ;
    };

    DataThread* get_thread_by_index(int index)
    {
        if(index < 0 || index >= m_thread_list.size()) return NULL ;
        return m_thread_list[index] ;
    };

private:
    ThreadConfig m_config ;
    ThreadContainer m_thread_list ;
    HandlerContainer m_handler_list ;
    int m_thread_count ;
};

#endif /* DATA_THREAD_MANAGER_H_ */
