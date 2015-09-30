#ifndef CRYPTOPP_HAVAL_H
#define CRYPTOPP_HAVAL_H

#include "iterhash.h"

NAMESPACE_BEGIN(CryptoPP)

class HAVAL : public IteratedHash<word32, false, 128>
{
public:
	enum {DIGESTSIZE = 32, VERSION = 1};

	// digestSize can be 16, 20, 24, 28, or 32
	// pass can be 3, 4 or 5
	HAVAL(unsigned int digestSize=DIGESTSIZE, unsigned int passes=3);
	void Final(byte *hash);
	unsigned int DigestSize() const {return digestSize;}

protected:
	static const unsigned int wi2[32], wi3[32], wi4[32], wi5[32];
	static const word32 mc2[32], mc3[32], mc4[32], mc5[32];

	void Init();
	void Tailor(unsigned int FPTLEN);
	void vTransform(const word32 *in);

	const unsigned int digestSize, pass;
};

class HAVAL3 : public HAVAL
{
public:
	HAVAL3(unsigned int digestSize=DIGESTSIZE) : HAVAL(digestSize, 3) {}
	static void Transform(word32 *buf, const word32 *in);
};

class HAVAL4 : public HAVAL
{
public:
	HAVAL4(unsigned int digestSize=DIGESTSIZE) : HAVAL(digestSize, 4) {}
	static void Transform(word32 *buf, const word32 *in);
};

class HAVAL5 : public HAVAL
{
public:
	HAVAL5(unsigned int digestSize=DIGESTSIZE) : HAVAL(digestSize, 5) {}
	static void Transform(word32 *buf, const word32 *in);
};

NAMESPACE_END

#endif
