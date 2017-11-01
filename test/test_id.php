<?php


function do_request($request)
{
    $server_list = array( "udp://127.0.0.1:1200"  ) ;
    //shuffle($server_list) ;
    foreach($server_list as $server_info)
    {
        $sock = fsockopen($server_info) ;
        stream_set_timeout($sock,2,0) ;
        fwrite($sock,json_encode($request) ) ;
        $data = fread($sock,256) ;
        fclose($sock) ;
        $result = json_decode($data,true) ;
        if ( is_array($result)  && $result["code"] ==0 )
        {   
            return $result ;
        }
    }

    return NULL;
}

function create_id($rule_name,$app_name,$salt='')
{
    $request = array("action"=>"create","rule_name"=>$rule_name,"app_name"=>$app_name,"salt"=>$salt) ;
    $response = do_request($request) ;
    //var_dump($response) ;
    $new_id = is_array($response) ?  $response["data"] : "" ;
    return $new_id ;

}


function bench_test($count)
{
    printf("pid:%d running...\n",getmypid()) ;

    $min_time = 1000000.0 ;
    $max_time = 0.0000001;
    $fail = 0 ;
    $total_time = 0.0000001;
    for($i=0 ; $i < $count ; ++$i)
    {
        $begin_time = microtime() ;
        $id = create_id("bench",strval(getmypid()),"xx") ;
        if(empty($id) || strlen($id) < 1 )
        {
            ++$fail ;
        }

        $consume_time = microtime() - $begin_time ;
        if($consume_time >0.0000001)
        {
            if($min_time > $consume_time) $min_time = $consume_time ;
            if($max_time < $consume_time) $max_time = $consume_time ;
            $total_time += $consume_time ;
        }
    }

    printf("pid:%d total:%d fail:%d min:%f max:%f avg:%f\n",
        getmypid(),$count,$fail,$min_time,$max_time,$total_time/$count) ;

}
        
function bench_process($process,$count)
{

    for($i = 0 ; $i < $process ; ++$i)
    {

        $pid = pcntl_fork() ;
        if($pid ==0)
        {
            sleep(1) ;
            bench_test($count) ;
            exit(0) ;
        }
    }

    exit(0) ;
}


$id = create_id("order_id","test","xx") ;
echo $id ;

//bench_process(4,200000) ;

?>
