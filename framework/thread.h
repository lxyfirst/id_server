/**
 * thread.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <stdint.h>

namespace framework
{

class thread
{
public:
    thread():m_tid(0) { } ;
    virtual ~thread() { } ;

public:
    /*
     * @brief create new thread and run
     * @return 0 on success
     */
    int start() ;

    /*
     * @brief join thread
     */
    void join() ;

    /*
     * @brief thread id
     */
    int64_t id() const { return m_tid ; } ;
protected:
    /*
     * @brief new thread callback , implemented by concrete class
     */
    virtual void run() = 0 ;

private:
    thread(const thread& o) ;
    thread& operator=(const thread& o) ;

private:
    static void* thread_entry(void* arg) ;

private:
    int64_t m_tid ;
};

class simple_thread : public thread
{
public:
    simple_thread():m_status(0) { } ;
    virtual ~simple_thread() { } ;

    /*
     * @brief stop thread
     */
    inline void stop() { m_status = 0 ; };

    inline bool is_run() { return m_status == 1 ; };
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

private:
    void run() ;
private:
    volatile int m_status ;
};

}

