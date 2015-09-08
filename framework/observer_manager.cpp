/*
 *  application.h
 *  
 *      Author: lixingyi (lxyfirst@163.com)
 */
#include <algorithm>
#include "observer_manager.h"

namespace framework
{

void observer_manager::subscribe(int event_id,event_observer* observer)
{
    observer_container& oc = m_subscriber_list[event_id] ;
    
    oc.insert(observer) ;

}

void observer_manager::unsubscribe(int event_id,event_observer* observer)
{
    subscriber_container::iterator si = m_subscriber_list.find(event_id) ;
    if(si != m_subscriber_list.end() )
    {
        si->second.erase(observer) ;
    }
}

void observer_manager::publish(int event_id,void* arg)
{
    subscriber_container::iterator si = m_subscriber_list.find(event_id) ;
    if(si != m_subscriber_list.end() )
    {
        /*
        class callable
        {
        public:
            callable(int event_id,void* arg):m_event_id(event_id),m_arg(arg) {} ;
            void operator()(event_observer* observer) { observer->on_event(m_event_id,m_arg) ; }
        private:
            int m_event_id ;
            void* m_arg ;
        } callback(event_id,arg) ;

        std::for_each(si->second.begin(),si->second.end(),callback) ;
        */

        
        observer_container& oc = si->second ;
        observer_container::iterator it = oc.begin();
        while(it!=oc.end())
        {
            event_observer* observer = *it ;
            ++it ;
            observer->on_event(event_id,arg) ;
        }
        
    }

}

}

