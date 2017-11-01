/*
 * network_util.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#define _XOPEN_SOURCE

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "network_util.h"

namespace framework
{


void init_sa_in(sa_in_t* addr,const char* ip,int port)
{
    if ( addr == NULL || ip == NULL || port < 1 ) return  ;
    memset(addr,0,sizeof(sa_in_t)) ;
    addr->sin_family = AF_INET ;
    addr->sin_port = htons(port) ;
    addr->sin_addr.s_addr = inet_addr(ip) ;
}

bool equal_sa_in(const sa_in_t* addr1,const sa_in_t* addr2)
{
    if (addr1->sin_port == addr2->sin_port && 
        addr1->sin_addr.s_addr == addr2->sin_addr.s_addr ) 
    {
        return true ;
    }
        

    return false ;
}


void init_sa_un(sa_un_t* addr,const char* file)
{
    if ( addr == NULL || file == NULL ) return  ;
    memset(addr,0,sizeof(sa_un_t)) ;
    addr->sun_family = AF_LOCAL ;
    strncpy(addr->sun_path,file,sizeof(addr->sun_path)-1) ;
}

const char* addr2str(char* dst,int dst_size,const sa_in_t* addr)
{
    return inet_ntop(AF_INET,(const void*)&(addr->sin_addr),dst,dst_size) ;
}

int set_nonblock(int fd)
{
    int flag = fcntl(fd,F_GETFL) ;
    if ( flag == -1)
    {
        return -1 ;
    }

    return fcntl(fd,F_SETFL,flag | O_NONBLOCK) ;
}

void set_socket_buffer(int fd,int val)
{
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &val, sizeof(val)) ;
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &val, sizeof(val)) ;
}

int set_addr_reuse(int fd)
{
    int reuse = 1 ;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) ;

}

int set_socket_reuse(int fd)
{
    int reuse = 1 ;
    if( setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))!=0) return -1 ;
    if( setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse))!=0) return -1 ;
    return 0 ;

}


int set_socket_nodelay(int fd)
{
    int one = 1 ;
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &one, sizeof(one)) ;
}

int set_defer_accept(int fd,int seconds)
{
    return setsockopt(fd, IPPROTO_TCP, TCP_DEFER_ACCEPT, (char *) &seconds, sizeof(seconds)) ;    
}



int get_socket_error(int fd)
{
    int error_code = 0 ;
    socklen_t len = (socklen_t)sizeof(error_code);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &error_code, &len);

    return error_code ;
}

int get_socket_option(int fd,int option_name)
{
    int option_value = 0 ;
    socklen_t len = (socklen_t)sizeof(option_value);
    if ( getsockopt(fd, SOL_SOCKET, option_name , &option_value, &len)!=0 ) return -1;
    
    return option_value ;
}

int set_socket_option(int fd,int option_name,int option_value)
{
    return setsockopt(fd, SOL_SOCKET, option_name , (char*)&option_value,sizeof(option_value) ) ;
}

int set_tcp_keepalive(int fd,int idle,int count,int interval)
{
    int keepalive = idle >0 ? 1 : 0 ;
    int ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE , (char*)&keepalive,sizeof(keepalive) ) ;
    if ( keepalive >0 && ret == 0 )
    {
        setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (char*)&idle,sizeof(idle) ) ;
        setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (char*)&count,sizeof(count) ) ;
        setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (char*)&interval,sizeof(interval) ) ;
    }
    
    return ret ;

}





int create_tcp_service(sa_in_t* addr)
{
    if ( addr == NULL  ) return -1 ;
    int sockfd = socket(AF_INET,SOCK_STREAM,0) ;
    if ( sockfd < 0 ) return -1 ;
    set_nonblock(sockfd) ;
    set_socket_reuse(sockfd) ;
    if( bind(sockfd,(sa_t*)addr,sizeof(sa_in_t)) !=0 )
    {
        close(sockfd) ;
        return -2 ;
    }
    if ( listen(sockfd,1024) != 0 )
    {
        close(sockfd) ;
        return -2 ;
    }

    return sockfd ;

}

int create_udp_service(sa_in_t* addr)
{
    if ( addr == NULL ) return -1 ;
    int sockfd = socket(AF_INET,SOCK_DGRAM,0) ;
    if ( sockfd < 0 ) return -1 ;
    set_nonblock(sockfd) ;
    set_socket_reuse(sockfd) ;
    if( bind(sockfd,(sa_t*)addr,sizeof(sa_in_t)) !=0 )
    {
        close(sockfd) ;
        return -2 ;
    }
    

    return sockfd ;

}

int check_socket_event(int fd,int seconds,bool read_event,bool write_event)
{
    struct pollfd pfd = {0} ;
    pfd.fd = fd;
    pfd.events = 0 ;
    if(read_event) pfd.events |= POLLIN ;
    if(write_event) pfd.events |= POLLOUT ;

    if( poll(&pfd,1,seconds*1000) <1)
    {
        errno = ETIMEDOUT ;
        return -1 ;
    }


    errno = get_socket_error(fd) ;
    if ( errno!=0 )
    {
        return -1 ;
    }

    return 0 ;

}

int create_tcp_client(sa_in_t* addr,int second)
{
    if ( addr == NULL ) return -1 ;

    int sockfd = socket(AF_INET,SOCK_STREAM,0) ;
    if ( sockfd < 0 ) return -1 ;
    set_nonblock(sockfd) ;

    if ( connect(sockfd,(sa_t*)addr,sizeof(sa_in_t)) != 0 && errno != EINPROGRESS )
    {
        close(sockfd) ;
        return -errno ;

    }

    if(second == 0) return sockfd ;

    if(check_socket_event(sockfd,second)!=0)
    {
        close(sockfd) ;
        return -errno ;
    }

    return sockfd ;

}


int create_unix_client(sa_un_t* addr,int second)
{
    if ( addr == NULL ) return -1 ;


    int sockfd = socket(AF_LOCAL,SOCK_STREAM,0) ;
    if ( sockfd < 0 ) return -1 ;
    set_nonblock(sockfd) ;
    if ( connect(sockfd,(sa_t*)addr,sizeof(sa_un_t)) != 0 && errno != EINPROGRESS )
    {
        close(sockfd) ;
        return -errno ;
    }

    if(second == 0) return sockfd ;

    if(check_socket_event(sockfd,second)!=0)
    {
        close(sockfd) ;
        return -errno ;
    }

    return sockfd ;
}

}

