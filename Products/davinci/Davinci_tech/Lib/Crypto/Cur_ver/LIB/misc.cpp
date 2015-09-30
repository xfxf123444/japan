// misc.cpp - written and placed in the public domain by Wei Dai

#include "misc.h"

void xorbuf(UCHAR *buf, const UCHAR *mask, ULONG count)
{
	if (((ULONG)buf | (ULONG)mask | count) % WORD_SIZE == 0)
		XorWords((USHORT *)buf, (const USHORT *)mask, count/WORD_SIZE);
	else
	{
		for (ULONG i=0; i<count; i++)
			buf[i] ^= mask[i];
	}
}

void xorbuf(UCHAR *output, const UCHAR *input, const UCHAR *mask, ULONG count)
{
	if (((ULONG)output | (ULONG)input | (ULONG)mask | count) % WORD_SIZE == 0)
		XorWords((USHORT *)output, (const USHORT *)input, (const USHORT *)mask, count/WORD_SIZE);
	else
	{
		for (ULONG i=0; i<count; i++)
			output[i] = input[i] ^ mask[i];
	}
}

