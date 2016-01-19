/**
 * timer_manager.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>


namespace framework
{

class timer_manager  ;

/*
 * @brief  basic timer class , doubled linked
 */
class base_timer
{
friend class timer_manager ;

public:
    base_timer() : m_next(NULL),m_prev(NULL),m_expired(0) { } ;
    virtual ~base_timer() { if(m_prev && m_next) base_timer_remove(this);} ;

    /*
     * @brief get timer counter
     */
    int64_t get_expired() { return m_expired ;} ;

    /*
     * @brief set timer counter
     * @param [in] timer counter
     * @return 0 on sucess
     */
    int set_expired(int64_t expired)
    {
        if(m_next || m_prev) return -1 ;
        m_expired = expired;
        return -1 ;
    } ;


    bool is_running() { return (m_next || m_prev) ; } ;

protected:
    /*
     * @brief callback implemented by concrete class
     * called when timer expired
     */
    virtual void on_timeout(timer_manager* manager)  { abort(); } ;

private:
    static void base_timer_insert(base_timer* prev,base_timer* curr,base_timer* next);
    static void base_timer_remove(base_timer* timer);
private:

    base_timer* m_next ;
    base_timer* m_prev ;
    int64_t m_expired ;
} ;

template<typename T,void (T::*callback)(timer_manager* manager) =&T::on_timeout>
class template_timer : public base_timer
{
public:
    explicit template_timer(T* owner = NULL ):m_owner(owner) { } ;

    void set_owner(T* owner) { m_owner = owner ; } ;

protected:
    virtual void on_timeout(timer_manager* manager) { (m_owner->*callback)(manager) ;} ;

private:
    T* m_owner ;
};

class timer_manager
{
public:


public:
    timer_manager() ;
    ~timer_manager();

public:
    /*
     * @brief initialize , alloc memory
     * @param [in] time counter begin to run
     * @param [in] slot bits
     * @return 0 on success
     */
    int init(int64_t start_time,int slot_bits) ;


    void fini() ;

    /*
     * @brief insert timer
     * @param [in] timer to be inserted
     * @return 0 on success
     */
    int add_timer(base_timer* timer) ;

    /*
     * @brief remove timer
     * @param [in] timer
     */
    void del_timer(base_timer* timer) ;

    /*
     * @brief call by main loop to run expired timers untill now
     */
    void run_until(int64_t now) ;

    int64_t get_curr_expired() const { return m_curr_expired ;} ;

    /*
     * @brief the latest timer counter to be expired
     */
    int64_t get_next_expired() const { return m_next_expired ;} ;

    int32_t get_max_timeout() const { return m_max_expired ; } ;
private:
    enum
    {
        step_slot_min_bits = 3 ,
        step_slot_max_bits = 20 ,

    } ;
private:
    timer_manager(const timer_manager& ) ;
    timer_manager& operator=(const timer_manager&) ;

    void skip_to_next_expired(int64_t now) ;
    void update_next_expired() ;
    void shift_high_slot() ;

private:
    base_timer *m_low_array ;
    base_timer *m_high_array ;
    int64_t m_curr_expired ;
    int64_t m_next_expired ;
    int8_t m_step_slot_bits ;
    int16_t m_step_slot_size ;
    int32_t m_step_slot_mask ;
    int32_t m_max_expired ;
    int32_t m_low_pos ;
    int32_t m_high_pos ;

};

}

