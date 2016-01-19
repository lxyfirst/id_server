/**
 * data_thread_manager.h
 * Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <vector>
#include <string>

#include "data_thread.h"
#include "counter_manager.h"

int load_counter_data(CounterManager& counter_manager,int node_offset,const ThreadConfig& config) ;

/**
 * @brief data thread manager
 */
class DataThreadManager
{
public:
    typedef std::vector<DataThread*> ThreadContainer ;
public:
    DataThreadManager();
    virtual ~DataThreadManager();

    int init(int thread_count,const ThreadConfig& thread_config,framework::log_thread& logger) ;

    void fini() ;

    /**
     * @brief save data to database
     */
    int async_save(const CounterData& data);

private:
    ThreadConfig m_config ;
    ThreadContainer m_thread_list ;
    int m_thread_count ;
};

