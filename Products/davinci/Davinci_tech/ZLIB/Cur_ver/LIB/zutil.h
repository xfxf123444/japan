/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* @(#) $Id$ */

#ifndef _Z_UTIL_H
#define _Z_UTIL_H

#include "..\Export\zlib.h"

#   include <errno.h>

#if (defined(_WIN32) || defined(__WIN32__)) && !defined(WIN32)
#  define WIN32
#endif
#if defined(__GNUC__) || defined(WIN32) || defined(__386__) || defined(i386)
#  ifndef __32BIT__
#    define __32BIT__
#  endif
#endif
#if defined(__MSDOS__) && !defined(MSDOS)
#  define MSDOS
#endif
/* compile with -Dlocal if your debugger can't find static symbols */
#ifndef NULL
#define NULL  0  
#endif

/* (size given to avoid silly warnings with Visual C++) */

#  define MAX_WBITS   15 /* 32K LZ77 window */
#  define DEF_WBITS   15
/* default windowBits for decompression. MAX_WBITS is for compression only */

#define MAX_MEM_LEVEL 8
#define DEF_MEM_LEVEL 8
/* default memLevel */
#define DIST_CODE_LEN  512 /* see definition of array dist_code below */

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

        /* target dependencies */

#ifdef MSDOS
#    include <malloc.h>
#    include <memory.h>
#endif
         /* functions */

#  ifdef SMALL_MEDIUM /* MSDOS small or medium model */
#    define zmemcpy _fmemcpy
#    define zmemcmp _fmemcmp
#    define zmemzero(dest, len) _fmemset(dest, 0, len)
#  else
#    define zmemcpy memcpy
#    define zmemcmp memcmp
#    define zmemzero(dest, len) memset(dest, 0, len)
#  endif

#if defined(M_I86) && !defined(__32BIT__)
	#if (!defined(_MSC_VER) || (_MSC_VER <= 600))
		#  define zcalloc  halloc
		#  define zcfree   hfree
	#else
		#define zcalloc _halloc
		#define zcfree  _hfree
	#endif
#else
	#define zcalloc calloc
	#define zcfree  free
#endif 

struct internal_state;

typedef struct z_stream_s {
    BYTE FAR    *next_in;  /* next input byte */
    UINT     avail_in;  /* number of bytes available at next_in */
    DWORD    total_in;  /* total nb of input bytes read so far */

    BYTE FAR    *next_out; /* next output byte should be put there */
    UINT     avail_out; /* remaining free space at next_out */
    DWORD    total_out; /* total nb of bytes output so far */

    char     *msg;      /* last error message, NULL if no error */
    struct internal_state FAR *state; /* not visible by applications */

    DWORD   adler;      /* adler32 value of the uncompressed data */
    DWORD   reserved;   /* reserved for future use */
} z_stream;

typedef z_stream FAR *z_streamp;

DWORD adler32(DWORD adler, const BYTE FAR *buf, UINT len);

/*
     Update a running Adler-32 checksum with the bytes buf[0..len-1] and
   return the updated checksum. If buf is NULL, this function returns
   the required initial value for the checksum.
   An Adler-32 checksum is almost as reliable as a CRC32 but can be computed
   much faster. Usage example:

     DWORD adler = adler32(0L, NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       adler = adler32(adler, buffer, length);
     }
     if (adler != original_adler) error();
*/


typedef DWORD (*check_func)(DWORD check, const BYTE FAR *buf,
				       UINT len);

#endif /* _Z_UTIL_H */
