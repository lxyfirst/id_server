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
