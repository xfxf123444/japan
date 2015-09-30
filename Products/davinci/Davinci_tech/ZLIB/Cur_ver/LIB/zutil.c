/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* @(#) $Id$ */

#include "zutil.h"
#include "deflate.h"
#include "infblock.h"

const char *z_errmsg[10] = {
"need dictionary",     /* Z_NEED_DICT       2  */
"stream end",          /* Z_STREAM_END      1  */
"",                    /* Z_OK              0  */
"file error",          /* Z_ERRNO         (-1) */
"stream error",        /* Z_STREAM_ERROR  (-2) */
"data error",          /* Z_DATA_ERROR    (-3) */
"insufficient memory", /* Z_MEM_ERROR     (-4) */
"buffer error",        /* Z_BUF_ERROR     (-5) */
"incompatible version",/* Z_VERSION_ERROR (-6) */
""};

#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);


const char ZEXPORT * zlibVersion()
{
    return ZLIB_VERSION;
}


/* exported to allow conversion of error code to string for compress() and
 * uncompress()
 */
const char ZEXPORT * zError(err)
    int err;
{
    return z_errmsg[Z_NEED_DICT - err];
}

/* ===========================================================================
     Decompresses the source buffer into the destination buffer.  sourceLen is
   the byte length of the source buffer. Upon entry, destLen is the total
   size of the destination buffer, which must be large enough to hold the
   entire uncompressed data. (The size of the uncompressed data must have
   been saved previously by the compressor and transmitted to the decompressor
   by some mechanism outside the scope of this compression library.)
   Upon exit, destLen is the actual size of the compressed buffer.
     This function can be used to decompress a whole file at once if the
   input file is mmap'ed.

     uncompress returns Z_OK if success, Z_MEM_ERROR if there was not
   enough memory, Z_BUF_ERROR if there was not enough room in the output
   buffer, or Z_DATA_ERROR if the input data was corrupted.
*/
int ZEXPORT uncompress (dest, destLen, source, sourceLen)
    BYTE FAR *dest;
    DWORD FAR *destLen;
    const BYTE FAR *source;
    DWORD sourceLen;
{
    z_stream stream;
    int err;

    stream.next_in = (BYTE FAR*)source;
    stream.avail_in = (UINT)sourceLen;
    /* Check for source > 64K on 16-bit machine: */
    if ((DWORD)stream.avail_in != sourceLen) return Z_BUF_ERROR;

    stream.next_out = dest;
    stream.avail_out = (UINT)*destLen;
    if ((DWORD)stream.avail_out != *destLen) return Z_BUF_ERROR;

    err = inflateInit(&stream, DEF_WBITS, ZLIB_VERSION, sizeof(z_stream));
	if (err != Z_OK) return err;

    err = inflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        inflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *destLen = stream.total_out;

    err = inflateEnd(&stream);
    return err;
}
/* ===========================================================================
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in deflateInit.  sourceLen is the byte
   length of the source buffer. Upon entry, destLen is the total size of the
   destination buffer, which must be at least 0.1% larger than sourceLen plus
   12 bytes. Upon exit, destLen is the actual size of the compressed buffer.

     compress returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_BUF_ERROR if there was not enough room in the output buffer,
   Z_STREAM_ERROR if the level parameter is invalid.
*/
int ZEXPORT compress(dest, destLen, source, sourceLen, level)
    BYTE FAR *dest;
    DWORD FAR *destLen;
    const BYTE FAR *source;
    DWORD sourceLen;
    int level;
{
    z_stream stream;
    int err;

    if (level < 0 || level > 8) level = 6;
    stream.next_in = (BYTE FAR*)source;
    stream.avail_in = (UINT)sourceLen;
    if ((DWORD)stream.avail_in != sourceLen) return Z_BUF_ERROR;
    stream.next_out = dest;
    stream.avail_out = (UINT)*destLen;
    if ((DWORD)stream.avail_out != *destLen) return Z_BUF_ERROR;

    err = deflateInit(&stream, level, Z_DEFLATED, MAX_WBITS, DEF_MEM_LEVEL,
				 Z_DEFAULT_STRATEGY,ZLIB_VERSION, sizeof(z_stream));
    if (err != Z_OK) return err;

    err = deflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        deflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *destLen = stream.total_out;

    err = deflateEnd(&stream);
    return err;
}

/* ========================================================================= */
DWORD adler32(adler, buf, len)
    DWORD adler;
    const BYTE FAR *buf;
    UINT len;
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    int k;

    if (buf == NULL) return 1L;

    while (len > 0) {
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            DO16(buf);
	    buf += 16;
            k -= 16;
        }
        if (k != 0) do {
            s1 += *buf++;
	    s2 += s1;
        } while (--k);
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}

