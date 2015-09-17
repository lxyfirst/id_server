/*
 * system_util.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>

#include "system_util.h"

namespace framework
{


int set_thread_title(const char* fmt, ...)
{
    char title [16] ={0};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf (title, sizeof (title) , fmt, ap);
    va_end (ap);

   return prctl(PR_SET_NAME,title) ;

}
void set_process_title(int argc,char* argv[],const char* fmt, ...)
{
    char *arg_start = argv[0];
    char *arg_end = argv[argc-1] + strlen(argv[argc-1])+1;

    char title [256] ={0};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf (title, sizeof (title), fmt, ap);
    va_end (ap);

    int i=0;
    int tlen = strlen (title) + 1;
    if (arg_end - arg_start < tlen && environ[0] == arg_end)
    {
        char *env_next = environ[0];
        for(i=0; environ[i]!=NULL; i++)
        {
            if(env_next == environ[i])
            {
                env_next = environ[i] + strlen (environ[i]) + 1;
                environ[i] = strdup(environ[i]);
            }
            else
            {
                break;
            }
        }
        arg_end = env_next;

    }

    i = arg_end - arg_start;
    if (tlen <= i)
    {
        strcpy(arg_start, title);
        memset(arg_start + tlen, 0, i - tlen);
    }
    else
    {
        stpncpy(arg_start, title, i - 1)[0] = '\0';
    }
}

int lock_file(const char* filename)
{
    int fd = open(filename,O_RDONLY|O_CREAT,0755) ;
    if ( fd < 0 ) return -1 ;
    if( flock(fd,LOCK_EX|LOCK_NB)!=0 )  return -1;
    return fd ;
}

int set_open_file_limit(int maxsize)
{
    if (maxsize < 0) return -1;
    struct rlimit limit = {(size_t)maxsize, (size_t)maxsize} ;
    return setrlimit(RLIMIT_NOFILE,&limit) ;
}

int get_open_file_limit()
{
    struct rlimit limit = {0} ;
    if( getrlimit(RLIMIT_NOFILE,&limit)!=0) return -1 ;
    return limit.rlim_max ;
}

int daemon_init(int nochdir,int noclose)
{
    pid_t pid = 0 ;
    if( (pid = fork() ) < 0 ) return -1 ;
    else if(pid != 0 )  _exit(0) ;
    
    setsid() ;
    umask(0) ;
    // Ensure future opens won't allocate controlling TTYs
    struct sigaction sa ;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN; 
    sigemptyset(&sa.sa_mask); 
    if (sigaction(SIGHUP, &sa, NULL) < 0) return -1 ;

    if( (pid = fork() ) < 0 ) return -1 ;
    else if(pid != 0 )  _exit(0) ;

    if((!nochdir) && (chdir("/")!=0) ) return -1 ;

    if(!noclose)
    {
        int fd =  open("/dev/null", O_RDWR);
        if(fd < 0 ) return -1 ;
        dup2(fd,0) ;
        dup2(fd,1) ;
        dup2(fd,2) ;
        close(fd);
    }

    return 0 ;
    
    
}

int get_uid_by_name(const char* username)
{
    struct passwd* pwd = getpwnam(username);
    if(pwd == NULL ) return -1 ;
    return pwd->pw_uid ;
}
 

int set_uid_by_name(const char* username) 
{
    int uid = get_uid_by_name(username) ;
    if(uid < 0) return -1 ;
    
    return setuid(uid) ;

}




}

