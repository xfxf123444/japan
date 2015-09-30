#ifndef CRYPTOPP_TIGER_H
#define CRYPTOPP_TIGER_H

#include "config.h"

#ifdef WORD64_AVAILABLE

#include "iterhash.h"

NAMESPACE_BEGIN(CryptoPP)

class Tiger : public IteratedHash<word64, false, 64>
{
public:
	enum {DIGESTSIZE = 24};
	Tiger() : IteratedHash<word64, false, 64>(DIGESTSIZE) {Init();}
	static void Transform(word64 *digest, const word64 *data);
	void Final(byte *hash);

protected:
	void Init();
	void vTransform(const word64 *data) {Transform(digest, data);}

	static const word64 table[4*256];
};

NAMESPACE_END

#endif

#endif
