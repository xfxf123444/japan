// ec2n.cpp - written and placed in the public domain by Wei Dai

#include "pch.h"
#include "ec2n.h"
#include "asn.h"
#include "nbtheory.h"	// for primeTable

#include "algebra.cpp"
#include "eprecomp.cpp"

NAMESPACE_BEGIN(CryptoPP)

EC2N::EC2N(BufferedTransformation &bt)
	: field(BERDecodeGF2NP(bt))
{
	BERSequenceDecoder seq(bt);
	field->BERDecodeElement(seq, a);
	field->BERDecodeElement(seq, b);
	// skip optional seed
	if (!seq.EndReached())
		BERDecodeOctetString(seq, BitBucket());
	seq.MessageEnd();
}

void EC2N::DEREncode(BufferedTransformation &bt) const
{
	field->DEREncode(bt);
	DERSequenceEncoder seq(bt);
	field->DEREncodeElement(seq, a);
	field->DEREncodeElement(seq, b);
	seq.MessageEnd();
}

bool EC2N::DecodePoint(EC2N::Point &P, const byte *encodedPoint, unsigned int encodedPointLen) const
{
	if (encodedPointLen < 1)
		return false;

	switch (encodedPoint[0])
	{
	case 0:
		P.identity = true;
		return true;
	case 2:
	case 3:
	{
		if (encodedPointLen != EncodedPointSize(true))
			return false;

		P.identity = false;
		P.x.Decode(encodedPoint+1, field->MaxElementByteLength()); 

		if (P.x.IsZero())
		{
			P.y = field->SquareRoot(P.x);
			return true;
		}

		P.y = field->Square(P.x);
		FieldElement z = field->Divide(field->Add(field->Multiply(P.y, field->Add(P.x, a)), b), P.y);
		z = field->SolveQuadraticEquation(z);
		z.SetCoefficient(0, encodedPoint[0] & 1);

		P.y = field->Multiply(z, P.x);
		return true;
	}
	case 4:
	{
		if (encodedPointLen != EncodedPointSize(false))
			return false;

		unsigned int len = field->MaxElementByteLength();
		P.identity = false;
		P.x.Decode(encodedPoint+1, len);
		P.y.Decode(encodedPoint+1+len, len);
		return true;
	}
	default:
		return false;
	}
}

void EC2N::EncodePoint(byte *encodedPoint, const Point &P, bool compressed) const
{
	if (P.identity)
		memset(encodedPoint, 0, EncodedPointSize(compressed));
	else if (compressed)
	{
		encodedPoint[0] = 2 + (!P.x ? 0 : field->Divide(P.y, P.x).GetBit(0));
		P.x.Encode(encodedPoint+1, field->MaxElementByteLength());
	}
	else
	{
		unsigned int len = field->MaxElementByteLength();
		encodedPoint[0] = 4;	// uncompressed
		P.x.Encode(encodedPoint+1, len);
		P.y.Encode(encodedPoint+1+len, len);
	}
}

EC2N::Point EC2N::BERDecodePoint(BufferedTransformation &bt) const
{
	SecByteBlock str;
	BERDecodeOctetString(bt, str);
	Point P;
	if (!DecodePoint(P, str, str.size))
		BERDecodeError();
	return P;
}

void EC2N::DEREncodePoint(BufferedTransformation &bt, const Point &P, bool compressed) const
{
	SecByteBlock str(EncodedPointSize(compressed));
	EncodePoint(str, P, compressed);
	DEREncodeOctetString(bt, str);
}

bool EC2N::ValidateParameters(RandomNumberGenerator &rng) const
{
	return field->GetModulus().IsIrreducible()
		&& a.CoefficientCount() <= field->MaxElementBitLength()
		&& b.CoefficientCount() <= field->MaxElementBitLength() && !!b;
}

bool EC2N::VerifyPoint(const Point &P) const
{
	const FieldElement &x = P.x, &y = P.y;
	return P.identity || 
		(x.CoefficientCount() <= field->MaxElementBitLength()
		&& y.CoefficientCount() <= field->MaxElementBitLength()
		&& !(((x+a)*x*x+b-(x+y)*y)%field->GetModulus()));
}

bool EC2N::Equal(const Point &P, const Point &Q) const
{
	if (P.identity && Q.identity)
		return true;

	if (P.identity && !Q.identity)
		return false;

	if (!P.identity && Q.identity)
		return false;

	return (field->Equal(P.x,Q.x) && field->Equal(P.y,Q.y));
}

const EC2N::Point& EC2N::Inverse(const Point &P) const
{
	if (P.identity)
		return P;
	else
	{
		R.identity = false;
		R.y = field->Add(P.x, P.y);
		R.x = P.x;
		return R;
	}
}

const EC2N::Point& EC2N::Add(const Point &P, const Point &Q) const
{
	if (P.identity) return Q;
	if (Q.identity) return P;
	if (Equal(P, Q)) return Double(P);
	if (field->Equal(P.x, Q.x) && field->Equal(P.y, field->Add(Q.x, Q.y))) return Zero();

	FieldElement t = field->Add(P.y, Q.y);
	t = field->Divide(t, field->Add(P.x, Q.x));
	FieldElement x = field->Square(t);
	field->Accumulate(x, t);
	field->Accumulate(x, Q.x);
	field->Accumulate(x, a);
	R.y = field->Add(P.y, field->Multiply(t, x));
	field->Accumulate(x, P.x);
	field->Accumulate(R.y, x);

	R.x.swap(x);
	R.identity = false;
	return R;
}

const EC2N::Point& EC2N::Double(const Point &P) const
{
	if (P.identity) return P;
	if (!field->IsUnit(P.x)) return Zero();

	FieldElement t = field->Divide(P.y, P.x);
	field->Accumulate(t, P.x);
	R.y = field->Square(P.x);
	R.x = field->Square(t);
	field->Accumulate(R.x, t);
	field->Accumulate(R.x, a);
	field->Accumulate(R.y, field->Multiply(t, R.x));
	field->Accumulate(R.y, R.x);

	R.identity = false;
	return R;
}

// ********************************************************

EcPrecomputation<EC2N>& EcPrecomputation<EC2N>::operator=(const EcPrecomputation<EC2N> &rhs)
{
	m_ec = rhs.m_ec;
	m_ep = rhs.m_ep;
	m_ep.m_group = m_ec.get();
	return *this;
}

void EcPrecomputation<EC2N>::SetCurveAndBase(const EC2N &ec, const EC2N::Point &base)
{
	m_ec.reset(new EC2N(ec));
	m_ep.SetGroupAndBase(*m_ec, base);
}

void EcPrecomputation<EC2N>::Precompute(unsigned int maxExpBits, unsigned int storage)
{
	m_ep.Precompute(maxExpBits, storage);
}

void EcPrecomputation<EC2N>::Load(BufferedTransformation &bt)
{
	BERSequenceDecoder seq(bt);
	word32 version;
	BERDecodeUnsigned<word32>(seq, version, INTEGER, 1, 1);
	m_ep.m_exponentBase.BERDecode(seq);
	m_ep.m_bases.clear();
	while (!seq.EndReached())
		m_ep.m_bases.push_back(m_ec->BERDecodePoint(seq));
	seq.MessageEnd();
}

void EcPrecomputation<EC2N>::Save(BufferedTransformation &bt) const
{
	DERSequenceEncoder seq(bt);
	DEREncodeUnsigned<word32>(seq, 1);	// version
	m_ep.m_exponentBase.DEREncode(seq);
	for (unsigned i=0; i<m_ep.m_bases.size(); i++)
		m_ec->DEREncodePoint(seq, m_ep.m_bases[i]);
	seq.MessageEnd();
}

EC2N::Point EcPrecomputation<EC2N>::Multiply(const Integer &exponent) const
{
	return m_ep.Exponentiate(exponent);
}

EC2N::Point EcPrecomputation<EC2N>::CascadeMultiply(const Integer &exponent, const EcPrecomputation<EC2N> &pc2, const Integer &exponent2) const
{
	return m_ep.CascadeExponentiate(exponent, pc2.m_ep, exponent2);
}

NAMESPACE_END
