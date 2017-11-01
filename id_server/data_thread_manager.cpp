/**
 * data_thread_manager.cpp
 * Author: lixingyi (lxyfirst@163.com)
 */

#include <string.h>

#include "framework/system_util.h"
#include "framework/member_function_bind.h"
#include "data_thread_manager.h"

#include "id_server.h"

using namespace framework ;


int load_counter_data(CounterManager& counter_manager,int node_offset,const ThreadConfig& config)
{
    MysqlConnection db ;
    db.init(config.host.c_str(),config.user.c_str(),config.password.c_str(), config.port ) ;
    db.set_charset(config.charset.c_str()) ;
    db.use(config.dbname.c_str());

    if(db.exec_format("select rule_name,app_name,counter,update_time from counter where node_offset=%d",
        node_offset ) < 0 )
    {
        return -1 ;
    }

    CounterData data ;
    for(int i = 0 ; i < db.result_row_count() ; ++i)
    {
        const char** row = db.result_row_data(i) ;
        data.rule_name.assign(row[0]) ;
        data.app_name.assign(row[1]) ;
        data.node_offset = node_offset ;
        data.counter = atoi(row[2]) ;
        data.update_time = atoi(row[3]) ;
        data.saved_counter = data.counter ;
        counter_manager.load_counter(data) ;

    }

    db.fini() ;

    return 0;

}


DataThreadManager::DataThreadManager()
{
    // TODO Auto-generated constructor stub

}

DataThreadManager::~DataThreadManager()
{
    fini() ;
}

int DataThreadManager::init(int thread_count,const ThreadConfig& thread_config,log_thread& logger)
{
    if(thread_count < 1 || thread_count > 1024 ) return -1 ;
    if(m_thread_list.size() > 0 ) return -2 ;

    m_config  = thread_config ;
    m_thread_count = thread_count ;
    m_thread_list.resize(thread_count) ;

    for(int i = 0 ; i < thread_count ; ++i)
    {
        DataThread* thread = new DataThread(logger,m_config) ;
        if(thread== NULL) error_return(-1,"alloc failed") ;

        if(thread->start()!=0)
        {
            delete thread ;
            error_return(-1,"create thread failed");
        }
        m_thread_list[i] = thread ;
        usleep(1000) ;
    }



    return 0 ;
}

void DataThreadManager::fini()
{

    for(ThreadContainer::iterator it = m_thread_list.begin();it!=m_thread_list.end();++it)
    {
        DataThread* thread = *it ;
        if(thread) thread->stop() ;
    }

    for(ThreadContainer::iterator it = m_thread_list.begin();it!=m_thread_list.end();++it)
    {
        DataThread* thread = *it ;
        if(thread)
        {
            thread->join() ;
            delete thread ;
        }
    }


    m_thread_list.clear() ;

}

static int hash(const char* data,int size)
{
    int total = 0 ;
    for(int i=0;i< size ; ++i)
    {
        total += data[i] ;
    }

    return total & 0xFFFFFFF ;
}



int DataThreadManager::async_save(const CounterData& data)
{
    int index = hash(data.app_name.c_str(),data.app_name.size() ) % m_thread_list.size() ;
    DataThread* thread  = m_thread_list[index] ;
    if( thread->async_save(data)!=0)
    {
        warn_log_format(get_app().logger(),"save failed rule_name:%s app_name:%s counter:%d",
            data.rule_name.c_str(),data.app_name.c_str(),data.saved_counter ) ;
        return -2 ;
    }

    return 0 ;

}


