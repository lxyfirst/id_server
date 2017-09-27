/*
 * application.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>

#include "application.h"
#include "system_util.h"

namespace framework
{
application::application():
        m_config_file(NULL),
        m_work_path(NULL),
        m_max_open_fd(get_open_file_limit()),
        m_interval_ms(DEFAULT_INTERVAL),
        m_delay_ms(0),
        m_daemon(0),
        m_lockfile(0),
        m_sig_status(STATUS_STOP),
        m_status(STATUS_STOP)
{
    m_timer.set_callback(this,&application::on_timeout) ;
    m_delay_timer.set_callback(this,&application::on_delay_timeout) ;
}


application::~application()
{

}


void application::show_help(const char* argv0)
{
    printf("usage:%s -hvd  [-c filename] [-w path]\n",argv0) ;
    printf("options:\n"
            "-h          :show help\n"
            "-v          :show version\n"
            "-d          :run as daemon\n"
            "-c filename :set config file\n"
            "-l          :lock config file\n"
            "-w path     :set work directory\n"
            ) ;

}


void application::show_version(const char* argv0) 
{
    printf("%s\n",version());
} 

int application::parse_option(int argc,char** argv)
{

    for(int i=1 ; i < argc ; ++i)
    {
        const char* p = argv[i] ;
        if (*p++ != '-') error_return(-1,"invalid option:%s",argv[i]) ;
        switch(*p++)
        {
        case '\0':
            continue ;
        case 'h':
            show_help(argv[0]) ;
            exit(0) ;
            break ;
        case 'v':
            show_version(argv[0]) ;
            exit(0) ;
            break ;
        case 'd':
            m_daemon = 1 ;
            break ;
        case 'l':
            m_lockfile = 1 ;
            break ;
        case 'c':
            if(*p) m_config_file = (const char*)p ;
            else if (argv[++i]) m_config_file = argv[i] ;
            else error_return(-1,"option -c require parameter") ;

            break ;
        case 'w':
            if(*p) m_work_path = (const char*)p ;
            else if (argv[++i]) m_work_path = argv[i] ;
            else error_return(-1,"option -w require parameter") ;

            break ;
            
        default:
            error_return(-1,"invalid option:%s",argv[i]) ;
        }

    }

    if(m_config_file == NULL ) error_return(-1,"require config file") ;

    return 0 ;
}


void application::on_timeout(timer_manager* manager)
{
    on_timer() ;
    add_timer_after(&m_timer,m_interval_ms) ;

}

void application::send_signal(int signo)
{
    on_signal(signo) ;
}

int application::set_app_timer(int interval_ms)
{
    if(interval_ms < MIN_INTERVAL || interval_ms > DEFAULT_INTERVAL*10 )
    {
        return -1 ;
    }

    m_interval_ms = interval_ms ;

    return 0 ;
}

int application::set_delay_stop_timer(int interval_ms)
{
    if(interval_ms < MIN_INTERVAL || interval_ms > DEFAULT_INTERVAL*10 )
    {
        return -1 ;
    }

    m_delay_ms = interval_ms ;

    return 0 ;
}


void application::on_signal(int signo)
{
    if(m_sig_status != STATUS_RUN ) return ;

    switch(signo)
    {
    case SIGINT:
    case SIGTERM:
    case SIGQUIT:
    case SIGUSR2:
        m_sig_status = STATUS_PRE_STOP ;
        break ;
    case SIGUSR1:
        m_sig_status = STATUS_RELOAD ;
        break ;
    default:
        ;
    }

}

void application::stop()
{
    set_status(STATUS_PRE_STOP) ;

}


void application::change_work_directory(const char* argv0)
{

    if(m_work_path) chdir(m_work_path) ;
    else
    {
        char* cmd = strdup(argv0) ;
        if(cmd)
        {
            chdir(dirname(cmd)) ;
            free(cmd) ;
        }

    }


}

int application::on_parse_args(int argc,char** argv)
{
    return parse_option(argc,argv) ;
}


int application::start(int argc,char** argv)
{
    if(on_parse_args(argc,argv)!= 0 ) error_return(-1,"parse_option failed") ;

    change_work_directory(argv[0]) ;
    
    if(m_lockfile)
    {
        int lock_fd = lock_file(m_config_file);
        if(lock_fd < 0 ) error_return(-1,"lock config file failed") ;
    }

    if(m_daemon) daemon_init(1,1) ;

    if(m_event_engine.init(m_max_open_fd) !=0 )
    {
        error_return(-1,"init epoll_reactor failed") ;
    }


    if(m_timer_engine.init(ms_to_timer_clock(get_run_ms()),10) !=0 )
    {
        error_return(-1,"init timer_manager failed");
    }


    if(on_init() != 0 ) error_return(-1,"on_init failed") ;

    m_status = STATUS_RUN ;
    m_sig_status = STATUS_RUN ;
    printf("system started\n") ;
    
    on_timeout(&m_timer_engine) ;
    while(1)
    {
        m_event_engine.run_once(timer_clock_to_ms(m_timer_engine.get_next_expired()) - get_run_ms() ) ;
        int64_t now_clock = ms_to_timer_clock(get_run_ms()) ;
        m_timer_engine.run_until(now_clock) ;
        

        if(m_sig_status == STATUS_RELOAD )
        {
            m_sig_status = STATUS_RUN ;
            on_reload() ;
        }
        else if(m_sig_status == STATUS_PRE_STOP)
        {
            m_sig_status = STATUS_RUN ;
            if(!m_delay_timer.is_running() )
            {
                del_timer(&m_timer);
                on_delay_stop() ;
                //cannot add delay timer , stop directly
                if(add_timer_after(&m_delay_timer,m_delay_ms)!=0 ) set_status(STATUS_STOP) ;

            }
        }

        if(m_status != STATUS_RUN)
        {
            break ;
        }
    }

    on_fini() ;
    printf("system stopped\n") ;

    return 0 ;
}

int application::add_timer_after(base_timer* timer,int duration_ms)
{
    if(duration_ms < MIN_INTERVAL ) return -1 ;
    timer->set_expired(ms_to_timer_clock(get_run_ms() + duration_ms)) ;
    return m_timer_engine.add_timer(timer) ;
}

int application::add_timer_until(base_timer* timer,int64_t expired_ms)
{
    int duration_ms = expired_ms - time_ms() ;
    return add_timer_after(timer,duration_ms) ;
}
 
void application::del_timer(base_timer* timer)
{
    m_timer_engine.del_timer(timer) ;
}

void application::on_delay_timeout(timer_manager* manager)
{
    set_status(STATUS_STOP) ;
}

void application::set_status(int8_t status)
{
    m_status = status ;
}
 


}

