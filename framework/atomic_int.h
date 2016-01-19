/**
 * atomic_int.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <stdint.h>

namespace framework
{

template<typename T>
class atomic_int
{
public:
    typedef T int_type ;

    atomic_int():m_counter(0) { } ;
    ~atomic_int() { } ;


    /**
     * brief atomically read value
     */
    int_type get() { return m_counter ; } ;

    /**
     * @brief atomically set value
     * @return old value
     */
    int_type set(int_type v) { return __sync_lock_test_and_set(&m_counter,v) ; } ;

    /**
     * @brief atomically add a value and return new value
     * @param [in]: integer value to add
     * @return old value
     */
    int_type add(int_type i) { return __sync_fetch_and_add(&m_counter,i) ; } ;

    /**
     * @brief atomically sub a value and return new value
     * @param [in]: integer value to sub
     * @return old value
     */
    int_type sub(int_type i) { return __sync_fetch_and_sub(&m_counter,i) ; } ;
    
    /**
     * @brief compare and switch
     * @param [in]: old value
     * @param [in]:  new value
     * @return true on success
     */
    bool cas(int_type old_value,int_type new_value) 
    { 
        return __sync_bool_compare_and_swap(&m_counter,old_value,new_value);
    } ;




private:
    volatile int_type m_counter ;

};

typedef atomic_int<int8_t> atomic_int8 ;
typedef atomic_int<int16_t> atomic_int16 ;
typedef atomic_int<int32_t> atomic_int32 ;
typedef atomic_int<int64_t> atomic_int64 ;


}

