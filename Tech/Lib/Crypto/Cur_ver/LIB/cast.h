#ifndef CRYPTOPP_CAST_H
#define CRYPTOPP_CAST_H

#include "cryptlib.h"
#include "misc.h"

NAMESPACE_BEGIN(CryptoPP)

class CAST
{
protected:
	static const word32 S[8][256];
};

class CAST128 : public BlockTransformation, public CAST
{
public:
	enum {KEYLENGTH=16, BLOCKSIZE=8};
	unsigned int BlockSize() const {return BLOCKSIZE;}
	static unsigned int KeyLength(unsigned int keylength)
		{return keylength < 5 ? 5 : (keylength <= 16 ? keylength : 16);}

protected:
	// keylength should be between 5 and 16
	CAST128(const byte *userKey, unsigned int keylength);

	bool reduced;
	SecBlock<word32> K;
};

class CAST128Encryption : public CAST128
{
public:
	CAST128Encryption(const byte *userKey, unsigned int keylength=KEYLENGTH)
		: CAST128(userKey, keylength) {}

	void ProcessBlock(const byte *inBlock, byte * outBlock) const;
	void ProcessBlock(byte * inoutBlock) const
		{CAST128Encryption::ProcessBlock(inoutBlock, inoutBlock);}
};

class CAST128Decryption : public CAST128
{
public:
	CAST128Decryption(const byte *userKey, unsigned int keylength=KEYLENGTH)
		: CAST128(userKey, keylength) {}

	void ProcessBlock(const byte *inBlock, byte * outBlock) const;
	void ProcessBlock(byte * inoutBlock) const
		{CAST128Decryption::ProcessBlock(inoutBlock, inoutBlock);}
};

class CAST256 : public BlockTransformation, public CAST
{
public:
	enum {KEYLENGTH=32, BLOCKSIZE=16};
	unsigned int BlockSize() const {return BLOCKSIZE;}
	static unsigned int KeyLength(unsigned int keylength)
		{return keylength < 16 ? 16 : (keylength <= 32 ? keylength : 32);}

	void ProcessBlock(const byte *inBlock, byte * outBlock) const;
	void ProcessBlock(byte * inoutBlock) const
		{CAST256::ProcessBlock(inoutBlock, inoutBlock);}

protected:
	// keylength should be between 16 and 32 
	CAST256(const byte *userKey, unsigned int keylength);

	SecBlock<word32> K;

private:
	static const word32 t_m[8][24];
	static const unsigned int t_r[8][24];

	static void Omega(int i, word32 kappa[8]);
};

class CAST256Encryption : public CAST256
{
public:
	CAST256Encryption(const byte *userKey, unsigned int keylength=KEYLENGTH)
		: CAST256(userKey, keylength) {}
};

class CAST256Decryption : public CAST256
{
public:
	CAST256Decryption(const byte *userKey, unsigned int keylength=KEYLENGTH);
};

NAMESPACE_END

#endif
