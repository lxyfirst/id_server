
### 系统概述 
    服务器的主要功能是根据预定义的规则和自定义的命名空间创建一定范围和周期内唯一ID 。 
    为了保证高可用性，一般以集群方式运行，每个服务器是独立和对等的节点，即master-master模式。
    客户端可以使用集群中任意选择一个服务器节点发起请求，失败则重试下一个。

### 系统实现 
    使用c++实现，依赖mysqlclient库、lua库和gperftools库，采用多线程异步模型，主线程处理请求，数据库线程组负责写数据库。

### 配置项 
#### 日志配置 
    <log prefix="id_server" level="5" />

#### 服务端口配置
    <listen host="10.8.64.23" port="1200" />

#### 数据库配置
    <database thread_count="4" host="10.8.72.11" port="3306" user="id_counter" password="ILtuVb4EN" dbname="id_counter" /> 

    ID规则配置，规则内容由lua脚本定义，修改配置后reload即可。 
    step表示ID自增的步长，也可以理解为同时运行的服务器数量 。 
    offset表示ID自增的偏移量，也可以理解为每个服务器的ID，从0开始，不能超过step,每个服务器不能重复 。
    <rules step="2"  offset="1" >
        <rule name="task" lua_file="task.lua" />
    </rules>

### ID规则定义 
    具体规则有lua脚本定义，需要实现4个函数
    min_counter :   计数器最小值
    max_counter :   计数器最大值
    reset_seconds : 计数器重置周期
    create_id : 根据计数器和时间参数创建ID，当请求中包含自定义格式时，优先使用自定义格式，不再使用本函数。
    参见：
    function min_counter()
        return 0
    end
    function max_counter()
        return 9999
    end
    function reset_seconds()
        return 86400
    end
    function create_id(counter,now,salt)
        local seq = counter:generate_counter()
        local new_id = string.format("%01d%02d%02d%04d",now:year()%10 ,now:month(),now:day(),seq)
        return new_id
    end

### 接口 
    采用udp协议，数据格式为json ，字段定义：
    action: 请求类型 get： 创建ID ，  monitor：监控
    rule_name: 规则名字， 由服务端定义
    app_name : 应用名或命名空间 ， 客户端自定义，rule_name和app_name一起决定生成ID的唯一性
    salt :  自定义参数 ，可选项 ， 
  
  
    创建ID请求:  {"action":"get","rule_name":"o2o","app_name":"test"}  响应：{"code":0,"message":"success","data":"505140001"}
    监控请求：{"action":"monitor","rule_name":"o2o","app_name":"test"}   响应：{"code":0,"message":"ok","data":{"counter":3,"node_offset":1}}

### 系统控制脚本 
    server_ctl.sh star|stop|reload id_server {server_id}
    修改配置后reload即可，为保证数据安全，只支持规则列表和日志的reload，其他配置修改后需要重启。

