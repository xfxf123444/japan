#ifndef CRYPTOPP_DES_H
#define CRYPTOPP_DES_H
#include "misc.h"

enum CipherDir {
	///
	ENCRYPTION,
	///
	DECRYPTION};

class DES
{

public:
	DES(UCHAR *userKey, CipherDir);

	void ProcessBlock(const UCHAR *inBlock, UCHAR * outBlock) const;
	void ProcessBlock(UCHAR * inoutBlock) const
		{DES::ProcessBlock(inoutBlock, inoutBlock);}

	enum {KEYLENGTH=8, BLOCKSIZE=8};
	ULONG BlockSize() const {return BLOCKSIZE;}
	ULONG KeyLength(ULONG keylength) {return KEYLENGTH;}

	// exposed for faster Triple-DES
	void RawProcessBlock(ULONG &l, ULONG &r) const;
	ULONG k[32];
};

class DESEncryption : public DES
{
public:
	DESEncryption(UCHAR * userKey, ULONG = 0)
		: DES (userKey, ENCRYPTION) {}
};

class DESDecryption : public DES
{
public:
	DESDecryption(UCHAR * userKey, ULONG = 0)
		: DES (userKey, DECRYPTION) {}
};

// two key triple-des

class DES_EDE2_Encryption
{
public:
	DES_EDE2_Encryption(UCHAR * userKey, ULONG = 0)
		: e(userKey, ENCRYPTION), d(userKey + DES::KEYLENGTH, DECRYPTION) {}

	void ProcessBlock(const UCHAR *inBlock, UCHAR * outBlock) const;
	void ProcessBlock(UCHAR * inoutBlock) const
		{DES_EDE2_Encryption::ProcessBlock(inoutBlock, inoutBlock);}

	enum {KEYLENGTH=16, BLOCKSIZE=8};
	ULONG BlockSize() const {return BLOCKSIZE;}
	ULONG KeyLength(ULONG keylength) {return KEYLENGTH;}

private:
	DES e, d;
};

class DES_EDE2_Decryption
{
public:
	DES_EDE2_Decryption(UCHAR * userKey, ULONG = 0)
		: d(userKey, DECRYPTION), e(userKey + DES::KEYLENGTH, ENCRYPTION) {}

	void ProcessBlock(const UCHAR *inBlock, UCHAR * outBlock) const;
	void ProcessBlock(UCHAR * inoutBlock) const
		{DES_EDE2_Decryption::ProcessBlock(inoutBlock, inoutBlock);}

	enum {KEYLENGTH=16, BLOCKSIZE=8};
	ULONG BlockSize() const {return BLOCKSIZE;}
	ULONG KeyLength(ULONG keylength) {return KEYLENGTH;}

private:
	DES d, e;
};

// three key triple-des

class DES_EDE3_Encryption
{
public:
	DES_EDE3_Encryption(UCHAR * userKey, ULONG = 0)
		: e1(userKey, ENCRYPTION), d2(userKey + DES::KEYLENGTH, DECRYPTION),
		  e3(userKey + 2*DES::KEYLENGTH, ENCRYPTION) {}

	void ProcessBlock(const UCHAR *inBlock, UCHAR * outBlock) const;
	void ProcessBlock(UCHAR * inoutBlock) const
		{DES_EDE3_Encryption::ProcessBlock(inoutBlock, inoutBlock);}

	enum {KEYLENGTH=24, BLOCKSIZE=8};
	ULONG BlockSize() const {return BLOCKSIZE;}
	ULONG KeyLength(ULONG keylength) {return KEYLENGTH;}

private:
	DES e1, d2, e3;
};

class DES_EDE3_Decryption
{
public:
	DES_EDE3_Decryption(UCHAR * userKey, ULONG = 0)
		: d1(userKey, DECRYPTION), e2(userKey + DES::KEYLENGTH, ENCRYPTION),
		  d3(userKey + 2*DES::KEYLENGTH, DECRYPTION) {}

	void ProcessBlock(const UCHAR *inBlock, UCHAR * outBlock) const;
	void ProcessBlock(UCHAR * inoutBlock) const
		{DES_EDE3_Decryption::ProcessBlock(inoutBlock, inoutBlock);}

	enum {KEYLENGTH=24, BLOCKSIZE=8};
	ULONG BlockSize() const {return BLOCKSIZE;}
	ULONG KeyLength(ULONG keylength) {return KEYLENGTH;}

private:
	DES d1, e2, d3;
};

// also known as DESX

class DES_XEX3_Encryption
{
public:
	DES_XEX3_Encryption(UCHAR * userKey, ULONG = 0)
		:e2(userKey + 8, ENCRYPTION)
	{
		YGECMemcpy(x1,userKey,8);
		YGECMemcpy(x3,&userKey[16], 8);
	}

	void ProcessBlock(const UCHAR *inBlock, UCHAR * outBlock) const;
	void ProcessBlock(UCHAR * inoutBlock) const
		{DES_XEX3_Encryption::ProcessBlock(inoutBlock, inoutBlock);}

	enum {KEYLENGTH=24, BLOCKSIZE=8};
	ULONG BlockSize() const {return BLOCKSIZE;}
	ULONG KeyLength(ULONG keylength) {return KEYLENGTH;}

private:
	UCHAR x1[8];
	DES e2;
	UCHAR x3[8];
};

class DES_XEX3_Decryption
{
public:
	DES_XEX3_Decryption(UCHAR * userKey, ULONG = 0)
		: d2(userKey + 8, DECRYPTION)
	{
		YGECMemcpy(x1,userKey,8);
		YGECMemcpy(x3,&userKey[16], 8);
	}

	void ProcessBlock(const UCHAR *inBlock, UCHAR * outBlock) const;
	void ProcessBlock(UCHAR * inoutBlock) const
		{DES_XEX3_Decryption::ProcessBlock(inoutBlock, inoutBlock);}

	enum {KEYLENGTH=24, BLOCKSIZE=8};
	ULONG BlockSize() const {return BLOCKSIZE;}
	ULONG KeyLength(ULONG keylength) {return KEYLENGTH;}

private:
	UCHAR x1[8];
	DES d2;
	UCHAR x3[8];
};

#endif
