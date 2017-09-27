/**
 * thread.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <stdint.h>

namespace framework
{

class simple_thread
{
public:
    enum
    {
        STATUS_STOPPED = 0 ,
        STATUS_RUNNING = 1 ,
    };

public:
    simple_thread():m_tid(0),m_status(STATUS_STOPPED) { } ;
    virtual ~simple_thread() { } ;
    /*
     * @brief create new thread and run
     * @return 0 on success
     */
    int start() ;

    /*
     * @brief stop thread
     */
    inline void stop() { m_status = STATUS_STOPPED ; };

    /*
     * @brief join thread
     */
    void join() ;

    /*
     * @brief thread id
     */
    int64_t id() const { return m_tid ; } ;


    inline bool is_run() { return m_status == STATUS_RUNNING ; };

    static simple_thread* current_thread() ;
protected:
    /*
     * @brief called before run loop
     * @return 0 on success
     */
    virtual int on_init() { return 0 ; } ;

    /*
     * @brief called after run loop
     */
    virtual void on_fini() {  } ;

    /*
     * @brief called every loop
     */
    virtual void run_once() = 0 ;

    /*
     * @brief  thread tag id , implemented by concrete class
     */
    virtual int tag_id()  { return 0 ; } ;
private:
    static void* thread_entry(void* arg) ;
private:
    int64_t m_tid ;
    volatile int m_status ;
};

}

