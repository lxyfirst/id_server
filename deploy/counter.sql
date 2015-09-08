create database if not exists id_counter;
use id_counter;

create table if not exists counter
(
    node_offset int not null default 0 comment 'node offset' ,
    counter int not null default 0 comment 'counter' ,
    update_time int not null default 0 comment 'counter last update time' ,
    rule_name varchar(64) not null default '' comment 'rule name' ,
    app_name varchar(64) not null default '' comment 'app name' ,
    primary key(node_offset,rule_name,app_name) 

) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='counter table' ;

