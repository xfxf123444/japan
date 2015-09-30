/* infutil.h -- types and macros common to blocks and codes
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

#ifndef _INFUTIL_H
#define _INFUTIL_H

typedef enum {
      TYPE,     /* get type bits (3, including end bit) */
      LENS,     /* get lengths for stored */
      STORED,   /* processing stored block */
      TABLE,    /* get table lengths */
      BTREE,    /* get bit lengths tree for a dynamic block */
      DTREE,    /* get length, distance trees for a dynamic block */
      CODES,    /* processing fixed or dynamic block */
      DRY,      /* output remaining window bytes */
      DONE,     /* finished last block, done */
      BAD}      /* got a data error--stuck here */
inflate_block_mode;

/* inflate blocks semi-private state */
struct inflate_blocks_state {

  /* mode */
  inflate_block_mode  mode;     /* current inflate_block mode */

  /* mode dependent information */
  union {
    UINT left;          /* if STORED, bytes left to copy */
    struct {
      UINT table;               /* table lengths (14 bits) */
      UINT index;               /* index into blens (or border) */
      UINT FAR *blens;             /* bit lengths of codes */
      UINT bb;                  /* bit length tree depth */
      inflate_huft *tb;         /* bit length decoding tree */
    } trees;            /* if DTREE, decoding info for trees */
    struct {
      inflate_codes_statef 
         *codes;
    } decode;           /* if CODES, current state */
  } sub;                /* submode */
  UINT last;            /* true if this block is the last block */

  /* mode independent information */
  UINT bitk;            /* bits in bit buffer */
  DWORD bitb;           /* bit buffer */
  inflate_huft *hufts;  /* single malloc for tree space */
  BYTE FAR *window;        /* sliding window */
  BYTE FAR *end;           /* one byte after sliding window */
  BYTE FAR *read;          /* window read pointer */
  BYTE FAR *write;         /* window write pointer */
  check_func checkfn;   /* check function */
  DWORD check;          /* check on output */

};


/* defines for inflate input/output */
/*   update pointers and return */
#define UPDBITS {s->bitb=b;s->bitk=k;}
#define UPDIN {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;}
#define UPDOUT {s->write=q;}
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LEAVE {UPDATE return inflate_flush(s,z,r);}
/*   get bytes and bits */
#define LOADIN {p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;}
#define NEEDBYTE {if(n)r=Z_OK;else LEAVE}
#define NEXTBYTE (n--,*p++)
#define NEEDBITS(j) {while(k<(j)){NEEDBYTE;b|=((DWORD)NEXTBYTE)<<k;k+=8;}}
#define DUMPBITS(j) {b>>=(j);k-=(j);}
/*   output bytes */
#define WAVAIL (UINT)(q<s->read?s->read-q-1:s->end-q)
#define LOADOUT {q=s->write;m=(UINT)WAVAIL;}
#define WRAP {if(q==s->end&&s->read!=s->window){q=s->window;m=(UINT)WAVAIL;}}
#define FLUSH {UPDOUT r=inflate_flush(s,z,r); LOADOUT}
#define NEEDOUT {if(m==0){WRAP if(m==0){FLUSH WRAP if(m==0) LEAVE}}r=Z_OK;}
#define OUTBYTE(a) {*q++=(BYTE)(a);m--;}
/*   load static pointers */
#define LOAD {LOADIN LOADOUT}

/* masks for lower bits (size given to avoid silly warnings with Visual C++) */
extern UINT inflate_mask[17];

/* copy as much as possible from the sliding window to the output area */
int inflate_flush(
    inflate_blocks_statef *,
    z_streamp ,
    int);

struct internal_state      {int dummy;}; /* for buggy compilers */

#endif
