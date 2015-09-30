#include "pch.h"
#include "eccrypto.h"
#include "ec2n.h"
#include "ecp.h"
#include "nbtheory.h"
#include "oids.h"

#include "algebra.cpp"

NAMESPACE_BEGIN(CryptoPP)

template void SimultaneousMultiplication<EC2NPoint, EC2NPoint *, const Integer *>(EC2NPoint * result, const AbstractGroup<EC2NPoint> &group, const EC2NPoint &base, const Integer * expBegin, const Integer * expEnd);
template void SimultaneousMultiplication<ECPPoint, ECPPoint *, const Integer *>(ECPPoint * result, const AbstractGroup<ECPPoint> &group, const ECPPoint &base, const Integer * expBegin, const Integer * expEnd);

// VC60 workaround: complains when these functions are put into an anonymous namespace
static Integer ConvertToInteger(const PolynomialMod2 &x)
{
	unsigned int l = x.ByteCount();
	SecByteBlock temp(l);
	x.Encode(temp, l);
	return Integer(temp, l);
}

static inline Integer ConvertToInteger(const Integer &x)
{
	return x;
}

static bool CheckMOVCondition(const Integer &q, const Integer &r)
{
	Integer t=1;
	unsigned int n=q.BitCount(), m=r.BitCount();

	for (unsigned int i=n; DiscreteLogWorkFactor(i)<m/2; i+=n)
	{
		t = (t*q)%r;
		if (t == 1)
			return false;
	}
	return true;
}

// ******************************************************************

template <class EC>
void ECParameters<EC>::BERDecode(BufferedTransformation &bt)
{
	BERSequenceDecoder seq(bt);
	m_ec.reset(new EC(seq));
	m_G = m_ec->BERDecodePoint(seq);
	m_n.BERDecode(seq);
	m_cofactorPresent = !seq.EndReached();
	if (m_cofactorPresent)
		m_k.BERDecode(seq);
	seq.MessageEnd();

	m_Gpc.SetCurveAndBase(GetCurve(), m_G);
}

template <class EC>
void ECParameters<EC>::DEREncode(BufferedTransformation &bt) const
{
	DERSequenceEncoder seq(bt);
	m_ec->DEREncode(seq);
	m_ec->DEREncodePoint(seq, m_G, m_compress);
	m_n.DEREncode(seq);
	if (m_cofactorPresent)
		m_k.DEREncode(seq);
	seq.MessageEnd();
}

template <class EC>
bool ECParameters<EC>::ValidateParameters(RandomNumberGenerator &rng) const
{
	Integer q = m_ec->FieldSize(), qSqrt = q.SquareRoot();

	return m_ec->ValidateParameters(rng) && m_n!=q && m_n>4*qSqrt && VerifyPrime(rng, m_n)
		&& m_ec->VerifyPoint(m_G) && !m_G.identity && m_ec->Multiply(m_n, m_G).identity
		&& m_k==(q+2*qSqrt+1)/m_n && CheckMOVCondition(q, m_n);
}

template <class EC>
void ECParameters<EC>::Precompute(unsigned int precomputationStorage)
{
	m_Gpc.Precompute(ExponentBitLength(), precomputationStorage);
}

template <class EC>
void ECParameters<EC>::LoadPrecomputation(BufferedTransformation &bt)
{
	m_Gpc.Load(bt);
}

template <class EC>
void ECParameters<EC>::SavePrecomputation(BufferedTransformation &bt) const
{
	m_Gpc.Save(bt);
}

// ******************************************************************

template <class EC>
ECPublicKey<EC>::ECPublicKey(BufferedTransformation &bt)
{
	BERSequenceDecoder seq(bt);
	BERSequenceDecoder algorithm(seq);
	if (OID(algorithm) != ASN1::id_ecPublicKey())
		BERDecodeError();
	ECParameters<EC>::BERDecode(algorithm);
	algorithm.MessageEnd();
	SecByteBlock subjectPublicKey;
	unsigned int unusedBits;
	BERDecodeBitString(seq, subjectPublicKey, unusedBits);
	if (!(unusedBits == 0 && m_ec->DecodePoint(m_Q, subjectPublicKey, subjectPublicKey.size)))
		BERDecodeError();
	seq.MessageEnd();

	m_Qpc.SetCurveAndBase(GetCurve(), m_Q);
}

template <class EC>
void ECPublicKey<EC>::DEREncode(BufferedTransformation &bt) const
{
	DERSequenceEncoder seq(bt);
	DERSequenceEncoder algorithm(seq);
	ASN1::id_ecPublicKey().DEREncode(algorithm);
	ECParameters<EC>::DEREncode(algorithm);
	algorithm.MessageEnd();
	SecByteBlock subjectPublicKey(EncodedPointSize());
	EncodePoint(subjectPublicKey, m_Q);
	DEREncodeBitString(seq, subjectPublicKey.ptr, subjectPublicKey.size);
	seq.MessageEnd();
}

template <class EC>
void ECPublicKey<EC>::Precompute(unsigned int precomputationStorage)
{
	m_Gpc.Precompute(ExponentBitLength(), precomputationStorage);
	m_Qpc.Precompute(ExponentBitLength(), precomputationStorage);
}

template <class EC>
void ECPublicKey<EC>::LoadPrecomputation(BufferedTransformation &bt)
{
	m_Gpc.Load(bt);
	m_Qpc.Load(bt);
}

template <class EC>
void ECPublicKey<EC>::SavePrecomputation(BufferedTransformation &bt) const
{
	m_Gpc.Save(bt);
	m_Qpc.Save(bt);
}

template <class EC>
Integer ECPublicKey<EC>::EncodeDigest(ECSignatureScheme ss, const byte *digest, unsigned int digestLen) const
{
	if (ss == ECNR)
		return NR_EncodeDigest(m_n.BitCount(), digest, digestLen);
	else
	{
		assert(ss == ECDSA);
		return DSA_EncodeDigest(m_n.BitCount(), digest, digestLen);
	}
}

// ******************************************************************

template <class EC>
ECPrivateKey<EC>::ECPrivateKey(BufferedTransformation &bt)
{
	BERSequenceDecoder seq(bt);
	word32 version;
	BERDecodeUnsigned<word32>(seq, version, INTEGER, 1, 1);	// check version
	// SEC 1 ver 1.0 says privateKey (m_d) has the same length as order of the curve
	// but there is some confusion so I'll allow any length
	BERGeneralDecoder dec(bt, OCTET_STRING);
	if (!dec.IsDefiniteLength())
		BERDecodeError();
	m_d.Decode(dec, dec.RemainingLength());
	dec.MessageEnd();
	// parameters is optional according to SEC 1, but we need it here
	BERGeneralDecoder parameters(seq, CONTEXT_SPECIFIC | CONSTRUCTED | 0);
	ECParameters<EC>::BERDecode(parameters);
	parameters.MessageEnd();
	if (seq.EndReached())
		m_Q = m_Gpc.Multiply(m_d);
	{
		SecByteBlock subjectPublicKey;
		unsigned int unusedBits;
		BERGeneralDecoder publicKey(seq, CONTEXT_SPECIFIC | CONSTRUCTED | 1);
		BERDecodeBitString(publicKey, subjectPublicKey, unusedBits);
		publicKey.MessageEnd();
		if (!(unusedBits == 0 && m_ec->DecodePoint(m_Q, subjectPublicKey, subjectPublicKey.size)))
			BERDecodeError();
	}
	seq.MessageEnd();

	m_Qpc.SetCurveAndBase(GetCurve(), m_Q);
}

template <class EC>
void ECPrivateKey<EC>::DEREncode(BufferedTransformation &bt) const
{
	DERSequenceEncoder seq(bt);
	DEREncodeUnsigned<word32>(seq, 1);	// version
	// SEC 1 ver 1.0 says privateKey (m_d) has the same length as order of the curve
	// this will be changed to order of base point in a future version
	m_d.DEREncodeAsOctetString(seq, m_n.ByteCount());

	DERGeneralEncoder parameters(seq, CONTEXT_SPECIFIC | CONSTRUCTED | 0);
	ECParameters<EC>::DEREncode(parameters);
	parameters.MessageEnd();

	DERGeneralEncoder publicKey(seq, CONTEXT_SPECIFIC | CONSTRUCTED | 1);
	SecByteBlock subjectPublicKey(EncodedPointSize());
	EncodePoint(subjectPublicKey, m_Q);
	DEREncodeBitString(publicKey, subjectPublicKey.ptr, subjectPublicKey.size);
	publicKey.MessageEnd();

	seq.MessageEnd();
}

template <class EC>
void ECPrivateKey<EC>::Randomize(RandomNumberGenerator &rng)
{
	m_d.Randomize(rng, 1, m_n-1, Integer::ANY);
	m_Q = m_Gpc.Multiply(m_d);
	m_Qpc.SetCurveAndBase(GetCurve(), m_Q);
}

// ******************************************************************

template <class EC, ECSignatureScheme SS>
bool ECDigestVerifier<EC, SS>::RawVerify(const Integer &e, const Integer &r, const Integer &s) const
{
	if (SS == ECNR)
	{
		if (r>=m_n || r<1 || s>=m_n)
			return false;

		// check r == ((r*P + s*P).x + e) % m_n
		Integer x = ConvertToInteger(m_Gpc.CascadeMultiply(s, m_Qpc, r).x);
		return r == (x+e) % m_n;
	}
	else	// ECDSA
	{
		if (r>=m_n || r<1 || s>=m_n || s<1)
			return false;

		Integer w = EuclideanMultiplicativeInverse(s, m_n);
		Integer u1 = (e * w) % m_n;
		Integer u2 = (r * w) % m_n;
		// check r == (u1*P + u2*P).x % n
		return r == ConvertToInteger(m_Gpc.CascadeMultiply(u1, m_Qpc, u2).x) % m_n;
	}
}

template <class EC, ECSignatureScheme SS>
bool ECDigestVerifier<EC, SS>::VerifyDigest(const byte *digest, unsigned int digestLen, const byte *signature) const
{
	assert (digestLen <= MaxDigestLength());

	Integer e = EncodeDigest(SS, digest, digestLen);
	Integer r(signature, ExponentLength());
	Integer s(signature+ExponentLength(), ExponentLength());

	return RawVerify(e, r, s);
}

// ******************************************************************

template <class EC, ECSignatureScheme SS>
void ECDigestSigner<EC, SS>::RawSign(const Integer &k, const Integer &e, Integer &r, Integer &s) const
{
	if (SS == ECNR)
	{
		do
		{
			// convert kP.x into an Integer
			Integer x = ConvertToInteger(m_Gpc.Multiply(k).x);
			r = (x+e) % m_n;
			s = (k-m_d*r) % m_n;
		} while (!r);
	}
	else
	{
		do
		{
			r = ConvertToInteger(m_Gpc.Multiply(k).x) % m_n;
			Integer kInv = EuclideanMultiplicativeInverse(k, m_n);
			s = (kInv * (m_d*r + e)) % m_n;
		} while (!r || !s);
	}
}

template <class EC, ECSignatureScheme SS>
void ECDigestSigner<EC, SS>::SignDigest(RandomNumberGenerator &rng, const byte *digest, unsigned int digestLen, byte *signature) const
{
	Integer r, s;
	Integer e = EncodeDigest(SS, digest, digestLen);
	Integer k(rng, 1, m_n-1, Integer::ANY);

	RawSign(k, e, r, s);

	r.Encode(signature, ExponentLength());
	s.Encode(signature+ExponentLength(), ExponentLength());
}

// ******************************************************************

template <class EC>
void ECDHC<EC>::GenerateKeyPair(RandomNumberGenerator &rng, byte *privateKey, byte *publicKey) const
{
	Integer x(rng, 1, m_n-1);
	Point Q = m_Gpc.Multiply(x);
	x.Encode(privateKey, PrivateKeyLength());
	EncodePoint(publicKey, Q);
}

template <class EC>
bool ECDHC<EC>::Agree(byte *agreedValue, const byte *privateKey, const byte *otherPublicKey, bool validateOtherPublicKey) const
{
	Point W;
	if (!GetCurve().DecodePoint(W, otherPublicKey, PublicKeyLength()))
		return false;
	if (validateOtherPublicKey && !GetCurve().VerifyPoint(W))
		return false;

	Integer s(privateKey, PrivateKeyLength());
	Point Q = GetCurve().Multiply(m_k*s, W);
	if (Q.identity)
		return false;
	Q.x.Encode(agreedValue, AgreedValueLength());
	return true;
}

// ******************************************************************

template <class EC>
void ECMQVC<EC>::GenerateStaticKeyPair(RandomNumberGenerator &rng, byte *privateKey, byte *publicKey) const
{
	Integer x(rng, 1, m_n-1);
	Point Q = m_Gpc.Multiply(x);
	x.Encode(privateKey, StaticPrivateKeyLength());
	EncodePoint(publicKey, Q);
}

template <class EC>
void ECMQVC<EC>::GenerateEphemeralKeyPair(RandomNumberGenerator &rng, byte *privateKey, byte *publicKey) const
{
	Integer x(rng, 1, m_n-1);
	Point Q = m_Gpc.Multiply(x);
	x.Encode(privateKey, ExponentLength());
	EncodePoint(privateKey+ExponentLength(), Q);
	EncodePoint(publicKey, Q);
}

template <class EC>
bool ECMQVC<EC>::Agree(byte *agreedValue, const byte *staticPrivateKey, const byte *ephemeralPrivateKey, const byte *staticOtherPublicKey, const byte *ephemeralOtherPublicKey, bool validateStaticOtherPublicKey) const
{
	Point WW, VV;
	if (!(GetCurve().DecodePoint(WW, staticOtherPublicKey, StaticPublicKeyLength())
		  && GetCurve().DecodePoint(VV, ephemeralOtherPublicKey, EphemeralPublicKeyLength())))
		return false;
	if (!GetCurve().VerifyPoint(VV) || (validateStaticOtherPublicKey && !GetCurve().VerifyPoint(WW)))
		return false;

	Integer s(staticPrivateKey, StaticPrivateKeyLength());
	Integer u(ephemeralPrivateKey, ExponentLength());
	Point V;
	if (!GetCurve().DecodePoint(V, ephemeralPrivateKey+ExponentLength(), EncodedPointSize()))
		return false;

	Integer h2 = Integer::Power2((m_n.BitCount()+1)/2);
	Integer e = ((h2+ConvertToInteger(V.x)%h2)*s+u) % m_n;
	Point Q = GetCurve().CascadeMultiply(m_k*e, VV, m_k*(e*(h2+ConvertToInteger(VV.x)%h2)%m_n), WW);
	if (Q.identity)
		return false;
	Q.x.Encode(agreedValue, AgreedValueLength());
	return true;
}

template class ECParameters<EC2N>;
template class ECParameters<ECP>;
template class ECPrivateKey<EC2N>;
template class ECPrivateKey<ECP>;
template class ECDigestVerifier<EC2N, ECDSA>;
template class ECDigestVerifier<ECP, ECDSA>;
template class ECDigestSigner<EC2N, ECDSA>;
template class ECDigestSigner<ECP, ECDSA>;
template class ECDigestVerifier<EC2N, ECNR>;
template class ECDigestVerifier<ECP, ECNR>;
template class ECDigestSigner<EC2N, ECNR>;
template class ECDigestSigner<ECP, ECNR>;
template class ECDHC<EC2N>;
template class ECDHC<ECP>;
template class ECMQVC<EC2N>;
template class ECMQVC<ECP>;

NAMESPACE_END
