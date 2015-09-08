<?php


function create_id($rule_name,$app_name,$salt='')
{
    $server_list = array( "udp://127.0.0.1:1200" ,  "udp://127.0.0.1:1200" ) ;
    shuffle($server_list) ;
    $new_id = "" ;
    foreach($server_list as $server_info)
    {
        $sock = fsockopen($server_info) ;
        stream_set_timeout($sock,2,0) ;
        fwrite($sock,json_encode(array("action"=>"create","rule_name"=>$rule_name,"app_name"=>$app_name,"salt"=>$salt)) ) ;
        $data = fread($sock,256) ;
        fclose($sock) ;
        $result = json_decode($data,true) ;
        if ( is_array($result)  && $result["code"] ==0 )
        {   
            $new_id = $result["data"] ;
            break ;
        }
    }

    return $new_id ;

}


$id = create_id("task","test","xx") ;

echo "$id \n" ;

        

?>
