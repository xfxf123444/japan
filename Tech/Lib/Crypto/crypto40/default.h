#ifndef CRYPTOPP_DEFAULT_H
#define CRYPTOPP_DEFAULT_H

#include "sha.h"
#include "hmac.h"
#include "des.h"
#include "filters.h"

NAMESPACE_BEGIN(CryptoPP)

typedef DES_EDE2_Encryption Default_ECB_Encryption;
typedef DES_EDE2_Decryption Default_ECB_Decryption;
typedef SHA DefaultHashModule;
typedef HMAC<DefaultHashModule> DefaultMAC;

class DefaultEncryptor : public ProxyFilter
{
public:
	DefaultEncryptor(const char *passphrase, BufferedTransformation *outQueue = NULL);

protected:
	void FirstPut(const byte *);
	void LastPut(const byte *inString, unsigned int length);

private:
	SecByteBlock m_passphrase;
	member_ptr<Default_ECB_Encryption> m_cipher;
};

class DefaultDecryptor : public ProxyFilter
{
public:
	DefaultDecryptor(const char *passphrase, BufferedTransformation *outQueue = NULL, bool throwException=true);

	class Err : public BufferedTransformation::Err
	{
	public:
		Err(const std::string &s) 
			: BufferedTransformation::Err(DATA_INTEGRITY_CHECK_FAILED, s) {}
	};
	class KeyBadErr : public Err {public: KeyBadErr() : Err("DefaultDecryptor: cannot decrypt message with this passphrase") {}};

	enum State {WAITING_FOR_KEYCHECK, KEY_GOOD, KEY_BAD};
	State CurrentState() const {return m_state;}

protected:
	void FirstPut(const byte *inString);
	void LastPut(const byte *inString, unsigned int length);

	State m_state;

private:
	void CheckKey(const byte *salt, const byte *keyCheck);

	SecByteBlock m_passphrase;
	member_ptr<Default_ECB_Decryption> m_cipher;
	member_ptr<FilterWithBufferedInput> m_decryptor;
	bool m_throwException;
};

class DefaultEncryptorWithMAC : public ProxyFilter
{
public:
	DefaultEncryptorWithMAC(const char *passphrase, BufferedTransformation *outQueue = NULL);

protected:
	void FirstPut(const byte *inString) {}
	void LastPut(const byte *inString, unsigned int length);

private:
	member_ptr<DefaultMAC> m_mac;
};

class DefaultDecryptorWithMAC : public ProxyFilter
{
public:
	class MACBadErr : public DefaultDecryptor::Err {public: MACBadErr() : DefaultDecryptor::Err("DefaultDecryptorWithMAC: MAC check failed") {}};

	DefaultDecryptorWithMAC(const char *passphrase, BufferedTransformation *outQueue = NULL, bool throwException=true);

	DefaultDecryptor::State CurrentState() const;
	bool CheckLastMAC() const;

protected:
	void FirstPut(const byte *inString) {}
	void LastPut(const byte *inString, unsigned int length);

private:
	member_ptr<DefaultMAC> m_mac;
	HashVerifier *m_hashVerifier;
	bool m_throwException;
};

NAMESPACE_END

#endif
