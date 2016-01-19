/**
 * aliased_streambuf.h
 *
 *      Author: lixingyi (lxyfirst@163.com)
 */

#pragma once

#include <streambuf>

namespace framework
{

/**
 * streambuf wrapper for protobuf
 */
class aliased_streambuf : public std::basic_streambuf<char>
{
public:
    typedef std::basic_streambuf<char> parent_type ;
public:
    aliased_streambuf(char* buf,int size)
    {
        this->setp(buf,buf + size) ;

        this->setg(buf,buf,buf + size ) ;
    }

protected:
    virtual int_type overflow(int_type c )
    {
        if(this->pptr() < this->epptr() )
        {

             return traits_type::to_int_type(*this->pptr());
        }

        return traits_type::eof() ;
    }

    virtual int_type underflow()
    {
        if (this->gptr() < this->egptr())
        {
             return traits_type::to_int_type(*this->gptr());
        }

        return traits_type::eof();

    }

    virtual int sync()
    {
        return 0 ;
    }

    virtual parent_type* setbuf( char * s, std::streamsize n )
    {
        return this ;
    }

    virtual pos_type seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode mode)
    {
        pos_type ret =  pos_type(off_type(-1));
        off_type newoffi = 0 , newoffo = 0 ;
        if(way == std::ios_base::cur)
        {
            if(mode & std::ios_base::in)
            {
                newoffi = this->gptr() - this->eback() + off ;
            }

            if(mode & std::ios_base::out)
            {
                newoffi = this->pptr() - this->pbase() + off ;
            }
        }
        else if(way == std::ios_base::end)
        {
            if(mode & std::ios_base::in)
            {
                newoffi = this->egptr() - this->eback() + off ;
            }

            if(mode & std::ios_base::out)
            {
                newoffo = this->epptr() - this->pbase() + off ;
            }

        }

        if( (mode & std::ios_base::in) && newoffi >= 0 && newoffi < this->egptr() - this->eback() )
        {
            this->gbump( this->eback() + newoffi - this->gptr()) ;
            ret = pos_type(newoffi) ;
        }

        if((mode & std::ios_base::out) && newoffo >=0 && newoffo < this->epptr() - this->pbase() )
        {
            this->pbump( this->pbase() + newoffi - this->pptr()) ;
            ret = pos_type(newoffi) ;
        }


        return ret ;
    }

    virtual pos_type seekpos(pos_type, std::ios_base::openmode )
    {
        return -1 ;
    }

    virtual std::streamsize showmanyc()
    {
        return 0 ;
    }



/*
    virtual pos_type seekpos(pos_type, ios_base::openmode)
    {
        return -1 ;
    }

*/



} ;

}

