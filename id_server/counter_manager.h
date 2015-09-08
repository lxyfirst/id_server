/*
 * counter_manager.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#ifndef COUNTER_MANAGER_H_
#define COUNTER_MANAGER_H_

#include <string>
#include <tr1/unordered_map>

using std::string ;

struct CounterData
{
	string rule_name ;
	string app_name ;
    int node_offset ;
	int counter ;
	int update_time ;
};

struct RuleConfig ;

class Counter
{
public:
	/*
	 * @brief load data from database
	 */
	void load(const CounterData& data) ;

	/*
	 * @brief init data
	 */
	void init(const string& rule_name,const string& app_name,const RuleConfig& rule_config);

	/*
	 * @brief create auto increment counter
	 * @return new counter
	 */
	int generate_counter() ;

	/*
	 * @brief get timestamp never rollback
	 * @return timestamp
	 */
	int generate_time() ;

	/*
	 * @brief save data to database
	 */
	void async_save() ;

	const CounterData& data() { return m_data ; } ;
private:
	CounterData m_data ;

};

std::string& get_counter_key(string& data,const char* rule_name,const char* app_name) ;


class CounterManager
{
public:
	typedef std::tr1::unordered_map<string,Counter> CounterContainer ;

	Counter* get_counter(const string& rule_name,const string& app_name);

	Counter* create_counter(const string& rule_name,const string& app_name);

	/*
	 * @brief load counter data from database
	 */
	Counter* load_counter(const CounterData& data) ;

public:
	CounterManager();
	~CounterManager();


private:
	CounterManager(const CounterManager& o) ;
	CounterManager& operator=(const CounterManager& o) ;

private:
	CounterContainer m_counter_list ;

};

#endif /*  */
