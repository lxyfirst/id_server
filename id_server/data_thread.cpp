/*
 * data_thread.cpp
 * Author: lixingyi (lxyfirst@163.com)
 */

#include <map>

#include "framework/system_util.h"
#include "framework/member_function_bind.h"
#include "framework/packet.h"
#include "data_thread.h"
#include "counter_manager.h"

using namespace framework ;





DataThread::DataThread(log_thread& logger,const ThreadConfig& config,int pipe_fd):
        m_logger(logger),m_config(config),m_pipe_fd(pipe_fd),m_now(0)
{
    // TODO Auto-generated constructor stub

}

DataThread::~DataThread()
{
    // TODO Auto-generated destructor stub
}

int DataThread::on_init()
{
    if(m_reactor.init(2)!=0) error_return(-1,"init reactor failed") ;

    pipe_handler::callback_type callback = member_function_bind(&DataThread::on_pipe_message,this) ;
    if(m_handler.init(m_reactor,m_pipe_fd,callback )!=0 )
    {
        error_return(-1,"init pipe failed") ;
    }

    //set_thread_title("data_thread");


    return 0;
}

void DataThread::on_fini()
{
    run_once();

    m_db.fini();

    close(m_pipe_fd) ;

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

    m_reactor.run_once(5000) ;

}

int  DataThread::send_response(const packet_info* msg,const char* data)
{
    packet_info response = *msg ;
    response.data =  data ;

    return m_handler.send_pipe_message(&response) ;
}

void DataThread::on_pipe_message(const packet_info* msg)
{
    switch(msg->type)
    {
    case DB_LOAD_COUNTER:
        on_load_counter(msg) ;
        break ;
    case DB_SQL_DELETE:
    case DB_SQL_INSERT:
    case DB_SQL_UPDATE:
        on_sql_update(msg) ;
        break ;


    }

}

void DataThread::on_load_counter(const packet_info* msg)
{
    /*
    const char* username = msg->data ;
    trace_log_format(m_logger,"load user:%s",username) ;
    if(m_db.exec_format("select uid,username,nickname,password,avatar from account where username='%s'",username)!=0  )
    {
        //send_response(msg,NULL) ;
        delete[] username ;
        return ;
    }

    delete[] username ;

    if(m_db.result_row_count() <1 )
    {
        send_response(msg,NULL) ;
        return  ;
    }

    CounterInfo* user_info = new UserInfo ;
    if(user_info)
    {
        user_info->rule_name.assign(m_db.result_row_data(0)[1]) ;
        user_info->app_name.assign(m_db.result_row_data(0)[2]) ;
        user_info->node = atoi(m_db.result_row_data(0)[3]) ;
        user_info->counter = atoi(m_db.result_row_data(0)[4]) ;
    }

    if(send_response(msg,(const char*)user_info )!=0 )
    {
        info_log_format(m_logger,"pipe response failed fsm_id:%d\n",msg->size ) ;

        delete user_info ;
    }
    */


}


void DataThread::on_sql_update(const packet_info* msg)
{
    debug_log_format(m_logger,"exec sql:%s",msg->data) ;

    if(m_db.exec(msg->data) <0 )
    {
        warn_log_format(m_logger,"db failed thread:%ld errno:%d sql:%s\n",
                id(), m_db.get_errno(), msg->data) ;
    }

    delete[] msg->data ;
}





