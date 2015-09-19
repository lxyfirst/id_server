
### 系统概述 
    服务器的主要功能是根据预定义的规则和自定义的命名空间创建一定范围和周期内唯一ID 。 
    为了保证高可用性，一般以集群方式运行，每个服务器是独立和对等的节点，即master-master模式。
    客户端可以使用集群中任意选择一个服务器节点发起请求，失败则重试下一个。

### 系统实现 
    使用c++实现，运行在linux下,依赖mysqlclient库、lua库和gperftools库，
    采用多线程异步模型，主线程处理请求，数据库线程组负责异步写数据库。
    主线程和数据库线程采用eventfd + lockfree queue方式通信。
    数据的持久化存储采用批量+预保存模式，减少数据库压力，保障数据落地的可靠性。

### 性能
    性能测试做的比较简单，以php为客户端进行测试。
    服务器为4核E5-2620 v2 @ 2.10GHz虚拟机。
    客户端为4个php并发进程，每个进程不停发送20万个请求，测试结果：
    total:200000 fail:0 min:0.000214 max:0.087330 avg:0.000393
    total:200000 fail:0 min:0.000215 max:0.087129 avg:0.000391
    total:200000 fail:0 min:0.000221 max:0.087252 avg:0.000391
    total:200000 fail:0 min:0.000218 max:0.087484 avg:0.000391
    说明  min : 最小耗时(秒) max : 最大耗时(秒) avg : 平均耗时(秒)
    服务器TPS达到1万/秒时，平均延迟在0.3毫秒。

### 配置项 
    日志配置 
    <log prefix="id_server" level="5" />

    服务端口配置
    <listen host="0.0.0.0" port="1200" />

    数据库配置
    <database thread_count="4" queue_size="100000" host="127.0.0.1" port="3306" 
        user="root" password="" dbname="id_counter" /> 

    ID规则配置，规则内容由lua脚本定义，修改配置后reload即可。 
    step表示ID自增的步长，也可以理解为同时运行的服务器数量 。 
    offset表示ID自增的偏移量，也可以理解为每个服务器的ID，从0开始，不能超过step,每个服务器不能重复 。
    <rules step="2"  offset="1" >
        <rule name="task" lua_file="task.lua" batch_save="10" />
    </rules>

### ID规则定义 
    具体规则有lua脚本定义，修改脚本后需要reload生效，需要实现4个函数
    min_counter :   计数器最小值
    max_counter :   计数器最大值
    reset_seconds : 计数器重置周期
    create_id : 根据计数器、自定义参数和时间参数创建ID。
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
    seq : 自定义参数，可选项，原样返回
  
  
    创建ID请求:  {"action":"get","rule_name":"o2o","app_name":"test"}  
    响应：{"code":0,"message":"success","data":"505140001"}
    
    监控请求：{"action":"monitor","rule_name":"o2o","app_name":"test"}   
    响应：{"code":0,"message":"ok","data":{"counter":3,"node_offset":1}}

### 编译
    $make release
    $make install 
    make install会将部署所需的文件拷贝到deploy目录下 。

### 创建数据库
    $mysql < deploy/counter.sql

### 启动服务器
    $deploy/server_ctl.sh start id_server 1
    修改配置后使用控制脚本server_ctl.sh控制系统的启动和关闭。

    系统控制脚本用法 ,可根据需要修改
    server_ctl.sh start|stop|reload id_server {server_id}
    载入的配置文件为 id_server_{server_id}.xml
    修改配置后reload即可，为保证数据安全，只支持规则列表和日志的reload，其他配置修改后需要重启。

