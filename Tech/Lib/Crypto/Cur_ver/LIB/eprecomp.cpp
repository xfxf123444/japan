#include "pch.h"
#include "eprecomp.h"

NAMESPACE_BEGIN(CryptoPP)

template <class T> void ExponentiationPrecomputation<T>::SetGroupAndBase(const Group &group, const Element &base)
{
	m_group = &group;
	m_bases.resize(1);
	m_bases[0] = base;
}

template <class T> void ExponentiationPrecomputation<T>::Precompute(unsigned int maxExpBits, unsigned int storage)
{
	assert(m_group != NULL);
	assert(m_bases.size() > 0);
	assert(storage <= maxExpBits);

	if (storage > 1)
		m_exponentBase = Integer::Power2((maxExpBits+storage-1)/storage);

	m_bases.resize(storage);
	for (unsigned i=1; i<storage; i++)
		m_bases[i] = m_group->ScalarMultiply(m_bases[i-1], m_exponentBase);
}

template <class T> ExponentiationPrecomputation<T>::Element ExponentiationPrecomputation<T>::Exponentiate(const Integer &exponent) const
{
	std::vector<BaseAndExponent<Element> > eb(m_bases.size());	// array of segments of the exponent and precalculated bases
	Integer temp, e = exponent;
	unsigned i;

	for (i=0; i+1<m_bases.size(); i++)
	{
		Integer::Divide(eb[i].exponent, temp, e, m_exponentBase);
		std::swap(temp, e);
		eb[i].base = m_bases[i];
	}
	eb[i].exponent = e;
	eb[i].base = m_bases[i];

	return GeneralCascadeMultiplication<Element>(*m_group, eb.begin(), eb.end());
}

template <class T> T 
	ExponentiationPrecomputation<T>::CascadeExponentiate(const Integer &exponent, 
		const ExponentiationPrecomputation<T> &pc2, const Integer &exponent2) const
{
	std::vector<BaseAndExponent<Element> > eb(m_bases.size()+pc2.m_bases.size());	// array of segments of the exponent and precalculated bases
	Integer temp, e = exponent;
	unsigned i;

	for (i=0; i+1<m_bases.size(); i++)
	{
		Integer::Divide(eb[i].exponent, temp, e, m_exponentBase);
		std::swap(temp, e);
		eb[i].base = m_bases[i];
	}
	eb[i].exponent = e;
	eb[i].base = m_bases[i];

	e = exponent2;
	for (i=m_bases.size(); i+1<m_bases.size()+pc2.m_bases.size(); i++)
	{
		Integer::Divide(eb[i].exponent, temp, e, pc2.m_exponentBase);
		std::swap(temp, e);
		eb[i].base = pc2.m_bases[i-m_bases.size()];
	}
	eb[i].exponent = e;
	eb[i].base = pc2.m_bases[i-m_bases.size()];

	return GeneralCascadeMultiplication<Element>(*m_group, eb.begin(), eb.end());
}

NAMESPACE_END
