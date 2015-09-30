#ifndef CRYPTOPP_RIPEMD_H
#define CRYPTOPP_RIPEMD_H

#include "iterhash.h"

NAMESPACE_BEGIN(CryptoPP)

class RIPEMD160 : public IteratedHash<word32, false, 64>
{
public:
	enum {DIGESTSIZE = 20};
	RIPEMD160() : IteratedHash<word32, false, 64>(DIGESTSIZE) {Init();}
	static void Transform(word32 *digest, const word32 *data);

protected:
	void Init();
	void vTransform(const word32 *data) {Transform(digest, data);}
};

NAMESPACE_END

#endif
