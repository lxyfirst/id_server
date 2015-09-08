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
#include "framework/pipe_handler.h"
#include "framework/log_thread.h"
#include "mysql_connection.h"


struct ThreadConfig
{

    std::string host ;
    std::string user ;
    std::string password ;
    std::string dbname ;
    std::string charset ;
    int port ;
    int timeout ;
}   ;

enum
{
	DB_LOAD_COUNTER = 1 ,
	DB_SQL_UPDATE = 3 ,
	DB_SQL_INSERT = 4 ,
	DB_SQL_DELETE = 5 ,

};

class DataThread: public framework::simple_thread
{
public:
	DataThread(framework::log_thread& logger,const ThreadConfig& config,int pipe_fd);
	virtual ~DataThread();



	void on_pipe_message(const framework::packet_info* msg);
protected:
    virtual int on_init() ;
    virtual void on_fini() ;
    virtual void run_once() ;

    void on_timeout() ;

    int send_response(const framework::packet_info* msg,const char* data);

    void on_sql_update(const framework::packet_info* msg);
    void on_load_counter(const framework::packet_info* msg);


private:
	framework::log_thread& m_logger ;
	const ThreadConfig& m_config ;
	framework::poll_reactor m_reactor ;
	framework::pipe_handler m_handler ;
	MysqlConnection m_db ;
	int m_pipe_fd ;
	int m_now ;

};

#endif /* DATA_THREAD_H_ */
