#ifndef CRYPTOPP_ADLER32_H
#define CRYPTOPP_ADLER32_H

#include "cryptlib.h"

NAMESPACE_BEGIN(CryptoPP)

class Adler32 : public HashModule
{
public:
	Adler32() {Reset();}
	void Update(const byte *input, unsigned int length);
	void Final(byte *hash);
	unsigned int DigestSize() const {return 4;}

	void Reset() {m_s1 = 1; m_s2 = 0;}

private:
	word16 m_s1, m_s2;
};

NAMESPACE_END

#endif
