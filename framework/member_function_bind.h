/**
 * member_function_bind.h
 * Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

namespace framework
{

template<typename R,typename T>
class simple_member_func0
{
private:
    typedef R (T::*F)() ;
    F m_f ;
    T* m_t ;
public:
    simple_member_func0(F f,T* t):m_f(f),m_t(t) {} ;
    R operator()()
    {
        return (m_t->*m_f)() ;
    }
} ;

template<typename R,typename T,typename A1>
class simple_member_func1
{
public:
    typedef R (T::*F)(A1) ;
    F m_f ;
    T* m_t ;
public:
    simple_member_func1(F f,T* t):m_f(f),m_t(t) {} ;
    R operator()(A1& a1)
    {
        return (m_t->*m_f)(a1) ;
    }

} ;


template<typename R,typename T,typename A1,typename A2>
class simple_member_func2
{
private:
    typedef R (T::*F)(A1,A2) ;
    F m_f ;
    T* m_t ;
public:
    simple_member_func2(F f,T* t):m_f(f),m_t(t) {} ;
    R operator()(A1& a1,A2& a2)
    {
        return (m_t->*m_f)(a1,a2) ;
    }
} ;

template<typename R,typename T,typename A1,typename A2,typename A3>
class simple_member_func3
{
private:
    typedef R (T::*F)(A1,A2,A3) ;
    F m_f ;
    T* m_t ;
public:
    simple_member_func3(F f,T* t):m_f(f),m_t(t) {} ;
    R operator()(A1& a1,A2& a2,A3& a3)
    {
        return (m_t->*m_f)(a1,a2,a3) ;
    }
} ;

template<typename R,typename T,typename A1,typename A2,typename A3,typename A4>
class simple_member_func4
{
private:
    typedef R (T::*F)(A1,A2,A3,A4) ;
    F m_f ;
    T* m_t ;
public:
    simple_member_func4(F f,T* t):m_f(f),m_t(t) {} ;
    R operator()(A1& a1,A2& a2,A3& a3,A4& a4)
    {
        return (m_t->*m_f)(a1,a2,a3,a4) ;
    }
} ;


template<typename R,typename T>
simple_member_func0<R,T> member_function_bind(R (T::*f)(),T* t)
{
    return simple_member_func0<R,T>(f,t) ;
}

template<typename R,typename T,typename A1>
simple_member_func1<R,T,A1> member_function_bind(R (T::*f)(A1),T* t)
{
    return simple_member_func1<R,T,A1>(f,t) ;
}



template<typename R,typename T,typename A1,typename A2>
simple_member_func2<R,T,A1,A2> member_function_bind(R (T::*f)(A1,A2),T* t)
{
    return simple_member_func2<R,T,A1,A2>(f,t) ;
}



template<typename R,typename T,typename A1,typename A2,typename A3>
simple_member_func3<R,T,A1,A2,A3> member_function_bind(R (T::*f)(A1,A2,A3),T* t)
{
    return simple_member_func3<R,T,A1,A2,A3>(f,t) ;
}

template<typename R,typename T,typename A1,typename A2,typename A3,typename A4>
simple_member_func4<R,T,A1,A2,A3,A4> member_function_bind(R (T::*f)(A1,A2,A3,A4),T* t)
{
    return simple_member_func4<R,T,A1,A2,A3,A4>(f,t) ;
}


}

