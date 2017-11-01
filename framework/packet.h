/**
 * packet.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <stdint.h>
#include <endian.h>
#include <byteswap.h>

namespace framework
{

# if __BYTE_ORDER == __BIG_ENDIAN

#define hton_int16(x) (x)
#define hton_int32(x) (x)
#define hton_int64(x) (x)

#define ntoh_int16(x) (x)
#define ntoh_int32(x) (x)
#define ntoh_int64(x) (x)

#else

#define hton_int16(x) bswap_16(x)
#define hton_int32(x) bswap_32(x)
#define hton_int64(x) bswap_64(x)

#define ntoh_int16(x) bswap_16(x)
#define ntoh_int32(x) bswap_32(x)
#define ntoh_int64(x) bswap_64(x)

#endif

class packet
{
public:
    virtual ~packet() { } ;

    /*
     * @brief get type , implemented by concrete class
     * @return object type
     */
    virtual int get_type() = 0 ;

    /*
     * @brief encode object to buffer , implemented by concrete class
     * @param [in] buffer position
     * @param [in] max buffer size
     * @return actual encoded size , -1 on failure
     */
    virtual int encode(char* data,int max_size) = 0 ;


    /*
     * @brief decode object from buffer , implemented by concrete class
     * @param [in] buffer position
     * @param [in]  buffer size
     * @return actual decoded size , -1 on failure
     */
    virtual int decode(const char* data,int size) = 0 ;


    /*
     * @brief get needed buffer size of encoding , implemented by concrete class

     * @return needed buffer size , -1 on failure
     */
    virtual int encode_size() = 0 ;


    /*
     * @brief get needed size of decoding , implemented by concrete class
     * @param [in] buffer position
     * @param [in] buffer size
     * @return actual encoded size , -1 on failure
     */
    virtual int decode_size(const char* data,int size) = 0 ;
};

struct packet_info
{
    int size ;
    int type ;
    const char* data ;
}  ;


class packet_factory
{
public:
    virtual ~packet_factory() { } ;


    /*
     * @brief get packet info , implemented by concrete class
     * @param [in] buffer pointer
     * @param [in] buffer size
     * @param [out] packet info
     * @return 0 on success , -1 on failure
     */
    virtual int get_info(const char* data,int size,packet_info* pi) = 0 ;


    /*
     * @brief create packet  , implemented by concrete class
     * @param [in] packet data
     * @param [in] packet info
     * @return 0 on success , -1 on failure
     */
    virtual packet* create(const char* data,const packet_info* pi) = 0 ;

};


}

