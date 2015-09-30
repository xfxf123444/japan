#ifndef CRYPTOPP_ALGEBRA_H
#define CRYPTOPP_ALGEBRA_H

#include "config.h"

NAMESPACE_BEGIN(CryptoPP)

class Integer;

// "const Element&" returned by member functions are references
// to internal data members. Since each object may have only
// one such data member for holding results, the following code
// will produce incorrect results:
// abcd = group.Add(group.Add(a,b), group.Add(c,d));
// But this should be fine:
// abcd = group.Add(a, group.Add(b, group.Add(c,d));

template <class T> class AbstractGroup
{
public:
	typedef T Element;

	virtual ~AbstractGroup() {}

	virtual bool Equal(const Element &a, const Element &b) const =0;
	virtual const Element& Zero() const =0;
	virtual const Element& Add(const Element &a, const Element &b) const =0;
	virtual const Element& Inverse(const Element &a) const =0;

	virtual const Element& Double(const Element &a) const;
	virtual const Element& Subtract(const Element &a, const Element &b) const;
	virtual Element& Accumulate(Element &a, const Element &b) const;
	virtual Element& Reduce(Element &a, const Element &b) const;

	virtual Element ScalarMultiply(const Element &a, const Integer &e) const;
	virtual Element CascadeScalarMultiply(const Element &x, const Integer &e1, const Element &y, const Integer &e2) const;
};

template <class T> class AbstractRing : public AbstractGroup<T>
{
public:
	typedef T Element;

	AbstractRing() {m_mg.m_pRing = this;}
	AbstractRing(const AbstractRing &source) {}
	AbstractRing& operator=(const AbstractRing &source) {}

	virtual bool IsUnit(const Element &a) const =0;
	virtual const Element& One() const =0;
	virtual const Element& Multiply(const Element &a, const Element &b) const =0;
	virtual const Element& MultiplicativeInverse(const Element &a) const =0;

	virtual const Element& Square(const Element &a) const;
	virtual const Element& Divide(const Element &a, const Element &b) const;

	virtual Element Exponentiate(const Element &a, const Integer &e) const;
	virtual Element CascadeExponentiate(const Element &x, const Integer &e1, const Element &y, const Integer &e2) const;

	virtual const AbstractGroup<T>& MultiplicativeGroup() const
		{return m_mg;}

private:
	class MultiplicativeGroupT : public AbstractGroup<T>
	{
	public:
		const AbstractRing<T>& GetRing() const
			{return *m_pRing;}

		bool Equal(const Element &a, const Element &b) const
			{return GetRing().Equal(a, b);}

		const Element& Zero() const
			{return GetRing().One();}

		const Element& Add(const Element &a, const Element &b) const
			{return GetRing().Multiply(a, b);}

		Element& Accumulate(Element &a, const Element &b) const
			{return a = GetRing().Multiply(a, b);}

		const Element& Inverse(const Element &a) const
			{return GetRing().MultiplicativeInverse(a);}

		const Element& Subtract(const Element &a, const Element &b) const
			{return GetRing().Divide(a, b);}

		Element& Reduce(Element &a, const Element &b) const
			{return a = GetRing().Divide(a, b);}

		const Element& Double(const Element &a) const
			{return GetRing().Square(a);}

		const AbstractRing<T> *m_pRing;
	};

	MultiplicativeGroupT m_mg;
};

// ********************************************************

template <class T, class E = Integer>
struct BaseAndExponent
{
public:
	bool operator<(const BaseAndExponent<T> &rhs) const {return exponent < rhs.exponent;}
	T base;
	E exponent;
};

// VC60 workaround: incomplete member template support
template <class Element, class Iterator>
	Element GeneralCascadeMultiplication(const AbstractGroup<Element> &group, Iterator begin, Iterator end);
template <class Element, class Iterator, class ConstIterator>
	void SimultaneousMultiplication(Iterator result, const AbstractGroup<Element> &group, const Element &base, ConstIterator expBegin, ConstIterator expEnd);
template <class Element, class Iterator>
	Element GeneralCascadeExponentiation(const AbstractRing<Element> &ring, Iterator begin, Iterator end);
template <class Element, class Iterator, class ConstIterator>
	void SimultaneousExponentiation(Iterator result, const AbstractRing<Element> &ring, const Element &base, ConstIterator expBegin, ConstIterator expEnd);

// ********************************************************

template <class T> class AbstractEuclideanDomain : public AbstractRing<T>
{
public:
	typedef T Element;

	virtual void DivisionAlgorithm(Element &r, Element &q, const Element &a, const Element &d) const =0;

	virtual const Element& Mod(const Element &a, const Element &b) const =0;
	virtual const Element& Gcd(const Element &a, const Element &b) const;

protected:
	mutable Element result;
};

// ********************************************************

template <class T> class EuclideanDomainOf : public AbstractEuclideanDomain<T>
{
public:
	typedef T Element;

	EuclideanDomainOf() {}

	bool Equal(const Element &a, const Element &b) const
		{return a==b;}

	const Element& Zero() const
		{return Element::Zero();}

	const Element& Add(const Element &a, const Element &b) const
		{return result = a+b;}

	Element& Accumulate(Element &a, const Element &b) const
		{return a+=b;}

	const Element& Inverse(const Element &a) const
		{return result = -a;}

	const Element& Subtract(const Element &a, const Element &b) const
		{return result = a-b;}

	Element& Reduce(Element &a, const Element &b) const
		{return a-=b;}

	const Element& Double(const Element &a) const
		{return result = a.Doubled();}

	const Element& One() const
		{return Element::One();}

	const Element& Multiply(const Element &a, const Element &b) const
		{return result = a*b;}

	const Element& Square(const Element &a) const
		{return result = a.Squared();}

	bool IsUnit(const Element &a) const
		{return a.IsUnit();}

	const Element& MultiplicativeInverse(const Element &a) const
		{return result = a.MultiplicativeInverse();}

	const Element& Divide(const Element &a, const Element &b) const
		{return result = a/b;}

	const Element& Mod(const Element &a, const Element &b) const
		{return result = a%b;}

	void DivisionAlgorithm(Element &r, Element &q, const Element &a, const Element &d) const
		{Element::Divide(r, q, a, d);}

private:
	mutable Element result;
};

template <class T> class QuotientRing : public AbstractRing<typename T::Element>
{
public:
	typedef T EuclideanDomain;
	typedef typename T::Element Element;

	QuotientRing(const EuclideanDomain &domain, const Element &modulus)
		: m_domain(domain), m_modulus(modulus) {}

	const EuclideanDomain & GetDomain() const
		{return m_domain;}

	const Element& GetModulus() const
		{return m_modulus;}

	bool Equal(const Element &a, const Element &b) const
		{return m_domain.Equal(m_domain.Mod(m_domain.Subtract(a, b), m_modulus), m_domain.Zero());}

	const Element& Zero() const
		{return m_domain.Zero();}

	const Element& Add(const Element &a, const Element &b) const
		{return m_domain.Add(a, b);}

	Element& Accumulate(Element &a, const Element &b) const
		{return m_domain.Accumulate(a, b);}

	const Element& Inverse(const Element &a) const
		{return m_domain.Inverse(a);}

	const Element& Subtract(const Element &a, const Element &b) const
		{return m_domain.Subtract(a, b);}

	Element& Reduce(Element &a, const Element &b) const
		{return m_domain.Reduce(a, b);}

	const Element& Double(const Element &a) const
		{return m_domain.Double(a);}

	bool IsUnit(const Element &a) const
		{return m_domain.IsUnit(m_domain.Gcd(a, m_modulus));}

	const Element& One() const
		{return m_domain.One();}

	const Element& Multiply(const Element &a, const Element &b) const
		{return m_domain.Mod(m_domain.Multiply(a, b), m_modulus);}

	const Element& Square(const Element &a) const
		{return m_domain.Mod(m_domain.Square(a), m_modulus);}

	const Element& MultiplicativeInverse(const Element &a) const;

protected:
	EuclideanDomain m_domain;
	Element m_modulus;
};

NAMESPACE_END

#endif
