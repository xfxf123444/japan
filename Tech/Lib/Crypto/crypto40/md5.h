#ifndef CRYPTOPP_MD5_H
#define CRYPTOPP_MD5_H

#include "iterhash.h"

NAMESPACE_BEGIN(CryptoPP)

class MD5 : public IteratedHash<word32, false, 64>
{
public:
	enum {DIGESTSIZE = 16};
	MD5() : IteratedHash<word32, false, 64>(DIGESTSIZE) {Init();}
	static void Transform(word32 *digest, const word32 *data);

protected:
	void Init();
	void vTransform(const word32 *data) {Transform(digest, data);}
};

NAMESPACE_END

#endif
