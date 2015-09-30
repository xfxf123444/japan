#ifndef CRYPTOPP_ECP_H
#define CRYPTOPP_ECP_H

#include "modarith.h"
#include "eprecomp.h"
#include "smartptr.h"

NAMESPACE_BEGIN(CryptoPP)

struct ECPPoint
{
	ECPPoint() : identity(true) {}
	ECPPoint(const Integer &x, const Integer &y)
		: identity(false), x(x), y(y) {}

	bool operator==(const ECPPoint &t) const
		{return (identity && t.identity) || (!identity && !t.identity && x==t.x && y==t.y);}
	bool operator< (const ECPPoint &t) const
		{return identity ? !t.identity : (t.identity && (x<t.x || (x<=t.x && y<t.y)));}

	bool identity;
	Integer x, y;
};

class ECP : public AbstractGroup<ECPPoint>
{
public:
	typedef ModularArithmetic Field;
	typedef Integer FieldElement;

	typedef ECPPoint Point;

	ECP(const ECP &ecp)
		: fieldPtr(new Field(ecp.field.GetModulus())), field(*fieldPtr), a(ecp.a), b(ecp.b) {}
	ECP(const Integer &modulus, const FieldElement &a, const FieldElement &b)
		: fieldPtr(new Field(modulus)), field(*fieldPtr), a(a.IsNegative() ? modulus+a : a), b(b) {}
	ECP(const MontgomeryRepresentation &mr, const FieldElement &a, const FieldElement &b)
		: field(mr), a(a), b(b) {}
	// construct from BER encoded parameters
	// this constructor will decode and extract the the fields fieldID and curve of the sequence ECParameters
	ECP(BufferedTransformation &bt);

	// encode the fields fieldID and curve of the sequence ECParameters
	void DEREncode(BufferedTransformation &bt) const;

	bool Equal(const Point &P, const Point &Q) const;
	const Point& Zero() const {static const Point zero; return zero;}
	const Point& Inverse(const Point &P) const;
	const Point& Add(const Point &P, const Point &Q) const;
	const Point& Double(const Point &P) const;
	Point ScalarMultiply(const Point &P, const Integer &k) const;
	Point Multiply(const Integer &k, const Point &P) const;
	Point CascadeMultiply(const Integer &k1, const Point &P, const Integer &k2, const Point &Q) const;

	bool ValidateParameters(RandomNumberGenerator &rng) const;
	bool VerifyPoint(const Point &P) const;

	unsigned int EncodedPointSize(bool compressed = false) const
		{return 1 + (compressed?1:2)*field.MaxElementByteLength();}
	// returns false if point is compressed and not valid (doesn't check if uncompressed)
	bool DecodePoint(Point &P, const byte *encodedPoint, unsigned int len) const;
	void EncodePoint(byte *encodedPoint, const Point &P, bool compressed = false) const;

	Point BERDecodePoint(BufferedTransformation &bt);
	void DEREncodePoint(BufferedTransformation &bt, const Point &P, bool compressed = false) const;

	Integer FieldSize() const {return field.GetModulus();}
	const Field & GetField() const {return field;}
	const FieldElement & GetA() const {return a;}
	const FieldElement & GetB() const {return b;}

private:
	member_ptr<Field> fieldPtr;
	const Field &field;
	FieldElement a, b;
	mutable Point R;
};

template <class T> class EcPrecomputation;

template<> class EcPrecomputation<ECP>
{
public:
	EcPrecomputation() {}
	EcPrecomputation(const EcPrecomputation &a)
		{operator=(a);}
	EcPrecomputation(const ECP &ec, const ECP::Point &base)
		{SetCurveAndBase(ec, base);}

	EcPrecomputation& operator=(const EcPrecomputation &rhs);

	void SetCurveAndBase(const ECP &ec, const ECP::Point &base);
	void Precompute(unsigned int maxExpBits, unsigned int storage);
	void Load(BufferedTransformation &storedPrecomputation);
	void Save(BufferedTransformation &storedPrecomputation) const;

	ECP::Point Multiply(const Integer &exponent) const;
	ECP::Point CascadeMultiply(const Integer &exponent, const EcPrecomputation<ECP> &pc2, const Integer &exponent2) const;

private:
	value_ptr<MontgomeryRepresentation> m_mr;
	value_ptr<ECP> m_ec;
	ExponentiationPrecomputation<ECP::Point> m_ep;
};

NAMESPACE_END

#endif
