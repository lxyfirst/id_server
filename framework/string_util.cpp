
/*
 *
 * Author : lixingyi (lxyfirst@163.com)
 */

#include <ctype.h>

#include "string_util.h" 


namespace framework
{


static char HEX_STR[] ="0123456789abcdef" ;
static int BIN_STR[] =
{
    -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,
    0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,
    0x8,0x9,-1,-1,-1,-1,-1,-1,
    -1,0xA,0xB,0xC,0xD,0xE,0xF,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,
    -1,0xA,0xB,0xC,0xD,0xE,0xF,-1,
} ;


int bin2hex(char* hex,const unsigned char* bin,int size)
{

    for(int i = 0 ; i < size ; ++i)
    {
        hex[i*2] = HEX_STR[(bin[i] & 0xf0) >> 4] ;
        hex[i*2+1] = HEX_STR[(bin[i] & 0xf)] ;

    }
    
    return size*2 ;

}

int hex2bin(unsigned char* bin,const char* hex,int size)
{
    
    for(int i = 0 ; i < size ; i+=2 )
    {
        int high = hex[i] ;
        int low = hex[i+1] ;    
        if( !isxdigit(high) || (!isxdigit(low)) ) return -1 ;
        bin[i/2]=BIN_STR[high] << 4 | BIN_STR[low] ;
        
    }

    return size/2 ;

}


int base64_encode(unsigned char* dst,const unsigned char* src,int src_size)
{
    static char basis[] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned char* dst_base = dst ;
    while(src_size > 2)
    {
        *dst++ = basis[(src[0] >> 2) & 0x3f];
        *dst++ = basis[((src[0] & 3) << 4) | (src[1] >> 4)];
        *dst++ = basis[((src[1] & 0x0f) << 2) | (src[2] >> 6)];
        *dst++ = basis[src[2] & 0x3f];

        src += 3;
        src_size -= 3;
    }

    if(src_size)
    {
        *dst++ = basis[(src[0] >> 2) & 0x3f];

        if (src_size == 1)
        {
            *dst++ = basis[(src[0] & 3) << 4];
            *dst++ = '=';

        }
        else
        {
            *dst++ = basis[((src[0] & 3) << 4) | (src[1] >> 4)];
            *dst++ = basis[(src[1] & 0x0f) << 2];
        }

        *dst++ = '=';
    }

    return dst - dst_base ;

}

int base64_decode(unsigned char* dst,const unsigned char* src,int src_size)
{
    static char basis[] =
    {
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77, 77, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
        77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 77,
        77, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 77, 77, 77, 77, 77,

        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
    };

    int len =0 ;
    unsigned char* dst_base = dst ;
    for (len = 0; len < src_size ; ++len)
    {
        if (src[len] == '=')  break ;

        if (basis[src[len] ] == 77)  return -1 ;
    }

    if (len % 4 == 1)  return -1 ;


    while (len > 3)
    {
        *dst++ = (unsigned char) (basis[src[0]] << 2 | basis[src[1]] >> 4);
        *dst++ = (unsigned char) (basis[src[1]] << 4 | basis[src[2]] >> 2);
        *dst++ = (unsigned char) (basis[src[2]] << 6 | basis[src[3]]);

        src += 4;
        len -= 4;
    }

    if (len > 1)
    {
        *dst++ = (unsigned char) (basis[src[0]] << 2 | basis[src[1]] >> 4);
    }

    if (len > 2)
    {
        *dst++ = (unsigned char) (basis[src[1]] << 4 | basis[src[2]] >> 2);
    }

    return dst - dst_base ;

}



int sql_escape_string(char* buf,int size,const char* data,int data_size)
{
    if(size < data_size ) return -1 ;
    static char map[40] = { 0,0,0,0,0,0,0,0, 1,1,1,1,0,1,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,1,0,0,0,0,1 } ;
    char* cur = buf ;
    const char* src_end = data + data_size ;
    for( ; (data != src_end) && (*data) ; ++data)
    {
        if(cur - buf + 2 > size) return -1 ;
        if( ( (*data >0) && (*data < sizeof(map) ) && map[(int)*data] ) || *data == 92 )
        {
            *cur++ = '\\' ;
        }
        *cur++ = *data ;
    }
    return cur - buf ;
}




int split(string_vector& dst,const char* src,int size,char sep) 
{
    int item_count = 0 ;
    const char* end = src + size ;
    const char* item_begin = NULL ;
    const char* item_end = NULL ;
    while(1)
    {
        for( ;src != end && *src == sep ; ++src) ;
        if( src == end ) break ;
        else item_begin = src ;

        for(++src ;src != end && *src != sep ; ++src) ;
        item_end = src ;

        ++item_count ;
        if(dst.size() < item_count ) dst.resize(item_count*2) ;
        dst[item_count -1].assign(item_begin,item_end - item_begin) ;

        if( src == end ) break ;
        src = item_end +1 ;

    } 

    dst.resize(item_count) ;

    return item_count ;

}

int split(string_vector& dst,const char* src,int size,const char* sep,bool ignore_empty) 
{
    int sep_size = strlen(sep) ;
    if ( sep_size < 1 || sep_size > size ) return 0 ;
    
    std::string item ;
    const char* end = src + size ;
    dst.clear() ;
    while(end > src)    
    {
        const char* match = strstr(src,sep) ;
        if (match == NULL ) match = end ;
        if ( match > src || !ignore_empty )
        {
            item.assign(src,match - src) ;
            dst.push_back(item) ;
        }
        src = match + sep_size ;
    }

    return dst.size() ;
}


std::string& int2str(std::string& str,long int value)
{
    char buf[64] = {0} ;
    snprintf(buf,sizeof(buf),"%ld",value);
    str.assign(buf) ;
    return str ;
}


int hash(const char *str,int size)
{
    // BKDR Hash Function
    static const int seed = 131;
    unsigned int hash = 0;
    const char* end = str + size ;
    while (str != end)
    {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}


void md5_init(md5_context *ctx)
{
    ctx->a = 0x67452301;
    ctx->b = 0xefcdab89;
    ctx->c = 0x98badcfe;
    ctx->d = 0x10325476;

    ctx->bytes = 0;
}

static const unsigned char *md5_body(md5_context *ctx, const unsigned char *data,size_t size)
{
#define F(x, y, z)  ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)  ((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z)  ((x) ^ (y) ^ (z))
#define I(x, y, z)  ((y) ^ ((x) | ~(z)))

#define STEP(f, a, b, c, d, x, t, s)                                          \
    (a) += f((b), (c), (d)) + (x) + (t);                                      \
    (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s))));                \
    (a) += (b)

#define SET(n)      (*(uint32_t *) &p[n * 4])
#define GET(n)      (*(uint32_t *) &p[n * 4])


    uint32_t       a, b, c, d;
    uint32_t       saved_a, saved_b, saved_c, saved_d;
    const unsigned char  *p;
    p = data;

    a = ctx->a;
    b = ctx->b;
    c = ctx->c;
    d = ctx->d;

    do
    {
        saved_a = a;
        saved_b = b;
        saved_c = c;
        saved_d = d;

        /* Round 1 */

        STEP(F, a, b, c, d, SET(0),  0xd76aa478, 7);
        STEP(F, d, a, b, c, SET(1),  0xe8c7b756, 12);
        STEP(F, c, d, a, b, SET(2),  0x242070db, 17);
        STEP(F, b, c, d, a, SET(3),  0xc1bdceee, 22);
        STEP(F, a, b, c, d, SET(4),  0xf57c0faf, 7);
        STEP(F, d, a, b, c, SET(5),  0x4787c62a, 12);
        STEP(F, c, d, a, b, SET(6),  0xa8304613, 17);
        STEP(F, b, c, d, a, SET(7),  0xfd469501, 22);
        STEP(F, a, b, c, d, SET(8),  0x698098d8, 7);
        STEP(F, d, a, b, c, SET(9),  0x8b44f7af, 12);
        STEP(F, c, d, a, b, SET(10), 0xffff5bb1, 17);
        STEP(F, b, c, d, a, SET(11), 0x895cd7be, 22);
        STEP(F, a, b, c, d, SET(12), 0x6b901122, 7);
        STEP(F, d, a, b, c, SET(13), 0xfd987193, 12);
        STEP(F, c, d, a, b, SET(14), 0xa679438e, 17);
        STEP(F, b, c, d, a, SET(15), 0x49b40821, 22);

        /* Round 2 */

        STEP(G, a, b, c, d, GET(1),  0xf61e2562, 5);
        STEP(G, d, a, b, c, GET(6),  0xc040b340, 9);
        STEP(G, c, d, a, b, GET(11), 0x265e5a51, 14);
        STEP(G, b, c, d, a, GET(0),  0xe9b6c7aa, 20);
        STEP(G, a, b, c, d, GET(5),  0xd62f105d, 5);
        STEP(G, d, a, b, c, GET(10), 0x02441453, 9);
        STEP(G, c, d, a, b, GET(15), 0xd8a1e681, 14);
        STEP(G, b, c, d, a, GET(4),  0xe7d3fbc8, 20);
        STEP(G, a, b, c, d, GET(9),  0x21e1cde6, 5);
        STEP(G, d, a, b, c, GET(14), 0xc33707d6, 9);
        STEP(G, c, d, a, b, GET(3),  0xf4d50d87, 14);
        STEP(G, b, c, d, a, GET(8),  0x455a14ed, 20);
        STEP(G, a, b, c, d, GET(13), 0xa9e3e905, 5);
        STEP(G, d, a, b, c, GET(2),  0xfcefa3f8, 9);
        STEP(G, c, d, a, b, GET(7),  0x676f02d9, 14);
        STEP(G, b, c, d, a, GET(12), 0x8d2a4c8a, 20);

        /* Round 3 */

        STEP(H, a, b, c, d, GET(5),  0xfffa3942, 4);
        STEP(H, d, a, b, c, GET(8),  0x8771f681, 11);
        STEP(H, c, d, a, b, GET(11), 0x6d9d6122, 16);
        STEP(H, b, c, d, a, GET(14), 0xfde5380c, 23);
        STEP(H, a, b, c, d, GET(1),  0xa4beea44, 4);
        STEP(H, d, a, b, c, GET(4),  0x4bdecfa9, 11);
        STEP(H, c, d, a, b, GET(7),  0xf6bb4b60, 16);
        STEP(H, b, c, d, a, GET(10), 0xbebfbc70, 23);
        STEP(H, a, b, c, d, GET(13), 0x289b7ec6, 4);
        STEP(H, d, a, b, c, GET(0),  0xeaa127fa, 11);
        STEP(H, c, d, a, b, GET(3),  0xd4ef3085, 16);
        STEP(H, b, c, d, a, GET(6),  0x04881d05, 23);
        STEP(H, a, b, c, d, GET(9),  0xd9d4d039, 4);
        STEP(H, d, a, b, c, GET(12), 0xe6db99e5, 11);
        STEP(H, c, d, a, b, GET(15), 0x1fa27cf8, 16);
        STEP(H, b, c, d, a, GET(2),  0xc4ac5665, 23);


        /* Round 4 */

        STEP(I, a, b, c, d, GET(0),  0xf4292244, 6);
        STEP(I, d, a, b, c, GET(7),  0x432aff97, 10);
        STEP(I, c, d, a, b, GET(14), 0xab9423a7, 15);
        STEP(I, b, c, d, a, GET(5),  0xfc93a039, 21);
        STEP(I, a, b, c, d, GET(12), 0x655b59c3, 6);
        STEP(I, d, a, b, c, GET(3),  0x8f0ccc92, 10);
        STEP(I, c, d, a, b, GET(10), 0xffeff47d, 15);
        STEP(I, b, c, d, a, GET(1),  0x85845dd1, 21);
        STEP(I, a, b, c, d, GET(8),  0x6fa87e4f, 6);
        STEP(I, d, a, b, c, GET(15), 0xfe2ce6e0, 10);
        STEP(I, c, d, a, b, GET(6),  0xa3014314, 15);
        STEP(I, b, c, d, a, GET(13), 0x4e0811a1, 21);
        STEP(I, a, b, c, d, GET(4),  0xf7537e82, 6);
        STEP(I, d, a, b, c, GET(11), 0xbd3af235, 10);
        STEP(I, c, d, a, b, GET(2),  0x2ad7d2bb, 15);
        STEP(I, b, c, d, a, GET(9),  0xeb86d391, 21);

        a += saved_a;
        b += saved_b;
        c += saved_c;
        d += saved_d;

        p += 64;

    } while (size -= 64);

    ctx->a = a;
    ctx->b = b;
    ctx->c = c;
    ctx->d = d;

    return p;

}

void md5_update(md5_context *ctx, const void *data, size_t size)
{
    size_t  used, free;

    used = (size_t) (ctx->bytes & 0x3f);
    ctx->bytes += size;

    if (used)
    {
        free = 64 - used;

        if (size < free)
        {
            memcpy(&ctx->buffer[used], data, size);
            return;
        }

        memcpy(&ctx->buffer[used], data, free);
        data = (unsigned char *) data + free;
        size -= free;
        md5_body(ctx, ctx->buffer, 64);
    }

    if (size >= 64)
    {
        data = md5_body(ctx, (unsigned char *)data, size & ~(size_t) 0x3f);
        size &= 0x3f;
    }

    memcpy(ctx->buffer, data, size);

}

void md5_final(unsigned char result[16], md5_context *ctx)
{
    size_t  used, free;

    used = (size_t) (ctx->bytes & 0x3f);

    ctx->buffer[used++] = 0x80;

    free = 64 - used;

    if (free < 8)
    {
        memset(&ctx->buffer[used],0, free);
        md5_body(ctx, ctx->buffer, 64);
        used = 0;
        free = 64;
    }

    memset(&ctx->buffer[used],0, free - 8);

    ctx->bytes <<= 3;
    ctx->buffer[56] = (unsigned char) ctx->bytes;
    ctx->buffer[57] = (unsigned char) (ctx->bytes >> 8);
    ctx->buffer[58] = (unsigned char) (ctx->bytes >> 16);
    ctx->buffer[59] = (unsigned char) (ctx->bytes >> 24);
    ctx->buffer[60] = (unsigned char) (ctx->bytes >> 32);
    ctx->buffer[61] = (unsigned char) (ctx->bytes >> 40);
    ctx->buffer[62] = (unsigned char) (ctx->bytes >> 48);
    ctx->buffer[63] = (unsigned char) (ctx->bytes >> 56);

    md5_body(ctx, ctx->buffer, 64);

    result[0] = (unsigned char) ctx->a;
    result[1] = (unsigned char) (ctx->a >> 8);
    result[2] = (unsigned char) (ctx->a >> 16);
    result[3] = (unsigned char) (ctx->a >> 24);
    result[4] = (unsigned char) ctx->b;
    result[5] = (unsigned char) (ctx->b >> 8);
    result[6] = (unsigned char) (ctx->b >> 16);
    result[7] = (unsigned char) (ctx->b >> 24);
    result[8] = (unsigned char) ctx->c;
    result[9] = (unsigned char) (ctx->c >> 8);
    result[10] = (unsigned char) (ctx->c >> 16);
    result[11] = (unsigned char) (ctx->c >> 24);
    result[12] = (unsigned char) ctx->d;
    result[13] = (unsigned char) (ctx->d >> 8);
    result[14] = (unsigned char) (ctx->d >> 16);
    result[15] = (unsigned char) (ctx->d >> 24);

    memset(ctx,0, sizeof(*ctx));


}

void md5(std::string& digest,const void *data, int size)
{
    unsigned char result[16] ;
    md5_context ctx ;
    md5_init(&ctx) ;
    md5_update(&ctx,data,size) ;
    md5_final(result,&ctx) ;
    char hex_result[40] = {0} ;
    bin2hex(hex_result,result,sizeof(result)) ;   
    digest.assign(hex_result) ;

}


}

