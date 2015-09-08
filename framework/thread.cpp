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

int thread::start()
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



void thread::join()
{
    if ( m_tid != 0 )
    {

        pthread_join(m_tid,NULL) ;
        m_tid = 0 ;
    }

}

void* thread::thread_entry (void* arg)
{
    thread* cur_thread = (thread*)arg;
    cur_thread->run() ;
    return NULL ;
}


void simple_thread::run()
{
    m_status = 1 ;
    if( on_init() != 0 ) return ;
    while(m_status == 1) run_once() ;

    on_fini() ;
}


}
