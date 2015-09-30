#ifndef CRYPTOPP_MD5MAC_H
#define CRYPTOPP_MD5MAC_H

#include "iterhash.h"

NAMESPACE_BEGIN(CryptoPP)

class MD5MAC : public IteratedHash<word32, false, 64>, public MessageAuthenticationCode
{
public:
	enum {KEYLENGTH=16, DIGESTSIZE = 16};
	MD5MAC(const byte *userKey);
	void Final(byte *mac);

protected:
	static void Transform (word32 *buf, const word32 *in, const word32 *key);
	void vTransform(const word32 *data) {Transform(digest, data, key+4);}
	void Init();

	static const word32 T[12];
	SecBlock<word32> key;
};

NAMESPACE_END

#endif
