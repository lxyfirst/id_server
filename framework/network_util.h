/**
 * network_util.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/types.h>

namespace framework
{

//in_addr_t
typedef struct ::sockaddr_in sa_in_t ;
typedef struct ::sockaddr   sa_t ;
typedef struct ::sockaddr_un sa_un_t ;


/**
 *@brief init address
 *@param [out] address
 *@param [in] ip  string
 *@param [in] port
 */
void init_sa_in(sa_in_t* addr,const char* ip,int port) ;
int compare_sa_in(sa_in_t* addr1,sa_in_t* addr2);
void init_sa_un(sa_un_t* addr,const char* sockfile) ;

const char* addr2str(char* dst,int dst_size,const sa_in_t* addr) ;


int set_nonblock(int fd) ;

void set_socket_buffer(int fd,int val);

//deprecated , use set_socket_reuse
int set_addr_reuse(int fd) ;
int set_socket_reuse(int fd) ;

int set_socket_nodelay(int fd) ;

int set_defer_accept(int fd,int seconds) ;

int get_socket_error(int fd) ;

/*
 * @return : 0 when event happened 
 */
int check_socket_event(int fd,int seconds,bool read_event = true,bool write_event = true);

//option list: SO_ERROR,SO_REUSEADDR,SO_SNDBUF,SO_RCVBUF,SO_KEEPALIVE,SO_LINGER,SO_TYPE,SO_NREAD,SO_NWRITE
int get_socket_option(int fd,int option_name) ;
int set_socket_option(int fd,int option_name,int option_value) ;

/**
 * @param fd
 * @param idle seconds , idle =0 means no keepalive
 * @param count
 * @param interval seconds
 * @return : 0 on success
 */
int set_tcp_keepalive(int fd,int idle=60,int count=2,int interval=10) ;


/** socket,bind,listen  */
int create_tcp_service(sa_in_t* addr) ;
int create_udp_service(sa_in_t* addr) ;

/** socket , connect */
int create_tcp_client(sa_in_t* remote_addr,int timeout) ;
int create_unix_client(sa_un_t* remote_addr,int timeout) ;

}

