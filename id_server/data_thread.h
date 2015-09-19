/*
 * data_thread.h
 * Author: lixingyi (lxyfirst@163.com)
 */

#ifndef DATA_THREAD_H_
#define DATA_THREAD_H_

#include <string>
#include <tr1/unordered_map>

#include "framework/poll_reactor.h"
#include "framework/thread.h"
#include "framework/eventfd_handler.h"
#include "framework/circular_queue.h"
#include "framework/log_thread.h"
#include "mysql_connection.h"

#include "counter_manager.h"

struct ThreadConfig
{
    std::string host ;
    std::string user ;
    std::string password ;
    std::string dbname ;
    std::string charset ;
    int port ;
    int queue_size ;
}   ;

class DataThread: public framework::simple_thread
{
public:
    DataThread(framework::log_thread& logger,const ThreadConfig& config);
    virtual ~DataThread();

    /*
     *@brief  called by main thread
     *@return 0 on success , else return -1
     */
    int async_save(const CounterData& data) ;


    void on_event(int64_t v) ;
protected:
    virtual int on_init() ;
    virtual void on_fini() ;
    virtual void run_once() ;

    void on_timeout() ;

private:
    framework::log_thread& m_logger ;
    const ThreadConfig& m_config ;
    framework::poll_reactor m_reactor ;
    framework::eventfd_handler m_handler ;
    framework::circular_queue<CounterData> m_queue ;
    MysqlConnection m_db ;
    int m_now ;

};

#endif /* DATA_THREAD_H_ */
