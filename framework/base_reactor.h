/*
 * base_reactor.h
 * Author: lixingyi (lxyfirst@163.com)
 */

#ifndef BASE_REACTOR_H_
#define BASE_REACTOR_H_

namespace framework
{

class io_handler ;

class base_reactor
{
public:
    enum event_type
    {
        EVENT_READ = 0x001 ,
        EVENT_WRITE = 0x004 ,
    }  ;

public:
    base_reactor(){ } ;
    virtual ~base_reactor() { } ;

public:
    /*
     * @brief register fd handler to epoll engine
     * @param [in] fd to be watched
     * @param [in] handler to callback
     * @param [in] event type
     * @return 0 on success , nagtive on failed
     */
    virtual int add_handler(int fd , io_handler* handler,int event_type) =0;

    /*
     * @brief modify fd watched
     * @param [in] fd have been watched
     * @param [in] handler to callback
     * @param [in] event type
     * @return 0 on success
     */
    virtual int mod_handler(int fd , io_handler* handler ,int event_type ) =0;

    /*
     * @brief remove fd watched
     * @param [in] fd have been watched
     */
    virtual void del_handler(int fd ) =0;

    /*
     * @brief get handler by fd
     * @param [in] fd have been watched
     * @return  handler pointer , NULL is not exist
     */
    virtual io_handler* get_handler(int fd) =0 ;

    /*
     * @brief watch events one time , will block msec milliseconds at most
     * @param [in] max milliseconds to wait
     * @return events count
     */
    virtual int run_once(int msec) =0;


private:
    base_reactor(const base_reactor& ) ;
    base_reactor& operator=(const base_reactor&) ;

};

}


#endif /* BASE_REACTOR_H_ */
