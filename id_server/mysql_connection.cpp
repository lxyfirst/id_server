/**
 * MysqlConnection.cpp
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "mysql_connection.h"

static char db_escaped_table[256] = {
    1,/*0  */
    1,/*1  */
    1,/*2  */
    1,/*3  */
    1,/*4  */
    1,/*5  */
    1,/*6  */
    1,/*7  */
    1,/*8 */
    1,/*9    */
    1,/*10 */
    1,/*11 */
    1,/*12  */
    1,/*13  */
    1,/*14  */
    1,/*15  */
    1,/*16  */
    1,/*17  */
    1,/*18  */
    1,/*19  */
    1,/*20  */
    1,/*21  */
    1,/*22  */
    1,/*23  */
    1,/*24  */
    1,/*25  */
    1,/*26  */
    1,/*27 */
    1,/*28  */
    1,/*29  */
    1,/*30  */
    1,/*31  */
    1,/*32   */
    1,/*33 ! */
    1,/*34 " */
    1,/*35 # */
    0,/*36 $ */
    1,/*37 % */
    1,/*38 & */
    1,/*39 ' */
    0,/*40 ( */
    0,/*41 ) */
    0,/*42 * */
    0,/*43 + */
    0,/*44 , */
    0,/*45 - */
    0,/*46 . */
    0,/*47 / */
    0,/*48 0 */
    0,/*49 1 */
    0,/*50 2 */
    0,/*51 3 */
    0,/*52 4 */
    0,/*53 5 */
    0,/*54 6 */
    0,/*55 7 */
    0,/*56 8 */
    0,/*57 9 */
    0,/*58 : */
    0,/*59 ; */
    1,/*60 < */
    0,/*61 = */
    1,/*62 > */
    0,/*63 ? */
    0,/*64 @ */
    0,/*65 A */
    0,/*66 B */
    0,/*67 C */
    0,/*68 D */
    0,/*69 E */
    0,/*70 F */
    0,/*71 G */
    0,/*72 H */
    0,/*73 I */
    0,/*74 J */
    0,/*75 K */
    0,/*76 L */
    0,/*77 M */
    0,/*78 N */
    0,/*79 O */
    0,/*80 P */
    0,/*81 Q */
    0,/*82 R */
    0,/*83 S */
    0,/*84 T */
    0,/*85 U */
    0,/*86 V */
    0,/*87 W */
    0,/*88 X */
    0,/*89 Y */
    0,/*90 Z */
    0,/*91 [ */
    0,/*92 \ */
    0,/*93 ] */
    0,/*94 ^ */
    0,/*95 _ */
    0,/*96 ` */
    0,/*97 a */
    0,/*98 b */
    0,/*99 c */
    0,/*100 d */
    0,/*101 e */
    0,/*102 f */
    0,/*103 g */
    0,/*104 h */
    0,/*105 i */
    0,/*106 j */
    0,/*107 k */
    0,/*108 l */
    0,/*109 m */
    0,/*110 n */
    0,/*111 o */
    0,/*112 p */
    0,/*113 q */
    0,/*114 r */
    0,/*115 s */
    0,/*116 t */
    0,/*117 u */
    0,/*118 v */
    0,/*119 w */
    0,/*120 x */
    0,/*121 y */
    0,/*122 z */
    0,/*123 { */
    0,/*124 | */
    0,/*125 } */
    0,/*126 ~ */
    0,/*127 */

};

bool have_escape_char(const char* data,int size)
{
    for(int i = 0 ; i < size ; ++i)
    {
        if( data[i]>=0 &&db_escaped_table[(int)data[i]] ==1 ) return true ;
    }

    return false ;
}


int MysqlConnection::init(const char* host,const char* user,const char* password,int port)
{
    mysql_init(&m_mysql) ;
    int timeout = 5 ;
    mysql_options(&m_mysql,MYSQL_OPT_CONNECT_TIMEOUT,(const char*)&timeout) ;
    mysql_options(&m_mysql,MYSQL_OPT_READ_TIMEOUT,(const char*)&timeout) ;
    mysql_options(&m_mysql,MYSQL_OPT_WRITE_TIMEOUT,(const char*)&timeout) ;

    MYSQL* mysql = NULL;
    if(strchr(host,'/') == NULL )
    {
        mysql = mysql_real_connect(&m_mysql,host,
                user,password,NULL,port,NULL,CLIENT_FOUND_ROWS ) ;
    }
    else
    {
        mysql = mysql_real_connect(&m_mysql,NULL,
                user,password,NULL,0,host,CLIENT_FOUND_ROWS ) ;
    }

    m_connected = 1 ;

    return mysql ? 0 : -1 ;
}


void MysqlConnection::fini()
{
    if(m_connected)
    {
        free_result() ;
        mysql_close(&m_mysql) ;
        mysql_thread_end() ;
        m_connected = 0 ;
    }

}

int MysqlConnection::exec_format(const char* fmt,...)
{
    char buf_data[4094] = {0} ;
    va_list ap ;
    va_start(ap, fmt);
    int length = vsnprintf(buf_data,sizeof(buf_data),fmt,ap) ;
    va_end(ap);
    if(length < 1 ) return -1 ;

    return exec(buf_data) ;
}

int MysqlConnection::exec(const char* sql)
{
    if(m_connected == 0 ) return -1 ;
    if(mysql_query(&m_mysql,sql) != 0 ) return -1 ;

    MYSQL_RES* result = mysql_store_result(&m_mysql);
    if(result == NULL && (mysql_field_count(&m_mysql) != 0) )
    {
        return -1 ;
    }

    if(m_result != NULL ) mysql_free_result(m_result) ;
    m_result = result ;
    return 0 ;


}

