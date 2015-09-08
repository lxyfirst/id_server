/*
 * id_server.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include "pugixml/pugixml.hpp"
#include "framework/system_util.h"
#include "framework/mmap_file.h"
#include "framework/time_util.h"
#include "framework/string_util.h"


#include "id_server.h"

using namespace framework ;


IdServer::IdServer()
{
	// TODO Auto-generated constructor stub

}

IdServer::~IdServer()
{
	// TODO Auto-generated destructor stub
}


int IdServer::on_init()
{

    pugi::xml_document xml_config ;
    if(!xml_config.load_file(config_file()))
    {
        error_return(-1,"load config failed") ;
    }
    pugi::xml_node root = xml_config.child("root") ;
    if(!root) error_return(-1,"missing <root> node") ;

    pugi::xml_node node = root.child("log") ;
    if(m_logger.init(node.attribute("prefix").value(),node.attribute("level").as_int() ) !=0 )
    {
        error_return(-1,"init logger failed") ;
    }
    
    m_log_thread.init(m_logger.get_prefix(),m_logger.get_level()) ;


    node = root.child("listen") ;
    if(m_client_handler.init(&reactor(),node.attribute("host").value(),
    		node.attribute("port").as_int())!=0 )
    {
    	error_return(-1,"init udp failed");
    }

    node = root.child("rules") ;
    int offset = node.attribute("offset").as_int() ;
    int step = node.attribute("step").as_int() ;

    if( m_rule_manager.init(offset,step)!=0) error_return(-1,"load rules failed");
    for (pugi::xml_node rule = node.first_child(); rule;rule = rule.next_sibling())
    {
    	if(m_rule_manager.load_rule_config(rule)!=0)  error_return(-1,"load rules failed");
    }



    node = root.child("database") ;
    ThreadConfig thread_config ;
    thread_config.host = node.attribute("host").value() ;
    thread_config.port = node.attribute("port").as_int();
    thread_config.user = node.attribute("user").value();
    thread_config.password = node.attribute("password").value();
    thread_config.dbname = node.attribute("dbname").value();
    int thread_count = node.attribute("thread_count").as_int() ;

    if( load_counter_data(m_counter_manager,m_rule_manager.get_offset(),thread_config) < 0 )
    {
    	error_return(-1,"load counter failed") ;
    }

    if(m_log_thread.start() !=0) error_return(-1,"init log thread failed") ;

    if(m_thread_manager.init(thread_count,thread_config,m_log_thread)!=0)
    {
        error_return(-1,"init thread failed") ;
    }
   
    set_delay_stop_timer(1000) ;
    set_app_timer(60000) ;
    info_log_string(m_logger,"system started") ;
    return 0 ;
}

int IdServer::on_reload()
{
    pugi::xml_document xml_config ;
    if(!xml_config.load_file(config_file()))
    {
        error_return(-1,"load config failed") ;
    }
    pugi::xml_node root = xml_config.child("root") ;
    if(!root) error_return(-1,"missing <root> node") ;

    m_logger.fini() ;
    pugi::xml_node node = root.child("log") ;
    if(m_logger.init(node.attribute("prefix").value(),node.attribute("level").as_int() ) !=0 )
    {
        error_return(-1,"init logger failed") ;
    }

    m_log_thread.init(node.attribute("prefix").value(),node.attribute("level").as_int()) ;

    node = root.child("rules") ;
    for (pugi::xml_node rule = node.first_child(); rule;rule = rule.next_sibling())
    {
    	if(m_rule_manager.load_rule_config(rule)!=0)  error_return(-1,"load rules failed");
    }

    info_log_string(m_logger,"system reload success") ;
    return 0 ;
}


void IdServer::on_fini()
{

    m_thread_manager.fini() ;

    m_log_thread.stop() ;
    m_log_thread.join() ;

	info_log_string(m_logger,"system stopped") ;

}

void IdServer::on_delay_stop()
{
	m_client_handler.fini() ;
}

void IdServer::on_timer()
{
    int avg_time = m_request_count >0 ? m_total_time / m_request_count : 0 ;
    debug_log_format(m_logger,"request count:%d avg_time:%d",m_request_count,avg_time) ;
    m_request_count = 0 ;
    m_total_time = 0 ;
}


int IdServer::create_id(string& new_id,const string& rule_name,const string& app_name,const string& salt)
{
    int64_t begin_time = time_ms() ;
	Rule* rule = m_rule_manager.get_rule(rule_name) ;
	if(rule == NULL) return -1 ;
    if(have_escape_char(app_name.data(),app_name.size()) ) return -2 ;

	Counter* counter = m_counter_manager.get_counter(rule_name,app_name) ;
	if(counter == NULL )
	{
		counter = m_counter_manager.create_counter(rule_name,app_name);
		if(counter == NULL) return -3 ;
		counter->init(rule_name,app_name,rule->config) ;
	}

	rule->lua_manager.create_id(new_id,counter,salt) ;

    m_request_count +=1 ;
    m_total_time += time_ms() - begin_time ;

	return 0 ;


}

void IdServer::create_format_id(string& new_id,const string& format,Counter* counter,int width_counter)
{
	string_vector tpl_list ;
	split(tpl_list,format.c_str(),format.size(),'#') ;

	char buf[64] = {0} ;
	int now = counter->generate_time() ;
	for(string_vector::iterator it=tpl_list.begin();it!=tpl_list.end();++it)
	{
		string& item = *it ;
		if(item.compare("date")==0)
		{
			time2str(buf,sizeof(buf),"%y%m%d",now) ;
			new_id.append(buf) ;
		}
		else if(item.compare("time")==0)
		{
			time2str(buf,sizeof(buf),"%H%M%S",now) ;
			new_id.append(buf) ;
		}
		else if(item.compare("counter")==0)
		{
			snprintf(buf,sizeof(buf),"%0*d",width_counter, counter->generate_counter()) ;
			new_id.append(buf) ;
		}
		else
		{
			new_id.append(item) ;
		}
	}

}

IMPLEMENT_APPLICATION_INSTANCE(IdServer) ;
IMPLEMENT_MAIN() ;

