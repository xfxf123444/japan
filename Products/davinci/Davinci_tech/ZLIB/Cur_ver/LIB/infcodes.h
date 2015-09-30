/* infcodes.h -- header to use infcodes.c
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */
typedef enum {        /* waiting for "i:"=input, "o:"=output, "x:"=nothing */
      START,    /* x: set up for LEN */
      LEN,      /* i: get length/literal/eob next */
      LENEXT,   /* i: getting length extra (have base) */
      DIST,     /* i: get distance next */
      DISTEXT,  /* i: getting distance extra */
      COPY,     /* o: copying bytes in window, waiting for space */
      LIT,      /* o: got literal, waiting for output space */
      WASH,     /* o: got eob, possibly still output waiting */
      END,      /* x: got eob and all data flushed */
      BADCODE}  /* x: got error */
inflate_codes_mode;

struct inflate_codes_state {

  /* mode */
  inflate_codes_mode mode;      /* current inflate_codes mode */

  /* mode dependent information */
  UINT len;
  union {
    struct {
      inflate_huft *tree;       /* pointer into tree */
      UINT need;                /* bits needed */
    } code;             /* if LEN or DIST, where in tree */
    UINT lit;           /* if LIT, literal */
    struct {
      UINT get;                 /* bits to get for extra */
      UINT dist;                /* distance back to copy from */
    } copy;             /* if EXT or COPY, where and how much */
  } sub;                /* submode */

  /* mode independent information */
  BYTE lbits;           /* ltree bits decoded per branch */
  BYTE dbits;           /* dtree bits decoder per branch */
  inflate_huft *ltree;          /* literal/length/eob tree */
  inflate_huft *dtree;          /* distance tree */

};
typedef struct inflate_codes_state FAR inflate_codes_statef;

inflate_codes_statef *inflate_codes_new(UINT, UINT,
										inflate_huft *, inflate_huft *,
										z_streamp );

int inflate_codes(inflate_blocks_statef *,z_streamp ,int);

void inflate_codes_free(inflate_codes_statef *, z_streamp );

