/**
 * data_thread.cpp
 * Author: lixingyi (lxyfirst@163.com)
 */

#include <map>

#include "framework/system_util.h"
#include "framework/member_function_bind.h"
#include "data_thread.h"
#include "counter_manager.h"

using namespace framework ;


DataThread::DataThread(log_thread& logger,const ThreadConfig& config):
        m_logger(logger),m_config(config),m_now(0)
{

}

DataThread::~DataThread()
{
}

int DataThread::on_init()
{
    if(m_reactor.init(4)!=0) error_return(-1,"init reactor failed") ;
    if(m_queue.init(m_config.queue_size)!=0) error_return(-1,"init queue failed") ;
    eventfd_handler::callback_type callback = member_function_bind(&DataThread::on_event,this) ;
    if(m_handler.init(m_reactor,callback )!=0 )
    {
        error_return(-1,"init eventfd failed") ;
    }

    set_thread_title("data_thread");

    return 0;
}

void DataThread::on_fini()
{
    on_event(1) ;

    m_db.fini();

    m_handler.fini() ;

    m_reactor.fini() ;

}

void DataThread::on_timeout()
{
    if(m_db.ping()!=0)
    {
        m_db.fini() ;

        info_log_format(m_logger,"connect to db thread:%ld host:%s port:%d\n",
                id(),m_config.host.c_str(), m_config.port) ;

        m_db.init(m_config.host.c_str(), m_config.user.c_str(),
                m_config.password.c_str(), m_config.port ) ;
        m_db.set_charset(m_config.charset.c_str()) ;
        m_db.use(m_config.dbname.c_str());
    }
}

void DataThread::run_once()
{


    int now = time(0) ;
    if (now - m_now >= 60)
    {
        m_now = now;
        on_timeout();
    }

    m_reactor.run_once(2000) ;

}


int DataThread::async_save(const CounterData& data)
{
    if( m_queue.push(data) !=0 ) return -1;
    m_handler.notify() ;
    return 0 ;
}


void DataThread::on_event(int64_t v)
{
    char  sql[1024]  ;
    sql[sizeof(sql)-1] = '\0' ;

    CounterData data ;
    while( m_queue.pop(data) == 0 )
    {
        snprintf(sql,sizeof(sql)-1,
            "replace into counter set rule_name='%s',app_name='%s',node_offset=%d,counter=%d,update_time=%d",
            data.rule_name.c_str(),data.app_name.c_str(),data.node_offset,data.saved_counter,data.update_time ) ;

        debug_log_format(m_logger,"exec sql:%s",sql) ;
        if(m_db.exec(sql) <0 )
        {
            warn_log_format(m_logger,"db failed thread:%ld errno:%d sql:%s\n",
                id(), m_db.get_errno(), sql) ;
        }

    }

}

