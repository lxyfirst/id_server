#!/bin/env bash

############### function ############


server_pid()
{
    #ps -e -o pid,args |grep -v grep |grep "$1" | awk '{print $1}'
     pgrep -fx "$1" | awk '{printf("%d ",$1);}'
}

start_server()
{
    echo "running [$1]"
    $1 
    sleep 1
    
}


stop_server()
{
    pid=$(server_pid "$1")
    if  [ ${#pid} -gt 0 ]  ;then
        echo "stopping [$pid $1]" 
        kill -USR2 $pid
        sleep 1
    else
       echo "cannot find [$1]"
    fi
}

reload_server()
{
    pid=$(server_pid "$1")
    if  [ ${#pid} -gt 0 ]  ;then
        echo "reloading [$pid $1]" 
        kill -USR1 $pid
        sleep 1
    else
       echo "cannot find [$1]"
    fi
}



############## main ################

if [ $# -lt 3 ] ; then 
    echo "usage:$0 start|stop|reload  id_server {node_id}"
    exit
fi

command="$1"
server="$2"
node_id="$3"

cd $(dirname $0)
base_dir=$(pwd)
echo "change dir to [$base_dir] "
cd $base_dir

server_cmd=""

case "$server" in
    id_server)
        server_cmd="./id_server.bin -c id_server_${node_id}.xml -w $base_dir -d"
    ;;
   *)
        echo "invalid server, quit"
        exit 1
    ;;

esac


case "$command" in
    start)
        start_server  "$server_cmd"
    ;;
    stop)
        stop_server "$server_cmd"
    ;;
    reload)
        reload_server "$server_cmd"
    ;;
    *)
        echo "invalid command , quit"
        exit 1
    ;;
esac
