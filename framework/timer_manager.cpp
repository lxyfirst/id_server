/*
 * timer_manager.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include <assert.h>
#include "timer_manager.h"

namespace framework
{

void base_timer::base_timer_insert(base_timer* prev,base_timer* curr,base_timer* next)
{
    prev->m_next = curr ;

    curr->m_prev = prev ;
    curr->m_next = next ;

    next->m_prev = curr ;
}
void base_timer::base_timer_remove(base_timer* timer)
{
    timer->m_prev->m_next = timer->m_next ;
    timer->m_next->m_prev = timer->m_prev ;
    timer->m_next = timer->m_prev = NULL ;
}

timer_manager::timer_manager():m_low_array(NULL),m_high_array(NULL)
{

}
timer_manager::~timer_manager()
{
    fini() ;
}

int timer_manager::init(int64_t start_time , int slot_bits)
{
    if(slot_bits < step_slot_min_bits || slot_bits > step_slot_max_bits) return -1 ;
    if(m_low_array != NULL) return -2 ;

    m_step_slot_bits = slot_bits ;
    m_step_slot_size = (0x1 << m_step_slot_bits) ;
    m_step_slot_mask = ~((~0x0) << m_step_slot_bits ) ;
    m_max_expired = m_step_slot_size << m_step_slot_bits ;

    m_curr_expired = start_time ;
    m_next_expired = m_curr_expired + m_step_slot_size ;

    m_low_pos = m_high_pos = 0 ;

    base_timer *timer_array = new base_timer[m_step_slot_size*2] ;
    if(timer_array == NULL) return -3 ;
    m_low_array = timer_array ;
    m_high_array = timer_array + m_step_slot_size ;

    base_timer* head = NULL ;
    for(int i=0 ; i < m_step_slot_size ; ++i)
    {
        head = m_low_array + i ;
        head->m_next = head->m_prev = head ;

        head = m_high_array + i ;
        head->m_next = head->m_prev = head ;
    }


    return 0 ;
}

void timer_manager::fini()
{
    if(m_low_array)
    {
        delete[] m_low_array ;
        m_low_array = m_high_array = NULL ;
    }
}



int timer_manager::add_timer(base_timer* timer)
{
    if(m_low_array == NULL || timer == NULL || (timer->m_next!= NULL) ) return -1 ;
    int interval = timer->m_expired - m_curr_expired ;
    if ( interval < 1 || interval > m_max_expired ) return -2 ;

    //calc positon
    base_timer* head = NULL ;
    if ( interval >= m_step_slot_size )
    {
        head = m_high_array +( ( ((interval + m_low_pos) >> m_step_slot_bits) -1 + m_high_pos ) & m_step_slot_mask) ;

    }
    else
    {
        head = m_low_array + ((interval + m_low_pos) & m_step_slot_mask) ;

    }

    base_timer::base_timer_insert(head->m_prev,timer,head) ;

    //update next expired
    if(m_next_expired > timer->m_expired ) m_next_expired = timer->m_expired ;

    return 0 ;
}

void timer_manager::del_timer(base_timer* timer)
{
    if(timer->m_next && timer->m_prev)
    {
        base_timer::base_timer_remove(timer) ;
    }
} ;

void timer_manager::run_until(int64_t now)
{
    //printf("run_until,now=%ld curr_expired=%ld next_expired=%ld\n",now,m_curr_expired,m_next_expired);
    
    skip_to_next_expired(now) ;
    //run 
    while( m_curr_expired < now )
    {
        ++m_curr_expired ;
        m_low_pos = ( m_low_pos +1 ) & m_step_slot_mask ;

        //move timers from high slot to low slots when run a cycle
        if(m_low_pos == 0)
        {
            shift_high_slot() ;
        }

        // trigger expired timers
        base_timer* expired_head = m_low_array + m_low_pos ;
        while(expired_head != expired_head->m_next)
        {
            base_timer* curr = expired_head->m_next ;
            base_timer::base_timer_remove(curr) ;
            curr->on_timeout(this) ;

        }

    }

    update_next_expired() ;

}

void timer_manager::skip_to_next_expired(int64_t now)
{
    //skip empty slots
    int skip = (now > m_next_expired ? m_next_expired : now) - m_curr_expired  -1 ;
    if(skip > 0)
    {
        m_curr_expired += skip ;
        m_high_pos = ( m_high_pos +((m_low_pos + skip) >> m_step_slot_bits) ) & m_step_slot_mask ;
        m_low_pos = ( m_low_pos +skip ) & m_step_slot_mask ;
    }
 
}

void timer_manager::update_next_expired()
{
    //update next expired counter by nearist timer , try max step_slot_size times
    if(m_next_expired <= m_curr_expired )
    {
        
        int i = m_low_pos ;
        for( ; i < m_step_slot_size ; ++i )
        {
            base_timer* next_head = m_low_array + i ;
            if(next_head != next_head->m_next) break ;
        }
        m_next_expired = m_curr_expired + i - m_low_pos  ;
       
    }


}

void timer_manager::shift_high_slot()
{
    base_timer* head = m_high_array + m_high_pos ;
    while(head != head->m_next)
    {
        base_timer* curr = head->m_next ;
        base_timer::base_timer_remove(curr) ;

        assert(curr->m_expired >= m_curr_expired ) ;
        int low_pos = (curr->m_expired - m_curr_expired  + m_low_pos) & m_step_slot_mask ;

        base_timer* low_head = m_low_array + low_pos ;
        base_timer::base_timer_insert(low_head->m_prev,curr,low_head) ;

    }

    m_high_pos = ( m_high_pos +1 ) & m_step_slot_mask ;


}

}
