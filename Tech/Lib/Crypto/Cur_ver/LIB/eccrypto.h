#ifndef CRYPTOPP_ECCRYPTO_H
#define CRYPTOPP_ECCRTPTO_H

#include "pubkey.h"
#include "integer.h"
#include "asn.h"
#include "hmac.h"
#include "sha.h"

NAMESPACE_BEGIN(CryptoPP)

/* The following classes are explicitly instantiated in eccrypto.cpp

ECPublicKey<EC2N, ECDSA>;
ECPublicKey<ECP, ECDSA>;
ECPrivateKey<EC2N, ECDSA>;
ECPrivateKey<ECP, ECDSA>;
ECPublicKey<EC2N, ECNR>;
ECPublicKey<ECP, ECNR>;
ECPrivateKey<EC2N, ECNR>;
ECPrivateKey<ECP, ECNR>;
ECDHC<EC2N>;
ECDHC<ECP>;
ECMQVC<EC2N>;
ECMQVC<ECP>;
*/

// The ECDSA signature format used by Crypto++ is as defined by IEEE P1363.
// To convert to or from other signature formats, see dsa.h.

enum ECSignatureScheme {ECNR, ECDSA};

template <class T> class EcPrecomputation;

// this class corresponds to the ASN.1 sequence of the same name
// in ANSI X9.62 (also SEC 1)
template <class EC>
class ECParameters : virtual public PK_Precomputation
{
public:
	typedef typename EC::Point Point;

	ECParameters() : m_compress(false) {}
	ECParameters(const EC &ec, const Point &G, const Integer &n)
		: m_ec(ec), m_G(G), m_Gpc(*m_ec, G), m_n(n), m_cofactorPresent(false), m_compress(false) {}
	ECParameters(const EC &ec, const Point &G, const Integer &n, const Integer &k)
		: m_ec(ec), m_G(G), m_Gpc(*m_ec, G), m_n(n), m_cofactorPresent(true), m_compress(false), m_k(k) {}
	ECParameters(BufferedTransformation &bt)
		: m_compress(false) {BERDecode(bt);}

	void BERDecode(BufferedTransformation &bt);
	void DEREncode(BufferedTransformation &bt) const;

	bool ValidateParameters(RandomNumberGenerator &rng) const;

	void Precompute(unsigned int precomputationStorage=16);
	void LoadPrecomputation(BufferedTransformation &storedPrecomputation);
	void SavePrecomputation(BufferedTransformation &storedPrecomputation) const;

	void SetPointCompression(bool compress) {m_compress = compress;}
	bool GetPointCompression() const {return m_compress;}

	const EC& GetCurve() const {return *m_ec;}
	const Point& GetBasePoint() const {return m_G;}
	const Integer& GetBasePointOrder() const {return m_n;}
	const bool CofactorPresent() const {return m_cofactorPresent;}
	const Integer& GetCofactor() const {return m_k;}

protected:
	unsigned int EncodedPointSize() const {return m_ec->EncodedPointSize(m_compress);}
	void EncodePoint(byte *encodedPoint, const Point &P) const {m_ec->EncodePoint(encodedPoint, P, m_compress);}
	unsigned int FieldElementLength() const {return m_ec->GetField().MaxElementByteLength();}
	unsigned int ExponentLength() const {return m_n.ByteCount();}
	unsigned int ExponentBitLength() const {return m_n.BitCount();}

	value_ptr<EC> m_ec;	// field and curve
	Point m_G;			// base
	EcPrecomputation<EC> m_Gpc;	// precomputed table for base
	Integer m_n;		// order
	bool m_cofactorPresent, m_compress;
	Integer m_k;		// cofactor
};

template <class EC>
class ECPublicKey : public ECParameters<EC>, virtual public PK_Precomputation
{
public:
	typedef typename EC::Point Point;
	
	ECPublicKey(const EC &ec, const Point &G, const Integer &n, const Point &Q)
		: ECParameters<EC>(ec, G, n), m_Q(Q), m_Qpc(ec, m_Q) {}
	// construct from a SubjectPublicKeyInfo sequence
	ECPublicKey(BufferedTransformation &bt);

	// encode to a SubjectPublicKeyInfo sequence
	void DEREncode(BufferedTransformation &bt) const;

	void Precompute(unsigned int precomputationStorage=16);
	void LoadPrecomputation(BufferedTransformation &storedPrecomputation);
	void SavePrecomputation(BufferedTransformation &storedPrecomputation) const;

	const Point& GetPublicPoint() const {return m_Q;}

protected:
	ECPublicKey() {}
	Integer EncodeDigest(ECSignatureScheme ss, const byte *digest, unsigned int digestLen) const;

	Point m_Q;
	EcPrecomputation<EC> m_Qpc;
};

template <class EC>
class ECPrivateKey : public ECPublicKey<EC>
{
public:
	typedef typename EC::Point Point;

	ECPrivateKey(const EC &ec, const Point &G, const Integer &n, const Point &Q, const Integer &d)
		: ECPublicKey<EC>(ec, G, n, Q), m_d(d) {}
	// generate a random private key
	ECPrivateKey(RandomNumberGenerator &rng, const EC &ec, const Point &G, const Integer &n)
		: ECPublicKey<EC>(ec, G, n, Point()) {Randomize(rng);}
	ECPrivateKey(BufferedTransformation &bt);

	void DEREncode(BufferedTransformation &bt) const;

	const Integer& GetPrivateExponent() const {return m_d;}

protected:
	typedef typename EC::FieldElement FieldElement;
	void Randomize(RandomNumberGenerator &rng);

	Integer m_d;
};

template <class EC, ECSignatureScheme SS = ECNR>
class ECDigestVerifier : public ECPublicKey<EC>, public PK_WithPrecomputation<DigestVerifier>
{
public:
	typedef typename EC::Point Point;
	
	ECDigestVerifier(const ECPublicKey<EC> &key)
		: ECPublicKey<EC>(key) {}
	ECDigestVerifier(const EC &ec, const Point &G, const Integer &n, const Point &Q)
		: ECPublicKey<EC>(ec, G, n, Q) {}
	ECDigestVerifier(BufferedTransformation &bt)
		: ECPublicKey<EC>(bt) {}

	bool VerifyDigest(const byte *digest, unsigned int digestLen, const byte *signature) const;

	unsigned int MaxDigestLength() const {return 0xffff;}
	unsigned int DigestSignatureLength() const {return 2*ExponentLength();}

	// exposed for validation testing
	bool RawVerify(const Integer &e, const Integer &n, const Integer &s) const;
};

template <class EC, ECSignatureScheme SS = ECNR>
class ECDigestSigner : public ECPrivateKey<EC>, public PK_WithPrecomputation<DigestSigner>
{
public:
	typedef typename EC::Point Point;
	
	ECDigestSigner(const ECPrivateKey<EC> &key)
		: ECPrivateKey<EC>(key) {}
	ECDigestSigner(const EC &ec, const Point &G, const Integer &n, const Point &Q, const Integer &d)
		: ECPrivateKey<EC>(ec, G, n, Q, d) {}
	// generate a random private key
	ECDigestSigner(RandomNumberGenerator &rng, const EC &ec, const Point &G, const Integer &n)
		: ECPrivateKey<EC>(rng, ec, G, n) {}

	void SignDigest(RandomNumberGenerator &, const byte *digest, unsigned int digestLen, byte *signature) const;

	unsigned int MaxDigestLength() const {return 0xffff;}
	unsigned int DigestSignatureLength() const {return 2*ExponentLength();}

	// exposed for validation testing
	void RawSign(const Integer &k, const Integer &e, Integer &n, Integer &s) const;
};

template <class EC, class H, ECSignatureScheme SS = ECNR>
class ECSigner : public SignerTemplate<ECDigestSigner<EC, SS>, H>, public PK_WithPrecomputation<PK_Signer>
{
	typedef SignerTemplate<ECDigestSigner<EC, SS>, H> Base;
public:
	typedef typename EC::Point Point;
	
	ECSigner(const ECPrivateKey<EC> &key)
		: Base(ECDigestSigner<EC, SS>(key)) {}
	ECSigner(const EC &ec, const Point &G, const Integer &n, const Point &Q, const Integer &d)
		: Base(ECDigestSigner<EC, SS>(ec, G, n, Q, d)) {}
	// generate a random private key
	ECSigner(RandomNumberGenerator &rng, const EC &ec, const Point &G, const Integer &n, const Point &Q, const Integer &d)
		: Base(ECDigestSigner<EC, SS>(rng, ec, G, n, Q)) {}
};

template <class EC, class H, ECSignatureScheme SS = ECNR>
class ECVerifier : public VerifierTemplate<ECDigestVerifier<EC, SS>, H>, public PK_WithPrecomputation<PK_Verifier>
{
	typedef VerifierTemplate<ECDigestVerifier<EC, SS>, H> Base;
public:
	typedef typename EC::Point Point;
	
	ECVerifier(const ECPublicKey<EC> &key)
		: Base(ECDigestVerifier<EC, SS>(key)) {}
	ECVerifier(const EC &ec, const Point &G, const Integer &n, const Point &Q)
		: Base(ECDigestVerifier<EC, SS>(ec, G, n, Q)) {}
	ECVerifier(BufferedTransformation &bt)
		: Base(ECDigestVerifier<EC, SS>(bt)) {}
};

template <class EC, class MAC = HMAC<SHA>, class KDF = P1363_KDF2<SHA> >
class ECEncryptor : public ECPublicKey<EC>, public PK_WithPrecomputation<PK_Encryptor>
{
public:
	typedef typename EC::Point Point;
	
	ECEncryptor(const ECPublicKey<EC> &key)
		: ECPublicKey<EC>(key) {}
	ECEncryptor(const EC &ec, const Point &G, const Integer &n, const Point &Q)
		: ECPublicKey<EC>(ec, G, n, Q) {}
	ECEncryptor(BufferedTransformation &bt)
		: ECPublicKey<EC>(bt) {}

	unsigned int MaxPlainTextLength(unsigned int cipherTextLength) const
		{return cipherTextLength < CipherTextLength(0) ? 0 : cipherTextLength - CipherTextLength(0);}
	unsigned int CipherTextLength(unsigned int plainTextLength) const
		{return plainTextLength + MAC::DIGESTSIZE + EncodedPointSize();}

	void Encrypt(RandomNumberGenerator &rng, const byte *plainText, unsigned int plainTextLength, byte *cipherText)
	{
		Integer x(rng, 1, m_n-1);
		Point Q = m_Gpc.Multiply(x);

		EncodePoint(cipherText, Q);
		cipherText += EncodedPointSize();

		SecByteBlock agreedSecret(FieldElementLength());
		Point Q1 = m_Qpc.Multiply(x);
		Q1.x.Encode(agreedSecret, agreedSecret.size);

		SecByteBlock derivedKey(plainTextLength + MAC::KEYLENGTH);
		KDF::DeriveKey(derivedKey, derivedKey.size, agreedSecret, agreedSecret.size);
		xorbuf(cipherText, plainText, derivedKey, plainTextLength);

		MAC mac(derivedKey + plainTextLength);
		mac.CalculateDigest(cipherText + plainTextLength, cipherText, plainTextLength);
	}
};

template <class EC, class MAC = HMAC<SHA>, class KDF = P1363_KDF2<SHA> >
class ECDecryptor : public ECPrivateKey<EC>, public PK_WithPrecomputation<PK_Decryptor>
{
public:
	typedef typename EC::Point Point;
	
	ECDecryptor(const ECPrivateKey<EC> &key)
		: ECPrivateKey<EC>(key) {}
	ECDecryptor(const EC &ec, const Point &G, const Integer &n, const Point &Q, const Integer &d)
		: ECPrivateKey<EC>(ec, G, n, Q, d) {}
	// generate a random private key
	ECDecryptor(RandomNumberGenerator &rng, const EC &ec, const Point &G, const Integer &n)
		: ECPrivateKey<EC>(rng, ec, G, n) {}

	unsigned int MaxPlainTextLength(unsigned int cipherTextLength) const
		{return cipherTextLength < CipherTextLength(0) ? 0 : cipherTextLength - CipherTextLength(0);}
	unsigned int CipherTextLength(unsigned int plainTextLength) const
		{return plainTextLength + MAC::DIGESTSIZE + EncodedPointSize();}

	unsigned int Decrypt(const byte *cipherText, unsigned int cipherTextLength, byte *plainText)
	{
		Point Q;
		if (!GetCurve().DecodePoint(Q, cipherText, EncodedPointSize()) || !GetCurve().VerifyPoint(Q) || Q.identity)
			return 0;
		cipherText += EncodedPointSize();

		const Integer e[2] = {m_n, m_d};
		Point R[2];
		SimultaneousMultiplication(R, GetCurve(), Q, e, e+2);

		if (!R[0].identity)
			return 0;

		SecByteBlock agreedSecret(FieldElementLength());
		R[1].x.Encode(agreedSecret, agreedSecret.size);

		unsigned int plainTextLength = MaxPlainTextLength(cipherTextLength);
		SecByteBlock derivedKey(plainTextLength + MAC::KEYLENGTH);
		KDF::DeriveKey(derivedKey, derivedKey.size, agreedSecret, agreedSecret.size);
		
		MAC mac(derivedKey + plainTextLength);
		if (!mac.VerifyDigest(cipherText + plainTextLength, cipherText, plainTextLength))
			return 0;

		xorbuf(plainText, cipherText, derivedKey, plainTextLength);
		return plainTextLength;
	}
};

// Elliptic Curve Diffie-Hellman with Cofactor Multiplication
template <class EC>
class ECDHC : public ECParameters<EC>, public PK_WithPrecomputation<PK_SimpleKeyAgreementDomain>
{
public:
	typedef typename EC::Point Point;
	
	// G is a point of prime order n, k is order of ec divided by n
	ECDHC(const EC &ec, const Point &G, const Integer &n, const Integer &k)
		: ECParameters<EC>(ec, G, n, k) {}

	bool ValidateDomainParameters(RandomNumberGenerator &rng) const
		{return ValidateParameters(rng);}
	unsigned int AgreedValueLength() const {return FieldElementLength();}
	unsigned int PrivateKeyLength() const {return ExponentLength();}
	unsigned int PublicKeyLength() const {return EncodedPointSize();}

	void GenerateKeyPair(RandomNumberGenerator &rng, byte *privateKey, byte *publicKey) const;
	bool Agree(byte *agreedValue, const byte *privateKey, const byte *otherPublicKey, bool validateOtherPublicKey=true) const;
};

// Elliptic Curve Menezes-Qu-Vanstone with Cofactor Multiplication
template <class EC>
class ECMQVC : public ECParameters<EC>, public PK_WithPrecomputation<PK_AuthenticatedKeyAgreementDomain>
{
public:
	typedef typename EC::Point Point;
	
	// G is a point of prime order n, k is order of ec divided by n
	ECMQVC(const EC &ec, const Point &G, const Integer &n, const Integer &k)
		: ECParameters<EC>(ec, G, n, k) {}

	bool ValidateDomainParameters(RandomNumberGenerator &rng) const
		{return ValidateParameters(rng);}
	unsigned int AgreedValueLength() const {return FieldElementLength();}

	unsigned int StaticPrivateKeyLength() const {return ExponentLength();}
	unsigned int StaticPublicKeyLength() const {return EncodedPointSize();}
	void GenerateStaticKeyPair(RandomNumberGenerator &rng, byte *privateKey, byte *publicKey) const;

	unsigned int EphemeralPrivateKeyLength() const {return ExponentLength()+EncodedPointSize();}
	unsigned int EphemeralPublicKeyLength() const {return EncodedPointSize();}
	void GenerateEphemeralKeyPair(RandomNumberGenerator &rng, byte *privateKey, byte *publicKey) const;

	bool Agree(byte *agreedValue,
		const byte *staticPrivateKey, const byte *ephemeralPrivateKey, 
		const byte *staticOtherPublicKey, const byte *ephemeralOtherPublicKey,
		bool validateStaticOtherPublicKey=true) const;
};

NAMESPACE_END

#endif
