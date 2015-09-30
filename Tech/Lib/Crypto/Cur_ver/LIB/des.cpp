// des.cpp - modified by Wei Dai from:

/*
 * This is a major rewrite of my old public domain DES code written
 * circa 1987, which in turn borrowed heavily from Jim Gillogly's 1977
 * public domain code. I pretty much kept my key scheduling code, but
 * the actual encrypt/decrypt routines are taken from from Richard
 * Outerbridge's DES code as printed in Schneier's "Applied Cryptography."
 *
 * This code is in the public domain. I would appreciate bug reports and
 * enhancements.
 *
 * Phil Karn KA9Q, karn@unix.ka9q.ampr.org, August 1994.
 */

#include "des.h"
#include "misc.h"


/* Tables defined in the Data Encryption Standard documents
 * Three of these tables, the initial permutation, the final
 * permutation and the expansion operator, are regular enough that
 * for speed, we hard-code them. They're here for reference only.
 * Also, the S and P boxes are used by a separate program, gensp.c,
 * to build the combined SP box, Spbox[]. They're also here just
 * for reference.
 */
const ULONG Spbox[8][64] = {
{
0x01010400,0x00000000,0x00010000,0x01010404, 0x01010004,0x00010404,0x00000004,0x00010000,
0x00000400,0x01010400,0x01010404,0x00000400, 0x01000404,0x01010004,0x01000000,0x00000004,
0x00000404,0x01000400,0x01000400,0x00010400, 0x00010400,0x01010000,0x01010000,0x01000404,
0x00010004,0x01000004,0x01000004,0x00010004, 0x00000000,0x00000404,0x00010404,0x01000000,
0x00010000,0x01010404,0x00000004,0x01010000, 0x01010400,0x01000000,0x01000000,0x00000400,
0x01010004,0x00010000,0x00010400,0x01000004, 0x00000400,0x00000004,0x01000404,0x00010404,
0x01010404,0x00010004,0x01010000,0x01000404, 0x01000004,0x00000404,0x00010404,0x01010400,
0x00000404,0x01000400,0x01000400,0x00000000, 0x00010004,0x00010400,0x00000000,0x01010004},
{
0x80108020,0x80008000,0x00008000,0x00108020, 0x00100000,0x00000020,0x80100020,0x80008020,
0x80000020,0x80108020,0x80108000,0x80000000, 0x80008000,0x00100000,0x00000020,0x80100020,
0x00108000,0x00100020,0x80008020,0x00000000, 0x80000000,0x00008000,0x00108020,0x80100000,
0x00100020,0x80000020,0x00000000,0x00108000, 0x00008020,0x80108000,0x80100000,0x00008020,
0x00000000,0x00108020,0x80100020,0x00100000, 0x80008020,0x80100000,0x80108000,0x00008000,
0x80100000,0x80008000,0x00000020,0x80108020, 0x00108020,0x00000020,0x00008000,0x80000000,
0x00008020,0x80108000,0x00100000,0x80000020, 0x00100020,0x80008020,0x80000020,0x00100020,
0x00108000,0x00000000,0x80008000,0x00008020, 0x80000000,0x80100020,0x80108020,0x00108000},
{
0x00000208,0x08020200,0x00000000,0x08020008, 0x08000200,0x00000000,0x00020208,0x08000200,
0x00020008,0x08000008,0x08000008,0x00020000, 0x08020208,0x00020008,0x08020000,0x00000208,
0x08000000,0x00000008,0x08020200,0x00000200, 0x00020200,0x08020000,0x08020008,0x00020208,
0x08000208,0x00020200,0x00020000,0x08000208, 0x00000008,0x08020208,0x00000200,0x08000000,
0x08020200,0x08000000,0x00020008,0x00000208, 0x00020000,0x08020200,0x08000200,0x00000000,
0x00000200,0x00020008,0x08020208,0x08000200, 0x08000008,0x00000200,0x00000000,0x08020008,
0x08000208,0x00020000,0x08000000,0x08020208, 0x00000008,0x00020208,0x00020200,0x08000008,
0x08020000,0x08000208,0x00000208,0x08020000, 0x00020208,0x00000008,0x08020008,0x00020200},
{
0x00802001,0x00002081,0x00002081,0x00000080, 0x00802080,0x00800081,0x00800001,0x00002001,
0x00000000,0x00802000,0x00802000,0x00802081, 0x00000081,0x00000000,0x00800080,0x00800001,
0x00000001,0x00002000,0x00800000,0x00802001, 0x00000080,0x00800000,0x00002001,0x00002080,
0x00800081,0x00000001,0x00002080,0x00800080, 0x00002000,0x00802080,0x00802081,0x00000081,
0x00800080,0x00800001,0x00802000,0x00802081, 0x00000081,0x00000000,0x00000000,0x00802000,
0x00002080,0x00800080,0x00800081,0x00000001, 0x00802001,0x00002081,0x00002081,0x00000080,
0x00802081,0x00000081,0x00000001,0x00002000, 0x00800001,0x00002001,0x00802080,0x00800081,
0x00002001,0x00002080,0x00800000,0x00802001, 0x00000080,0x00800000,0x00002000,0x00802080},
{
0x00000100,0x02080100,0x02080000,0x42000100, 0x00080000,0x00000100,0x40000000,0x02080000,
0x40080100,0x00080000,0x02000100,0x40080100, 0x42000100,0x42080000,0x00080100,0x40000000,
0x02000000,0x40080000,0x40080000,0x00000000, 0x40000100,0x42080100,0x42080100,0x02000100,
0x42080000,0x40000100,0x00000000,0x42000000, 0x02080100,0x02000000,0x42000000,0x00080100,
0x00080000,0x42000100,0x00000100,0x02000000, 0x40000000,0x02080000,0x42000100,0x40080100,
0x02000100,0x40000000,0x42080000,0x02080100, 0x40080100,0x00000100,0x02000000,0x42080000,
0x42080100,0x00080100,0x42000000,0x42080100, 0x02080000,0x00000000,0x40080000,0x42000000,
0x00080100,0x02000100,0x40000100,0x00080000, 0x00000000,0x40080000,0x02080100,0x40000100},
{
0x20000010,0x20400000,0x00004000,0x20404010, 0x20400000,0x00000010,0x20404010,0x00400000,
0x20004000,0x00404010,0x00400000,0x20000010, 0x00400010,0x20004000,0x20000000,0x00004010,
0x00000000,0x00400010,0x20004010,0x00004000, 0x00404000,0x20004010,0x00000010,0x20400010,
0x20400010,0x00000000,0x00404010,0x20404000, 0x00004010,0x00404000,0x20404000,0x20000000,
0x20004000,0x00000010,0x20400010,0x00404000, 0x20404010,0x00400000,0x00004010,0x20000010,
0x00400000,0x20004000,0x20000000,0x00004010, 0x20000010,0x20404010,0x00404000,0x20400000,
0x00404010,0x20404000,0x00000000,0x20400010, 0x00000010,0x00004000,0x20400000,0x00404010,
0x00004000,0x00400010,0x20004010,0x00000000, 0x20404000,0x20000000,0x00400010,0x20004010},
{
0x00200000,0x04200002,0x04000802,0x00000000, 0x00000800,0x04000802,0x00200802,0x04200800,
0x04200802,0x00200000,0x00000000,0x04000002, 0x00000002,0x04000000,0x04200002,0x00000802,
0x04000800,0x00200802,0x00200002,0x04000800, 0x04000002,0x04200000,0x04200800,0x00200002,
0x04200000,0x00000800,0x00000802,0x04200802, 0x00200800,0x00000002,0x04000000,0x00200800,
0x04000000,0x00200800,0x00200000,0x04000802, 0x04000802,0x04200002,0x04200002,0x00000002,
0x00200002,0x04000000,0x04000800,0x00200000, 0x04200800,0x00000802,0x00200802,0x04200800,
0x00000802,0x04000002,0x04200802,0x04200000, 0x00200800,0x00000000,0x00000002,0x04200802,
0x00000000,0x00200802,0x04200000,0x00000800, 0x04000002,0x04000800,0x00000800,0x00200002},
{
0x10001040,0x00001000,0x00040000,0x10041040, 0x10000000,0x10001040,0x00000040,0x10000000,
0x00040040,0x10040000,0x10041040,0x00041000, 0x10041000,0x00041040,0x00001000,0x00000040,
0x10040000,0x10000040,0x10001000,0x00001040, 0x00041000,0x00040040,0x10040040,0x10041000,
0x00001040,0x00000000,0x00000000,0x10040040, 0x10000040,0x10001000,0x00041040,0x00040000,
0x00041040,0x00040000,0x10041000,0x00001000, 0x00000040,0x10040040,0x00001000,0x00041040,
0x10001000,0x00000040,0x10000040,0x10040000, 0x10040040,0x10000000,0x00040000,0x10001040,
0x00000000,0x10041040,0x00040040,0x10000040, 0x10040000,0x10001000,0x10001040,0x00000000,
0x10041040,0x00041000,0x00041000,0x00001040, 0x00001040,0x00040040,0x10000000,0x10041000}
};

/* permuted choice table (key) */
const UCHAR pc1[] = {
	   57, 49, 41, 33, 25, 17,  9,
		1, 58, 50, 42, 34, 26, 18,
	   10,  2, 59, 51, 43, 35, 27,
	   19, 11,  3, 60, 52, 44, 36,

	   63, 55, 47, 39, 31, 23, 15,
		7, 62, 54, 46, 38, 30, 22,
	   14,  6, 61, 53, 45, 37, 29,
	   21, 13,  5, 28, 20, 12,  4
};

/* number left rotations of pc1 */
const UCHAR totrot[] = {
	   1,2,4,6,8,10,12,14,15,17,19,21,23,25,27,28
};

/* permuted choice key (table) */
const UCHAR pc2[] = {
	   14, 17, 11, 24,  1,  5,
		3, 28, 15,  6, 21, 10,
	   23, 19, 12,  4, 26,  8,
	   16,  7, 27, 20, 13,  2,
	   41, 52, 31, 37, 47, 55,
	   30, 40, 51, 45, 33, 48,
	   44, 49, 39, 56, 34, 53,
	   46, 42, 50, 36, 29, 32
};

/* bit 0 is left-most in UCHAR */
const long bytebit[] = {
	   0200,0100,040,020,010,04,02,01
};

/* Set key (initialize key schedule array) */
DES::DES(UCHAR *key, CipherDir dir)
{
    UCHAR buffer[56+56+8];
    UCHAR *pc1m= buffer;                 /* place to modify pc1 into */
    UCHAR *pcr=pc1m+56;                 /* place to rotate pc1 into */
    UCHAR *ks=pcr+56;
    register long i,j,l;
    long m;
	ULONG dwTemp;

    for (j=0; j<56; j++) 
	{          /* convert pc1 to bits of key */
        l=pc1[j]-1;             /* integer bit location  */
	    m = l & 07;             /* find bit              */
	    pc1m[j]=(key[l>>3] &    /* find which key UCHAR l is in */
	      	   bytebit[m])     /* and which bit of that UCHAR */
			   ? 1 : 0;        /* and store 1-bit result */
    }
    for (i=0; i<16; i++) 
    {          /* key chunk for each iteration */
 	    YGECMemset(ks,0,8);         /* Clear key schedule */
 	    for (j=0; j<56; j++)    /* rotate pc1 the right amount */
		   pcr[j] = pc1m[(l=j+totrot[i])<(j<28? 28 : 56) ? l: l-28];
					   /* rotate left and right halves independently */
 	    for (j=0; j<48; j++)
		{   /* select bits individually */
			   /* check bit that goes to ks[j] */
		   if (pcr[pc2[j]-1])
		   {
			   /* mask it in if it's there */
			   l= j % 6;
			   ks[j/6] |= bytebit[l] >> 2;
		   }
		}
		   /* Now convert to odd/even interleaved form for use in F */
	   k[2*i] = ((ULONG)ks[0] << 24)
				| ((ULONG)ks[2] << 16)
				| ((ULONG)ks[4] << 8)
				| ((ULONG)ks[6]);
	   k[2*i+1] = ((ULONG)ks[1] << 24)
				| ((ULONG)ks[3] << 16)
				| ((ULONG)ks[5] << 8)
				| ((ULONG)ks[7]);
   }

   if (dir==DECRYPTION)     // reverse key schedule order
		for (i=0; i<16; i+=2)
		{
			dwTemp = k[i];
			k[i] = k[32-2-i];
			k[32-2-i] = dwTemp;
			dwTemp = k[i+1];
			k[i+1] = k[32-1-i];
			k[32-1-i] = dwTemp;
		}
}


// Wei Dai's modification to Richard Outerbridge's initial permutation 
// algorithm, this one is faster if you have access to rotate instructions 
// (like in MSVC)
inline void IPERM(ULONG &left, ULONG &right)
{
	ULONG work;

	right = rotlFixed(right, 4U);
	work = (left ^ right) & 0xf0f0f0f0;
	left ^= work;
	right = rotrFixed(right^work, 20U);
	work = (left ^ right) & 0xffff0000;
	left ^= work;
	right = rotrFixed(right^work, 18U);
	work = (left ^ right) & 0x33333333;
	left ^= work;
	right = rotrFixed(right^work, 6U);
	work = (left ^ right) & 0x00ff00ff;
	left ^= work;
	right = rotlFixed(right^work, 9U);
	work = (left ^ right) & 0xaaaaaaaa;
	left = rotlFixed(left^work, 1U);
	right ^= work;
}

inline void FPERM(ULONG &left, ULONG &right)
{
	ULONG work;

	right = rotrFixed(right, 1U);
	work = (left ^ right) & 0xaaaaaaaa;
	right ^= work;
	left = rotrFixed(left^work, 9U);
	work = (left ^ right) & 0x00ff00ff;
	right ^= work;
	left = rotlFixed(left^work, 6U);
	work = (left ^ right) & 0x33333333;
	right ^= work;
	left = rotlFixed(left^work, 18U);
	work = (left ^ right) & 0xffff0000;
	right ^= work;
	left = rotlFixed(left^work, 20U);
	work = (left ^ right) & 0xf0f0f0f0;
	right ^= work;
	left = rotrFixed(left^work, 4U);
}

void DES::RawProcessBlock(ULONG &l_, ULONG &r_) const
{
	ULONG l = l_, r = r_;
	const ULONG *kptr=k;

	for (unsigned i=0; i<8; i++)
	{
		ULONG work = rotrFixed(r, 4U) ^ kptr[4*i+0];
		l ^= Spbox[6][(work) & 0x3f]
		  ^  Spbox[4][(work >> 8) & 0x3f]
		  ^  Spbox[2][(work >> 16) & 0x3f]
		  ^  Spbox[0][(work >> 24) & 0x3f];
		work = r ^ kptr[4*i+1];
		l ^= Spbox[7][(work) & 0x3f]
		  ^  Spbox[5][(work >> 8) & 0x3f]
		  ^  Spbox[3][(work >> 16) & 0x3f]
		  ^  Spbox[1][(work >> 24) & 0x3f];

		work = rotrFixed(l, 4U) ^ kptr[4*i+2];
		r ^= Spbox[6][(work) & 0x3f]
		  ^  Spbox[4][(work >> 8) & 0x3f]
		  ^  Spbox[2][(work >> 16) & 0x3f]
		  ^  Spbox[0][(work >> 24) & 0x3f];
		work = l ^ kptr[4*i+3];
		r ^= Spbox[7][(work) & 0x3f]
		  ^  Spbox[5][(work >> 8) & 0x3f]
		  ^  Spbox[3][(work >> 16) & 0x3f]
		  ^  Spbox[1][(work >> 24) & 0x3f];
	}

	l_ = l; r_ = r;
}

// Encrypt or decrypt a block of data in ECB mode
void DES::ProcessBlock(const UCHAR *inBlock, UCHAR * outBlock) const
{
	ULONG l,r;
	GetBlockBigEndian(inBlock, l, r);
	IPERM(l,r);

	const ULONG *kptr=k;

	for (unsigned i=0; i<8; i++)
	{
		ULONG work = rotrFixed(r, 4U) ^ kptr[4*i+0];
		l ^= Spbox[6][(work) & 0x3f]
		  ^  Spbox[4][(work >> 8) & 0x3f]
		  ^  Spbox[2][(work >> 16) & 0x3f]
		  ^  Spbox[0][(work >> 24) & 0x3f];
		work = r ^ kptr[4*i+1];
		l ^= Spbox[7][(work) & 0x3f]
		  ^  Spbox[5][(work >> 8) & 0x3f]
		  ^  Spbox[3][(work >> 16) & 0x3f]
		  ^  Spbox[1][(work >> 24) & 0x3f];

		work = rotrFixed(l, 4U) ^ kptr[4*i+2];
		r ^= Spbox[6][(work) & 0x3f]
		  ^  Spbox[4][(work >> 8) & 0x3f]
		  ^  Spbox[2][(work >> 16) & 0x3f]
		  ^  Spbox[0][(work >> 24) & 0x3f];
		work = l ^ kptr[4*i+3];
		r ^= Spbox[7][(work) & 0x3f]
		  ^  Spbox[5][(work >> 8) & 0x3f]
		  ^  Spbox[3][(work >> 16) & 0x3f]
		  ^  Spbox[1][(work >> 24) & 0x3f];
	}

	FPERM(l,r);
	PutBlockBigEndian(outBlock, r, l);
}

void DES_EDE2_Encryption::ProcessBlock(const UCHAR *inBlock, UCHAR *outBlock) const
{
	ULONG l,r;
	GetBlockBigEndian(inBlock, l, r);
	IPERM(l,r);
	e.RawProcessBlock(l, r);
	d.RawProcessBlock(r, l);
	e.RawProcessBlock(l, r);
	FPERM(l,r);
	PutBlockBigEndian(outBlock, r, l);
}

void DES_EDE2_Decryption::ProcessBlock(const UCHAR *inBlock, UCHAR *outBlock) const
{
	ULONG l,r;
	GetBlockBigEndian(inBlock, l, r);
	IPERM(l,r);
	d.RawProcessBlock(l, r);
	e.RawProcessBlock(r, l);
	d.RawProcessBlock(l, r);
	FPERM(l,r);
	PutBlockBigEndian(outBlock, r, l);
}

void DES_EDE3_Encryption::ProcessBlock(const UCHAR *inBlock, UCHAR *outBlock) const
{
	ULONG l,r;
	GetBlockBigEndian(inBlock, l, r);
	IPERM(l,r);
	e1.RawProcessBlock(l, r);
	d2.RawProcessBlock(r, l);
	e3.RawProcessBlock(l, r);
	FPERM(l,r);
	PutBlockBigEndian(outBlock, r, l);
}

void DES_EDE3_Decryption::ProcessBlock(const UCHAR *inBlock, UCHAR *outBlock) const
{
	ULONG l,r;
	GetBlockBigEndian(inBlock, l, r);
	IPERM(l,r);
	d3.RawProcessBlock(l, r);
	e2.RawProcessBlock(r, l);
	d1.RawProcessBlock(l, r);
	FPERM(l,r);
	PutBlockBigEndian(outBlock, r, l);
}

void DES_XEX3_Encryption::ProcessBlock(const UCHAR *inBlock, UCHAR *outBlock) const
{
	xorbuf(outBlock, inBlock, x1, BLOCKSIZE);
	e2.ProcessBlock(outBlock);
	xorbuf(outBlock, x3, BLOCKSIZE);
}

void DES_XEX3_Decryption::ProcessBlock(const UCHAR *inBlock, UCHAR *outBlock) const
{
	xorbuf(outBlock, inBlock, x3, BLOCKSIZE);
	d2.ProcessBlock(outBlock);
	xorbuf(outBlock, x1, BLOCKSIZE);
}

