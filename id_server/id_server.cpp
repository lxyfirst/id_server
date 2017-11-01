/**
 * id_server.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include "framework/system_util.h"
#include "framework/mmap_file.h"
#include "framework/time_util.h"
#include "framework/string_util.h"


#include "id_server.h"

using namespace framework ;


IdServer::IdServer()
{

}

IdServer::~IdServer()
{
}

int IdServer::load_config(Json::Value& config)
{
    mmap_file mfile ;
    if(mfile.load_file(config_file())!=0 ) return -1 ;
    const char* begin = (const char*)mfile.file_data() ;
    if(!json_decode(config,begin,mfile.file_size()) ) error_return(-1,"decode json failed");
    static const JsonFieldInfo config_field_list{
        {"log_prefix",Json::stringValue},
        {"log_level",Json::intValue },
        {"service_host",Json::stringValue },
        {"service_port",Json::intValue },
        {"id_step",Json::intValue },
        {"id_offset",Json::intValue },
        {"database",Json::objectValue},
        {"rules",Json::arrayValue},
    } ;

    if(!json_check_field(config,config_field_list)) error_return(-1,"missing some fields") ;

    return 0 ;
}

int IdServer::init_logger(const Json::Value& config)
{

    const char* log_prefix = config["log_prefix"].asCString() ;
    int log_level = config["log_level"].asInt() ;
    if(m_logger.init(log_prefix,log_level)!=0)  return -1 ;
    return m_log_thread.init(log_prefix,log_level) ;

}

int IdServer::load_rules(const Json::Value& rules_config)
{
    for(const auto& rule : rules_config)
    {
        std::string name = json_get_value(rule,"name","") ;
        std::string lua_file = json_get_value(rule,"lua_file","") ;
        int batch_save = json_get_value(rule,"batch_save",0) ;
        if(m_rule_manager.load_rule_config(name,lua_file,batch_save)!=0)  error_return(-1,"load rules failed");
    }

    return 0 ;

}


int IdServer::on_init()
{

    Json::Value config ;
    if(load_config(config)!=0) error_return(-1,"load config failed") ;
    if(init_logger(config)!=0) error_return(-1,"init logger failed") ;

    if(m_log_thread.start() !=0) error_return(-1,"init log thread failed") ;

    if(m_client_handler.init(reactor(),json_get_value(config,"service_host","0.0.0.0"),
            json_get_value(config,"service_port",1200) )!=0 )
    {
        error_return(-1,"init udp failed");
    }

    int step = json_get_value(config,"id_step",0);
    int offset = json_get_value(config,"id_offset",0);
    if( offset < 0 || offset >= step ) error_return(-1,"invalid id_step or id_offset") ;
    if( m_rule_manager.init(offset,step)!=0) error_return(-1,"init rules failed");
    if( load_rules(config["rules"]) !=0) error_return(-1,"load rules failed") ;

    const Json::Value& database = config["database"] ;
    ThreadConfig thread_config ;
    thread_config.host = database["host"].asString() ;
    thread_config.port = database["port"].asInt();
    thread_config.user = database["user"].asString() ;
    thread_config.password = database["password"].asString() ;
    thread_config.dbname = database["dbname"].asString() ;
    thread_config.charset= database["charset"].asString() ;
    thread_config.queue_size= json_get_value(config,"queue_size",100000);
    if(thread_config.queue_size < 10 || thread_config.queue_size > 1000000) 
    {
        error_return(-1,"invalid queue_size, should between (10,1000000)") ;
    }

    if( load_counter_data(m_counter_manager,m_rule_manager.get_offset(),thread_config) < 0 )
    {
        error_return(-1,"load counter failed") ;
    }


    int thread_count = json_get_value(config,"thread_count",4) ;
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

    Json::Value config ;
    if(load_config(config)!=0)
    {
        error_log_string(m_logger,"load config failed") ;
        return -1 ;
    }

    m_logger.fini() ;
    init_logger(config)  ;

    if(load_rules(config["rules"])!=0) error_log_string(m_logger,"load rules failed") ;

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
    trace_log_format(m_logger,"request count:%d avg_time:%d",m_request_count,avg_time) ;
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
        counter = m_counter_manager.create_counter(rule_name,app_name,rule->config);
        if(counter == NULL) return -3 ;
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

IMPLEMENT_MAIN(get_app()) ;

