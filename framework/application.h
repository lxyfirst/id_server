/**
 * application.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <signal.h>
#include <stdio.h>

#include "epoll_reactor.h"
#include "timer_manager.h"
#include "time_util.h"

#ifndef APP_VERSION
#define APP_VERSION "unknown"
#endif

namespace framework
{

/**
 * application framework
 */
class application
{
friend class base_timer ;
public:
    enum
    {
        STATUS_STOP = 0 ,
        STATUS_RUN = 1 ,
        STATUS_RELOAD = 2 ,
        STATUS_PRE_STOP = 3 ,
    };

    enum
    {
        MS_PER_CLOCK = 100,
        MIN_INTERVAL = MS_PER_CLOCK,
        DEFAULT_INTERVAL = 600000,
    } ;


public:
    application() ;
    virtual ~application() ;

public:
    /**
     * @brief program entry
     */
    int start(int argc,char** argv) ;

    /**
     * stop application
     */
    void stop() ;

    //int8_t get_status() const{ return m_status ; } ;
    bool is_running() const { return m_status == STATUS_RUN ; } ;
    bool is_stopped() const { return m_status == STATUS_STOP; } ;

    /**
     * @brief  send signal to application , applicaton::on_signal will be called
     */
    void send_signal(int signo) ;
    

    int add_timer_after(base_timer* timer,int duration_ms) ;
    int add_timer_until(base_timer* timer,int64_t expired_ms) ;

    void del_timer(base_timer* timer) ;

    base_reactor& reactor() { return m_event_engine ; } ;

    const char* config_file() const { return m_config_file ; } ;
    const char* work_path() const { return m_work_path ; } ;
    int max_open_fd() const { return m_max_open_fd ; } ;

protected:
    /**
     * @brief get version string , implemented derived class
     */
    virtual const char* version() { return "version " APP_VERSION " compiled at " __TIME__ " "  __DATE__ ; } ;


    /**
     * @brief called  when parse args , call parse_option() default
     * @return 0 on success , if failed ,application will exit
     */
    virtual int on_parse_args(int argc,char** argv) ;

    /**
     * @brief called  when initialized , implemented derived class
     * @return 0 on success , if failed ,application will exit
     */
    virtual int on_init() {return 0; };


    /**
     * @brief called when get USR1 signal , implemented derived class
     * @return 0 on success , if failed , application will exit
     */
    virtual int on_reload() { return 0 ; } ;


    /**
     * @brief called when quit , implemented derived class
     */
    virtual void on_fini() { };

    /**
     * @brief called when get signal , should return immediately
     */
    virtual void on_signal(int signo) ;


    /**
     * @brief called when app timer triggered , implemented derived class
     */
    virtual void on_timer() { } ;

    /**
     * @brief called when delay stop timer triggered , implemented derived class
     */
    virtual void on_delay_stop() { } ;

protected:
    //int64_t get_run_ms() const { return rdtsc()/m_tick_ms ; } ;
    int64_t get_run_ms() const { return time_ms() ; } ;

    int set_app_timer(int interval_ms) ;
    int set_delay_stop_timer(int interval_ms);

    void set_status(int8_t status) ;

private:

    int parse_option(int argc,char** argv) ;

    void change_work_directory(const char* argv0) ;

    void show_help(const char* argv0) ;

    void show_version(const char* argv0)  ;

    void on_timeout(timer_manager* manager) ;
    void on_delay_timeout(timer_manager* manager) ;

    int64_t ms_to_timer_clock(int64_t ms) { return ms /MS_PER_CLOCK; } ;
    int64_t timer_clock_to_ms(int64_t clock) { return clock *MS_PER_CLOCK; } ;

    application(const application& app) ;
    application& operator=(const application& app) ;

protected:
    epoll_reactor m_event_engine ;
    timer_manager m_timer_engine ;
    const char* m_config_file ;
    const char* m_work_path ;
    const int32_t m_max_open_fd ;

private:
    int32_t m_interval_ms ; 
    int32_t m_delay_ms ;
    int8_t m_daemon ;
    int8_t m_lockfile ;
    int8_t m_sig_status ;
    volatile int8_t m_status ;

    base_timer m_timer ;
    base_timer m_delay_timer ;


};

template<typename T> T& singleton()
{
    static T app ;
    return app ;
}


}

#define IMPLEMENT_MAIN(app_instance)      \
    static void sig_handler(int signo){app_instance.send_signal(signo);}      \
    int main(int argc,char** argv){                 \
        signal(SIGINT,sig_handler) ;               \
        signal(SIGTERM,sig_handler) ;              \
        signal(SIGQUIT,sig_handler) ;              \
        signal(SIGUSR2,sig_handler) ;              \
        signal(SIGUSR1,sig_handler) ;            \
        signal(SIGPIPE, SIG_IGN);         \
        signal(SIGALRM,SIG_IGN);          \
        signal(SIGHUP,SIG_IGN);           \
        return app_instance.start(argc,argv);}

