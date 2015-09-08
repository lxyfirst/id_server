/*
 * object_switcher.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#ifndef OJBECT_SWITCHER_H_
#define OJBECT_SWITCHER_H_



namespace framework
{


template<typename T>
class object_switcher
{
public:
    object_switcher():m_first_active(true){ } ;

    T& active() { return m_first_active ? m_objects[0] : m_objects[1] ; } ;
    T& backup() { return m_first_active ? m_objects[1] : m_objects[0] ; } ;

    void switch_object() { m_first_active = !m_first_active ; } ;

private:
    object_switcher(const object_switcher& ) ;
    object_switcher& operator=(const object_switcher&) ;

private:
    T m_objects[2] ;
    volatile bool m_first_active ;

};

}

#endif /* IO_HANDLER_H_ */

