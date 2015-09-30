#ifndef CRYPTOPP_RIJNDAEL_H
#define CRYPTOPP_RIJNDAEL_H

#include "misc.h"

class Rijndael
{
public:
	enum {KEYLENGTH=16, BLOCKSIZE=16};
	unsigned int BlockSize() const {return BLOCKSIZE;}
	static unsigned int KeyLength(unsigned int keylength)
		{return keylength <= 16 ? 16 : (keylength <= 24 ? 24 : 32);}

protected:
	Rijndael(const UCHAR *userKey, unsigned int keylength);

	ULONG k_len;
	ULONG key[32*5+24];
};

class RijndaelEncryption : public Rijndael
{
public:
	RijndaelEncryption(const UCHAR *userKey, unsigned int keylength=KEYLENGTH)
		: Rijndael(userKey, keylength) {}

	void ProcessBlock(const UCHAR *inBlock, UCHAR * outBlock) const;
	void ProcessBlock(UCHAR * inoutBlock) const
		{RijndaelEncryption::ProcessBlock(inoutBlock, inoutBlock);}
};

class RijndaelDecryption : public Rijndael
{
public:
	RijndaelDecryption(const UCHAR *userKey, unsigned int keylength=KEYLENGTH);

	void ProcessBlock(const UCHAR *inBlock, UCHAR * outBlock) const;
	void ProcessBlock(UCHAR * inoutBlock) const
		{RijndaelDecryption::ProcessBlock(inoutBlock, inoutBlock);}
};


#endif
