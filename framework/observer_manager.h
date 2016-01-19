/**
 *  application.h
 *  
 *      Author: lixingyi (lxyfirst@163.com)
 */ 

#pragma once

#include <tr1/unordered_map>
#include <tr1/unordered_set>

namespace framework
{

class event_observer
{
public:
    virtual void on_event(int event_id,void* arg) = 0 ;
} ;

class observer_manager
{
public:
    typedef std::tr1::unordered_set<event_observer*> observer_container ;
    typedef std::tr1::unordered_map<int,observer_container > subscriber_container ;
public:
    void subscribe(int event_id,event_observer* observer) ;
    void unsubscribe(int event_id,event_observer* observer) ;

    void publish(int event_id,void* arg) ;
    
private:
    subscriber_container m_subscriber_list ;
     
} ;

}



