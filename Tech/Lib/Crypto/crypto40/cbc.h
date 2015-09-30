#ifndef CRYPTOPP_CBC_H
#define CRYPTOPP_CBC_H

#include "filters.h"
#include "modes.h"

NAMESPACE_BEGIN(CryptoPP)

/// CBC mode encryptor

/** Compatible with FIPS 81.
	Padded with '\0's if plaintext length is not a multiple of block size.
*/
class CBCRawEncryptor : protected CipherMode, public FilterWithBufferedInput
{
public:
	CBCRawEncryptor(const BlockTransformation &cipher, const byte *IV, BufferedTransformation *outQueue = NULL);

protected:
	void FirstPut(const byte *) {}
	void NextPut(const byte *inString, unsigned int length);
	void LastPut(const byte *inString, unsigned int length);
};

class CBCRawDecryptor : protected CipherMode, public FilterWithBufferedInput
{
public:
	CBCRawDecryptor(const BlockTransformation &cipher, const byte *IV, BufferedTransformation *outQueue = NULL);

protected:
	void FirstPut(const byte *) {}
	void NextPut(const byte *inString, unsigned int length);
	void LastPut(const byte *inString, unsigned int length);
};

/// CBC mode encryptor with padding

/** Compatible with RFC 1423 (and also 2040 and 2630).
*/
class CBCPaddedEncryptor : protected CipherMode, public FilterWithBufferedInput
{
public:
	CBCPaddedEncryptor(const BlockTransformation &cipher, const byte *IV, BufferedTransformation *outQueue = NULL);

protected:
	void FirstPut(const byte *) {}
	void NextPut(const byte *inString, unsigned int length);
	void LastPut(const byte *inString, unsigned int length);
};

class CBCPaddedDecryptor : protected CipherMode, public FilterWithBufferedInput
{
public:
	CBCPaddedDecryptor(const BlockTransformation &cipher, const byte *IV, BufferedTransformation *outQueue = NULL);

protected:
	void FirstPut(const byte *) {}
	void NextPut(const byte *inString, unsigned int length);
	void LastPut(const byte *inString, unsigned int length);
};

/// CBC mode encryptor with ciphertext stealing

/** Compatible with RFC 2040.
	Ciphertext stealing requires at least cipher.BlockSize()+1 bytes of plaintext.
	Shorter plaintext will be padded with '\0's.
*/
class CBC_CTS_Encryptor : protected CipherMode, public FilterWithBufferedInput
{
public:
	CBC_CTS_Encryptor(const BlockTransformation &cipher, const byte *IV, BufferedTransformation *outQueue = NULL);

protected:
	void FirstPut(const byte *inString);
	void NextPut(const byte *inString, unsigned int length);
	void LastPut(const byte *inString, unsigned int length);
};

class CBC_CTS_Decryptor : protected CipherMode, public FilterWithBufferedInput
{
public:
	CBC_CTS_Decryptor(const BlockTransformation &cipher, const byte *IV, BufferedTransformation *outQueue = NULL);

protected:
	void FirstPut(const byte *inString) {}
	void NextPut(const byte *inString, unsigned int length);
	void LastPut(const byte *inString, unsigned int length);
};

NAMESPACE_END

#endif
