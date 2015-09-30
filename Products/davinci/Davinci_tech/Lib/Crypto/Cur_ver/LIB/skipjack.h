#ifndef CRYPTOPP_SKIPJACK_H
#define CRYPTOPP_SKIPJACK_H

#include "cryptlib.h"
#include "misc.h"

NAMESPACE_BEGIN(CryptoPP)

class SKIPJACK : public BlockTransformation
{
public:
	enum {KEYLENGTH=10, BLOCKSIZE=8};
	unsigned int BlockSize() const {return BLOCKSIZE;}
	static unsigned int KeyLength(unsigned int keylength)
		{return KEYLENGTH;}

protected:
	SKIPJACK(const byte *userKey);

	static const byte fTable[256];

	SecBlock<byte[256]> tab;
};

class SKIPJACKEncryption : public SKIPJACK
{
public:
	SKIPJACKEncryption(const byte *userKey, unsigned int = 0)
		: SKIPJACK(userKey) {}

	void ProcessBlock(const byte *inBlock, byte * outBlock) const;
	void ProcessBlock(byte * inoutBlock) const
		{SKIPJACKEncryption::ProcessBlock(inoutBlock, inoutBlock);}
};

class SKIPJACKDecryption : public SKIPJACK
{
public:
	SKIPJACKDecryption(const byte *userKey, unsigned int = 0)
		: SKIPJACK(userKey) {}

	void ProcessBlock(const byte *inBlock, byte * outBlock) const;
	void ProcessBlock(byte * inoutBlock) const
		{SKIPJACKDecryption::ProcessBlock(inoutBlock, inoutBlock);}
};

NAMESPACE_END

#endif
