/* d3des.h -
 *
 *	Headers and defines for d3des.c
 *	Graven Imagery, 1992.
 *
 * Copyright (c) 1988,1989,1990,1991,1992 by Richard Outerbridge
 *	(GEnie : OUTER; CIS : [71755,204])
 */

#ifdef __cplusplus
extern "C" {
#endif


#define	D2_DES		/* include double-length support */
#define	D3_DES		/* include triple-length support */

#ifdef	D3_DES
#ifndef D2_DES
#define D2_DES		/* D2_DES is needed for D3_DES */
#endif
#endif


#define D1_DES	1	/* des */
#ifdef D2_DES
#undef D2_DES
#define D2_DES	2	/* 2-des */
#endif
#ifdef D3_DES
#undef D3_DES
#define D3_DES	3	/* 3-des */
#endif

#define EN0		0	/* MODE == encrypt */
#define DE1		1	/* MODE == decrypt */

/* A useful alias on 68000-ish machines, but NOT USED HERE. */
//#pragma pack(1)
//
//typedef struct 
//{
//	long	Type, Mode;
//	unsigned char Key[24];
//	unsigned long KnL[32];
//	unsigned long KnR[32];
//	unsigned long Kn3[32];
//}DES_INFO, * PDES_INFO;
//
//#pragma pack()

void __cdecl deskey(PDES_INFO pdes, unsigned char *, short);
/*		      hexkey[8]     MODE
 * Sets the internal key register according to the hexadecimal
 * key contained in the 8 bytes of hexkey, according to the DES,
 * for encryption or decryption according to MODE.
 */

void __cdecl usekey(PDES_INFO pdes, unsigned long *);
/*		    cookedkey[32]
 * Loads the internal key register with the data in cookedkey.
 */

void __cdecl cpkey(PDES_INFO pdes, unsigned long *);
/*		   cookedkey[32]
 * Copies the contents of the internal key register into the storage
 * located at &cookedkey[0].
 */

void __cdecl des(PDES_INFO pdes, unsigned char *, unsigned char *);
/*		    from[8]	      to[8]
 * Encrypts/Decrypts (according to the key currently loaded in the
 * internal key register) one block of eight bytes at address 'from'
 * into the block at address 'to'.  They can be the same.
 */

#ifdef D2_DES

#define desDkey(a,b)	des2key((a),(b))
void __cdecl des2key(PDES_INFO pdes, unsigned char *, short);
/*		      hexkey[16]     MODE
 * Sets the internal key registerS according to the hexadecimal
 * keyS contained in the 16 bytes of hexkey, according to the DES,
 * for DOUBLE encryption or decryption according to MODE.
 * NOTE: this clobbers all three key registers!
 */

void __cdecl Ddes(PDES_INFO pdes, unsigned char *, unsigned char *);
/*		    from[8]	      to[8]
 * Encrypts/Decrypts (according to the keyS currently loaded in the
 * internal key registerS) one block of eight bytes at address 'from'
 * into the block at address 'to'.  They can be the same.
 */

void __cdecl D2des(PDES_INFO pdes, unsigned char *, unsigned char *);
/*		    from[16]	      to[16]
 * Encrypts/Decrypts (according to the keyS currently loaded in the
 * internal key registerS) one block of SIXTEEN bytes at address 'from'
 * into the block at address 'to'.  They can be the same.
 */

void __cdecl makekey(PDES_INFO pdes, char *, WORD wKeyLength);
/*		*password,	single-length key[8]
 * With a double-length default key, this routine hashes a NULL-terminated
 * string into an eight-byte random-looking key, suitable for use with the
 * deskey() routine.
 */

#define makeDkey(a,b,c)	make2key((a),(b),(c))
void __cdecl make2key(PDES_INFO pdes, char *, WORD wKeyLength);
/*		*password,	double-length key[16]
 * With a double-length default key, this routine hashes a NULL-terminated
 * string into a sixteen-byte random-looking key, suitable for use with the
 * des2key() routine.
 */

#ifndef D3_DES	/* D2_DES only */

#define useDkey(a,b)	use2key((a),(b))
#define cpDkey(a,b)		cp2key((a),(b))

void __cdecl use2key(PDES_INFO pdes, unsigned long *);
/*		    cookedkey[64]
 * Loads the internal key registerS with the data in cookedkey.
 * NOTE: this clobbers all three key registers!
 */

void __cdecl cp2key(PDES_INFO pdes, unsigned long *);
/*		   cookedkey[64]
 * Copies the contents of the internal key registerS into the storage
 * located at &cookedkey[0].
 */

#else	/* D3_DES too */

#define useDkey(a,b)	use3key((a),(b))
#define cpDkey(a,b)		cp3key((a),(b))

void __cdecl des3key(PDES_INFO pdes, unsigned char *, short);
/*		      hexkey[24]     MODE
 * Sets the internal key registerS according to the hexadecimal
 * keyS contained in the 24 bytes of hexkey, according to the DES,
 * for DOUBLE encryption or decryption according to MODE.
 */

void __cdecl use3key(PDES_INFO pdes, unsigned long *);
/*		    cookedkey[96]
 * Loads the 3 internal key registerS with the data in cookedkey.
 */

void __cdecl cp3key(PDES_INFO pdes, unsigned long *);
/*		   cookedkey[96]
 * Copies the contents of the 3 internal key registerS into the storage
 * located at &cookedkey[0].
 */

void __cdecl make3key(PDES_INFO pdes, char *, WORD wKeyLength);
/*		*password,	triple-length key[24]
 * With a triple-length default key, this routine hashes a NULL-terminated
 * string into a twenty-four-byte random-looking key, suitable for use with
 * the des3key() routine.
 */

void __cdecl D3des(PDES_INFO pdes, unsigned char *from, unsigned char *into);

#endif	/* D3_DES */
#endif	/* D2_DES */

/* d3des.h V5.09 rwo 9208.04 15:06 Graven Imagery
 ********************************************************************/

#ifdef __cplusplus
}
#endif
