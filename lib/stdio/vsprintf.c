/*************************************************************************
*                        -=-=-= vsprintf.c =-=-=-
*
* THIS FILE WAS CREATED FOR ELEC4710 - REALTIME SYSTEMS
* MAJOR PROJECT ASSIGNMENT
*
* This file contains standard ISO C library functions. They were not 
* written by me nor do I take credit for them. They are included so 
* hopefully this OS will be able to compile ANSI C programs.
*
* Compiler: gcc
* 
* Note: MUCH OF THIS CODE IS BASED ON GPL CODE AVAILABLE FROM THE LIBC 
*       LIBRARY AND THE LINUX KERNEL
**************************************************************************/
#include <os/config.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern int isdigit(char c);

static int skip_atoi( const char **s )
{
    int i = 0;

    while ( isdigit( **s ) )
    {
        i = i * 10 + * ( (*s)++ ) - '0';
    }

    return i;
}


#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */

#define do_div( n ,base ) ({ \
    int __res; \
    __asm__("divl %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
    __res; })


static char *number( char *str, int num, int base, int size, int precision, int type )
{
    char c, sign, tmp[ 140 ];
    const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i;

    if ( type & LARGE )
    {
        digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    }
    if ( type & LEFT )
    {
        type &= ~ZEROPAD;
    }
    if ( base < 2 || base > 36 )
    {
        return 0;
    }
    c = ( type & ZEROPAD ) ? '0' : ' ';
    sign  = 0;
    if ( type & SIGN )
    {
        if ( num < 0 )
        {
            sign = '-';
            num = -num;
            size--;
        }
        else
            if ( type & PLUS )
            {
                sign = '+';
                size--;
            }
            else
                if ( type & SPACE )
                {
                    sign = ' ';
                    size--;
                }
    }
    if ( type & SPECIAL )
    {
        if ( base == 16 )
        {
            size -= 2;
        }
        else
            if ( base == 8 )
            {
                size--;
            }
    }
    i = 0;
    if ( num == 0 )
    {
        tmp[ i++ ] = '0';
    }
    else
        while ( num != 0 )
        {
            tmp[ i++ ] = digits[ do_div( num, base ) ];
        }
        if ( i > precision )
        {
            precision = i;
        }
        size -= precision;
        if ( !( type & ( ZEROPAD + LEFT ) ) )
        {
            while ( size-- > 0 )
            {
                *str++ = ' ';
            }
        }
        if ( sign )
        {
            *str++ = sign;
        }
        if ( type & SPECIAL )
        {
            if ( base == 8 )
            {
                *str++ = '0';
            }
            else
                if ( base == 16 )
                {
                    *str++ = '0';
                    *str++ = digits[ 33 ];
                }
        }
        if ( !( type & LEFT ) )
        {
            while ( size-- > 0 )
            {
                *str++ = c;
            }
        }
        while ( i < precision-- )
        {
            *str++ = '0';
        }
        while ( i-- > 0 )
        {
            *str++ = tmp[ i ];
        }
        while ( size-- > 0 )
        {
            *str++ = ' ';
        }

        return str;
}

int vsprintf( char *buf, const char *fmt, va_list args )
{
    int len;
    //unsigned long long num;
    int num;
    int i, base;
    char * str;
    const char *s;

    int flags;		/* flags to number() */

    int field_width;	/* width of output field */
    int precision;		/* min. # of digits for integers; max
                        number of chars for from string */
    int qualifier;		/* 'h', 'l', or 'L' for integer fields */

    for ( str = buf; *fmt; ++fmt )
    {
        if ( *fmt != '%' )
        {
            *str++ = *fmt;
            continue;
        }

        /* 
        * process flags 
        */
        flags = 0;
repeat:
        ++fmt;		/* this also skips first '%' */

        switch (*fmt)
        {
        case '-':
            flags |= LEFT;
            goto repeat;
        case '+':
            flags |= PLUS;
            goto repeat;
        case ' ':
            flags |= SPACE;
            goto repeat;
        case '#':
            flags |= SPECIAL;
            goto repeat;
        case '0':
            flags |= ZEROPAD;
            goto repeat;
        }

        /* 
        * get field width 
        */
        field_width = -1;
        if ( isdigit( *fmt ) )
        {
            field_width = skip_atoi( &fmt );
        }
        else
            if ( *fmt == '*' )
            {
                /* 
                * it's the next argument 
                */
                ++fmt;
                field_width = va_arg( args, int );
                if ( field_width < 0 )
                {
                    field_width = -field_width;
                    flags |= LEFT;
                }
            }

            /* 
            * get the precision 
            */
            precision = -1;
            if ( *fmt == '.' )
            {
                ++fmt;	
                if ( isdigit( *fmt ) )
                {
                    precision = skip_atoi( &fmt );
                }
                else
                    if ( *fmt == '*' )
                    {
                        /* 
                        * it's the next argument 
                        */
                        precision = va_arg( args, int );
                    }
                    if ( precision < 0 )
                    {
                        precision = 0;
                    }
            }

            /* 
            * get the conversion qualifier 
            */
            qualifier = -1;
            if ( *fmt == 'h' || *fmt == 'l' || *fmt == 'L')
            {
                qualifier = *fmt;
                ++fmt;
            }

            base = 10;

            switch ( *fmt )
            {
            case 'c':
                if ( !( flags & LEFT ) )
                {
                    while ( --field_width > 0 )
                    {
                        *str++ = ' ';
                    }
                    *str++ = ( uchar ) va_arg( args, int );
                    while ( --field_width > 0 )
                    {
                        *str++ = ' ';
                    }
                }
                continue;
            case 's':
                s = va_arg( args, char * );
                if ( !s )
                {
                    s = "<NULL>";
                }

                len = strnlen(s,precision);

                if ( !( flags & LEFT ) )
                {
                    while ( len < field_width-- )
                    {
                        *str++ = ' ';
                    }
                }
                for ( i = 0; i < len; ++i )
                {
                    *str++ = *s++;
                }
                while ( len < field_width-- )
                {
                    *str++ = ' ';
                }
                continue;

            case 'p':
                if ( field_width == -1 )
                {
                    field_width = 2*sizeof(void*);
                    flags |= ZEROPAD;
                }
                str = number( str, ( ulong ) va_arg( args, void * ), 16,
                    field_width, precision, flags );
                continue;
            case 'n':
                if ( qualifier == 'l' )
                {
                    long *ip = va_arg( args, long * );
                    *ip = (str - buf);
                }
                else
                    if ( qualifier == 'L' )
                    {
                        long long *ip = va_arg( args, long long *);
                        *ip = (str - buf);
                    }
                    else
                    {
                        int *ip = va_arg( args, int *);
                        *ip = (str - buf );
                    }
                    continue;
            case 'f':
                num = va_arg( args, float );
                flags |= SIGN;
                str = number( str, num, 10, field_width, precision, flags );
                continue;
            case 'o':
                base = 8;
                break;
            case 'X':
                flags |= LARGE;
            case 'x':
                base = 16;
                break;
            case 'd':
            case 'i':
                flags |= SIGN;
            case 'u':
                break;

            default:
                if (*fmt != '%')
                {
                    *str++ = '%';
                }
                if (*fmt)
                {
                    *str++ = *fmt;
                }
                else
                {
                    --fmt;
                }
                continue;
            }
            if ( qualifier == 'l' )
            {
                num = va_arg( args, unsigned long );
            }
            else
                if ( qualifier == 'L' )
                {
                    num = va_arg( args, long long );
                }
                else
                    if ( qualifier == 'h' )
                    {
                        if ( flags & SIGN )
                        {
                            num = va_arg( args, short );
                        }
                        else
                        {
                            num = va_arg( args, unsigned short );
                        }
                    }
                    else
                        if ( flags & SIGN )
                        {
                            num = va_arg( args, int );
                        }
                        else
                        {
                            num = va_arg( args, unsigned int );
                        }
                        str = number( str, num, base, field_width, precision, flags );
    }
    *str = '\0';
    return str-buf;
}
