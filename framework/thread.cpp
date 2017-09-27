/*
 * thread.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include <pthread.h>
#include <signal.h>

#include "thread.h"


namespace framework
{

int simple_thread::start()
{
    if ( m_tid != 0 ) return -1 ;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 8 << 20 );

    sigset_t sigmask ;
    sigfillset(&sigmask) ;
    pthread_sigmask(SIG_BLOCK,&sigmask,NULL) ;
    int ret = pthread_create((pthread_t*)&m_tid, &attr, thread_entry, this);
    pthread_sigmask(SIG_UNBLOCK,&sigmask,NULL) ;

    pthread_attr_destroy(&attr) ;

    return ret ;
}



void simple_thread::join()
{
    if ( m_tid != 0 )
    {

        pthread_join(m_tid,NULL) ;
        m_tid = 0 ;
    }

}

static thread_local simple_thread* cur_thread = NULL ;
void* simple_thread::thread_entry(void* arg)
{
    cur_thread = (simple_thread*)arg;

    cur_thread->m_status = STATUS_RUNNING ;
    if( cur_thread->on_init() != 0 )
    {
        return NULL;
    }

    while(cur_thread->m_status == STATUS_RUNNING)
    {
        cur_thread->run_once() ;
    }

    cur_thread->on_fini() ;

    return NULL ;
}

simple_thread* simple_thread::current_thread()
{
    return cur_thread ;
}



}
