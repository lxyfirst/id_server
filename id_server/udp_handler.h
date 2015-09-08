/*
 * udp_handler.h
 * Author: lixingyi (lxyfirst@163.com)
 */

#ifndef UDP_HANDLER_H_
#define UDP_HANDLER_H_
#include <string>
#include "framework/udp_data_handler.h"

using std::string ;
using framework::sa_in_t ;

class UdpHandler: public framework::udp_data_handler
{
public:
	UdpHandler();
	virtual ~UdpHandler();

	int process_packet(const framework::udp_packet* p);

	int send_response(const sa_in_t& to_addr,int code,const char* message,const char* data="");
protected:
	/*
	 * @brief process create id request
	 */
	int process_action_create(const string& rule_name,const string& app_name,const string& salt,const sa_in_t& from_addr);
	/*
	 * @brief process monitor request
	 */
	int process_action_monitor(const string& rule_name,const string& app_name,const sa_in_t& from_addr);

};

#endif /* UDP_HANDLER_H_ */
