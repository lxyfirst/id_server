/*
 * data_thread_manager.cpp
 * Author: lixingyi (lxyfirst@163.com)
 */

#include <string.h>

#include "framework/system_util.h"
#include "framework/member_function_bind.h"
#include "framework/pipe_handler.h"
#include "data_thread_manager.h"

#include "id_server.h"

using namespace framework ;


int load_counter_data(CounterManager& counter_manager,int node_offset,const ThreadConfig& config)
{
    MysqlConnection db ;
    db.init(config.host.c_str(),config.user.c_str(),config.password.c_str(), config.port ) ;

    db.use(config.dbname.c_str());

    if(db.exec_format("select rule_name,app_name,counter,update_time from counter where node_offset=%d",node_offset) < 0 )
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
    m_handler_list.resize(thread_count) ;

    for(int i = 0 ; i < thread_count ; ++i)
    {
        int pipe_fd[2] = {0} ;
        if(socketpair(AF_UNIX,SOCK_SEQPACKET,0,pipe_fd)!=0)
        {
            error_return(-1,"create pipe failed") ;
        }

        pipe_handler* front_handler = new pipe_handler ;
        if(front_handler==NULL) error_return(-1,"alloc failed") ;
        pipe_handler::callback_type callback = member_function_bind(&DataThreadManager::on_pipe_message,this) ;
        if(front_handler->init(get_app().reactor(),pipe_fd[1],callback )!=0 )
        {
            delete front_handler ;
            error_return(-1,"init pipe failed") ;
        }
        m_handler_list[i] = front_handler ;

        DataThread* thread = new DataThread(logger,m_config,pipe_fd[0]) ;
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

    for(HandlerContainer::iterator it = m_handler_list.begin();it!=m_handler_list.end();++it)
    {
        pipe_handler* front_handler = *it ;

        if(front_handler) delete front_handler ;
    }

    m_handler_list.clear() ;

    for(ThreadContainer::iterator it = m_thread_list.begin();it!=m_thread_list.end();++it)
    {
        DataThread* thread = *it ;
        if(thread)
        {
            thread->stop() ;
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








int DataThreadManager::async_update(const CounterData& data)
{
    enum { MAX_SQL_SIZE = 2000 } ;

    char* sql_buf = new char[MAX_SQL_SIZE] ;
    snprintf(sql_buf,MAX_SQL_SIZE,
            "replace into counter set rule_name='%s',app_name='%s',node_offset=%d,counter=%d,update_time=%d",
            data.rule_name.c_str(),data.app_name.c_str(),data.node_offset,data.saved_counter,data.update_time ) ;

    int index = hash(data.app_name.c_str(),data.app_name.size() ) % m_handler_list.size() ;
    pipe_handler* front_handler = m_handler_list[index] ;
    packet_info pi  ;
    pi.type = DB_SQL_UPDATE ;
    pi.size = 0 ;
    pi.data = sql_buf ;
    if( front_handler->send_pipe_message(&pi)!=0)
    {
        warn_log_format(get_app().logger(),"send pipe failed sql:%s",sql_buf) ;
        delete[] sql_buf ;
        return -2 ;
    }

    return 0 ;

}


void DataThreadManager::on_pipe_message(const framework::packet_info* msg)
{
    switch(msg->type)
    {
    case DB_SQL_UPDATE:
        on_sql_update_response(msg) ;
        break ;

    }


}



void DataThreadManager::on_sql_update_response(const packet_info* msg)
{

}
