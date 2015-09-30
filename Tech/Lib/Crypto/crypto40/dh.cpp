// dh.cpp - written and placed in the public domain by Wei Dai

#include "pch.h"
#include "dh.h"
#include "asn.h"
#include "nbtheory.h"

NAMESPACE_BEGIN(CryptoPP)

DH::DH(const Integer &p, const Integer &g)
	: p(p), g(g), gpc(p, g)
{
}

DH::DH(RandomNumberGenerator &rng, unsigned int pbits)
{
	PrimeAndGenerator pg(1, rng, pbits);
	p = pg.Prime();
	g = pg.Generator();
	gpc.SetModulusAndBase(p, g);
}

DH::DH(BufferedTransformation &bt)
{
	BERSequenceDecoder seq(bt);
	p.BERDecode(seq);
	g.BERDecode(seq);
	seq.MessageEnd();

	gpc.SetModulusAndBase(p, g);
}

void DH::DEREncode(BufferedTransformation &bt) const
{
	DERSequenceEncoder seq(bt);
	p.DEREncode(seq);
	g.DEREncode(seq);
	seq.MessageEnd();
}

void DH::Precompute(unsigned int precomputationStorage)
{
	gpc.Precompute(ExponentBitLength(), precomputationStorage);
}

void DH::LoadPrecomputation(BufferedTransformation &bt)
{
	gpc.Load(bt);
}

void DH::SavePrecomputation(BufferedTransformation &bt) const
{
	gpc.Save(bt);
}

bool DH::ValidateDomainParameters(RandomNumberGenerator &rng) const
{
	return VerifyPrime(rng, p) && VerifyPrime(rng, (p-1)/2) && g > 1 && g < p && Jacobi(g, p) == 1;
}

void DH::GenerateKeyPair(RandomNumberGenerator &rng, byte *privateKey, byte *publicKey) const
{
	Integer x(rng, ExponentBitLength());
	Integer y = gpc.Exponentiate(x);
	x.Encode(privateKey, PrivateKeyLength());
	y.Encode(publicKey, PublicKeyLength());
}

bool DH::Agree(byte *agreedValue, const byte *privateKey, const byte *otherPublicKey, bool validateOtherPublicKey) const
{
	Integer w(otherPublicKey, PublicKeyLength());
	// verifying that Jacobi(w, p) == 1 is omitted because it's too costly
	// and at most 1 bit is leaked if it's false
	if (validateOtherPublicKey && !(w > 1 && w < p))
		return false;

	Integer s(privateKey, PrivateKeyLength());
	Integer z = a_exp_b_mod_c(w, s, p);
	z.Encode(agreedValue, AgreedValueLength());
	return true;
}

unsigned int DH::ExponentBitLength() const
{
	return 2*DiscreteLogWorkFactor(p.BitCount());
}

NAMESPACE_END
