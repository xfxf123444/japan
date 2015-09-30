/* RSA.C - RSA routines for RSAREF
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */

#include "misc.h"
#include "nn.h"
#include "rsa.h"
#include "md5.h"
#include "..\Export\Crypto.h"

ULONG SMALL_PRIMES[] = { 3, 5, 7, 11 };
#define SMALL_PRIME_COUNT 4


/* RSA public-key encryption, according to PKCS #1.
 */
long YGRSAPublicEncrypt(UCHAR *output,                                      /* output block */
					 ULONG *outputLen,                          /* length of output block */
					 UCHAR *input,                                        /* input block */
					 ULONG inputLen,                             /* length of input block */
					 R_RSA_PUBLIC_KEY *publicKey)                              /* RSA public key */
{
    long status;
    UCHAR byte, pkcsBlock[MAX_RSA_MODULUS_LEN];
    ULONG i, modulusLen,ulCounts,ulBlockLen,OutputLen;
    R_RANDOM_STRUCT randomStruct;

	*outputLen = 0;
    modulusLen = (publicKey->bits + 7) / 8;
	ulBlockLen = modulusLen - 11;
	while (inputLen)
	{
		if (ulBlockLen > inputLen)
			ulCounts = inputLen;
		else ulCounts = ulBlockLen;
		YGECMemset((UCHAR *)&randomStruct,0,sizeof(R_RANDOM_STRUCT));

		pkcsBlock[0] = 0;
		/* block type 2 */
		pkcsBlock[1] = 2;

		for (i = 2; i < modulusLen - ulCounts - 1; i++) 
		{
			/* Find nonzero random byte.
			 */
			do {
			  R_GenerateBytes (&byte, 1,&randomStruct);
			} while (byte == 0);
			pkcsBlock[i] = byte;
		}
		/* separator */
		pkcsBlock[i++] = 0;
  
		YGECMemcpy ((UCHAR *)&pkcsBlock[i], (UCHAR *)input, ulCounts);
  
		status = RSAPublicBlock(output, &OutputLen, pkcsBlock, modulusLen, publicKey);
  
		/* Zeroize sensitive information.
		*/
		byte = 0;
		YGECMemset ((UCHAR *)pkcsBlock, 0, sizeof (pkcsBlock));
		inputLen -= ulCounts;
		output += modulusLen;
		input += ulCounts;
		*outputLen += OutputLen;
	}  
    return (status);
}

/* RSA public-key decryption, according to PKCS #1.
 */
long YGRSAPublicDecrypt(UCHAR *output,                                      /* output block */
					 ULONG *outputLen,                          /* length of output block */
					 UCHAR *input,                                        /* input block */
					 ULONG inputLen,                             /* length of input block */
					 R_RSA_PUBLIC_KEY *publicKey)                              /* RSA public key */
{
  long status;
  UCHAR pkcsBlock[MAX_RSA_MODULUS_LEN];
  ULONG i, modulusLen, pkcsBlockLen,OutputLen;
  
  modulusLen = (publicKey->bits + 7) / 8;
  if (inputLen % modulusLen) return RE_LEN;
  *outputLen = 0;
  while (inputLen)
  {
	  if (status = RSAPublicBlock(pkcsBlock, &pkcsBlockLen, input,modulusLen, publicKey))
		return (status);
  
	  if (pkcsBlockLen != modulusLen)
		return (RE_LEN);
  
	  /* Require block type 1.
	   */
	  if ((pkcsBlock[0] != 0) || (pkcsBlock[1] != 1))
	   return (RE_DATA);

	  for (i = 2; i < modulusLen-1; i++)
		if (pkcsBlock[i] != 0xff)
		  break;
    
	  /* separator */
	  if (pkcsBlock[i++] != 0)
		return (RE_DATA);
  
	  OutputLen = modulusLen - i;
  
	  if (OutputLen + 11 > modulusLen)
		return (RE_DATA);

	  YGECMemcpy ((UCHAR *)output, (UCHAR *)&pkcsBlock[i], OutputLen);
  
	  /* Zeroize potentially sensitive information.
	   */
	  YGECMemset ((UCHAR *)pkcsBlock, 0, sizeof (pkcsBlock));
	  *outputLen += OutputLen;
	  output += OutputLen;
	  input +=  modulusLen;
	  inputLen -= modulusLen;
  }  
  return (0);
}

/* RSA private-key encryption, according to PKCS #1.
 */
long YGRSAPrivateEncrypt (UCHAR *output,                                      /* output block */
						ULONG *outputLen,                          /* length of output block */
						UCHAR *input,                                        /* input block */
						ULONG inputLen,                             /* length of input block */
						R_RSA_PRIVATE_KEY *privateKey)                           /* RSA private key */
{
  long status;
  UCHAR pkcsBlock[MAX_RSA_MODULUS_LEN];
  ULONG i, modulusLen,ulBlockLen,OutputLen,ulCounts;
  
  modulusLen = (privateKey->bits + 7) / 8;
  if (inputLen % modulusLen) return RE_LEN;
  ulBlockLen = modulusLen - 11; 

  while (inputLen)
  {
	  if (ulBlockLen > inputLen)
		  ulCounts = inputLen;
	  else ulCounts = ulBlockLen;
	  pkcsBlock[0] = 0;
	  /* block type 1 */
	  pkcsBlock[1] = 1;

	  for (i = 2; i < modulusLen - ulCounts - 1; i++)
		pkcsBlock[i] = 0xff;

	  /* separator */
	  pkcsBlock[i++] = 0;
  
	  YGECMemcpy ((UCHAR *)&pkcsBlock[i], (UCHAR *)input, ulCounts);
  
	  status = RSAPrivateBlock(output, &OutputLen, pkcsBlock, modulusLen, privateKey);

	  /* Zeroize potentially sensitive information.
	   */
	  YGECMemset ((UCHAR *)pkcsBlock, 0, sizeof (pkcsBlock));
	  inputLen -= ulCounts;
	  output += modulusLen;
	  input += ulCounts;
	  *outputLen += OutputLen;
  }
  return (status);
}

/* RSA private-key decryption, according to PKCS #1.
 */
long YGRSAPrivateDecrypt (UCHAR *output,                                      /* output block */
						ULONG *outputLen,                          /* length of output block */
						UCHAR *input,                                        /* input block */
						ULONG inputLen,                             /* length of input block */
						R_RSA_PRIVATE_KEY *privateKey)                           /* RSA private key */
{
  long status;
  UCHAR pkcsBlock[MAX_RSA_MODULUS_LEN];
  ULONG i, modulusLen, pkcsBlockLen,OutputLen;

  modulusLen = (privateKey->bits + 7) / 8;
  *outputLen = 0;
  while (inputLen)
  {
	  if (status = RSAPrivateBlock(pkcsBlock, &pkcsBlockLen, input, modulusLen, privateKey))
		return (status);
  
	  if (pkcsBlockLen != modulusLen)
		return (RE_LEN);
  
	  /* Require block type 2.
	   */
	  if ((pkcsBlock[0] != 0) || (pkcsBlock[1] != 2))
	   return (RE_DATA);

	  for (i = 2; i < modulusLen-1; i++)
		/* separator */
		if (pkcsBlock[i] == 0)
		  break;
    
	  i++;
	  if (i >= modulusLen)
		return (RE_DATA);
    
	  OutputLen = modulusLen - i;
  
	  if (OutputLen + 11 > modulusLen)
		return (RE_DATA);

	  YGECMemcpy ((UCHAR *)output, (UCHAR *)&pkcsBlock[i], OutputLen);
  
	  /* Zeroize sensitive information.
	   */
	  YGECMemset ((UCHAR *)pkcsBlock, 0, sizeof (pkcsBlock));
	  *outputLen += OutputLen;
	  output += OutputLen;
	  input +=  modulusLen;
	  inputLen -= modulusLen;
  }  
  return (0);
}

/* Raw RSA public-key operation. Output has same length as modulus.

   Assumes inputLen < length of modulus.
   Requires input < modulus.
 */
long RSAPublicBlock(UCHAR *output,                                      /* output block */
						  ULONG *outputLen,                          /* length of output block */
						  UCHAR *input,                                        /* input block */
						  ULONG inputLen,                             /* length of input block */
						  R_RSA_PUBLIC_KEY *publicKey)                              /* RSA public key */
{
  ULONG c[MAX_NN_DIGITS], e[MAX_NN_DIGITS], m[MAX_NN_DIGITS],
	    n[MAX_NN_DIGITS];
  ULONG eDigits, nDigits;

  NN_Decode (m, MAX_NN_DIGITS, input, inputLen);
  NN_Decode (n, MAX_NN_DIGITS, publicKey->modulus, MAX_RSA_MODULUS_LEN);
  NN_Decode (e, MAX_NN_DIGITS, publicKey->exponent, MAX_RSA_MODULUS_LEN);
  nDigits = NN_Digits (n, MAX_NN_DIGITS);
  eDigits = NN_Digits (e, MAX_NN_DIGITS);
  
  if (NN_Cmp (m, n, nDigits) >= 0)   return (RE_DATA);
  
  /* Compute c = m^e mod n.
   */
  NN_ModExp (c, m, e, eDigits, n, nDigits);

  *outputLen = (publicKey->bits + 7) / 8;
  NN_Encode (output, *outputLen, c, nDigits);
  
  /* Zeroize sensitive information.
   */
  YGECMemset ((UCHAR *)c, 0, sizeof (c));
  YGECMemset ((UCHAR *)m, 0, sizeof (m));

  return (0);
}

/* Raw RSA private-key operation. Output has same length as modulus.

   Assumes inputLen < length of modulus.
   Requires input < modulus.
 */
long RSAPrivateBlock( UCHAR *output,                                      /* output block */
							ULONG *outputLen,                          /* length of output block */
							UCHAR *input,                                        /* input block */
							ULONG inputLen,                             /* length of input block */
							R_RSA_PRIVATE_KEY *privateKey)                           /* RSA private key */
{
  ULONG c[MAX_NN_DIGITS], cP[MAX_NN_DIGITS], cQ[MAX_NN_DIGITS],
    dP[MAX_NN_DIGITS], dQ[MAX_NN_DIGITS], mP[MAX_NN_DIGITS],
    mQ[MAX_NN_DIGITS], n[MAX_NN_DIGITS], p[MAX_NN_DIGITS], q[MAX_NN_DIGITS],
    qInv[MAX_NN_DIGITS], t[MAX_NN_DIGITS];
  ULONG cDigits, nDigits, pDigits;
  
  NN_Decode (c, MAX_NN_DIGITS, input, inputLen);
  NN_Decode (n, MAX_NN_DIGITS, privateKey->modulus, MAX_RSA_MODULUS_LEN);
  NN_Decode (p, MAX_NN_DIGITS, privateKey->prime[0], MAX_RSA_PRIME_LEN);
  NN_Decode (q, MAX_NN_DIGITS, privateKey->prime[1], MAX_RSA_PRIME_LEN);
  NN_Decode 
    (dP, MAX_NN_DIGITS, privateKey->primeExponent[0], MAX_RSA_PRIME_LEN);
  NN_Decode 
    (dQ, MAX_NN_DIGITS, privateKey->primeExponent[1], MAX_RSA_PRIME_LEN);
  NN_Decode (qInv, MAX_NN_DIGITS, privateKey->coefficient, MAX_RSA_PRIME_LEN);
  cDigits = NN_Digits (c, MAX_NN_DIGITS);
  nDigits = NN_Digits (n, MAX_NN_DIGITS);
  pDigits = NN_Digits (p, MAX_NN_DIGITS);

  if (NN_Cmp (c, n, nDigits) >= 0)
    return (RE_DATA);
  
  /* Compute mP = cP^dP mod p  and  mQ = cQ^dQ mod q. (Assumes q has
     length at most pDigits, i.e., p > q.)
   */
  NN_Mod (cP, c, cDigits, p, pDigits);
  NN_Mod (cQ, c, cDigits, q, pDigits);
  NN_ModExp (mP, cP, dP, pDigits, p, pDigits);
  NN_AssignZero (mQ, nDigits);
  NN_ModExp (mQ, cQ, dQ, pDigits, q, pDigits);
  
  /* Chinese Remainder Theorem:
       m = ((((mP - mQ) mod p) * qInv) mod p) * q + mQ.
   */
  if (NN_Cmp (mP, mQ, pDigits) >= 0)
    NN_Sub (t, mP, mQ, pDigits);
  else {
    NN_Sub (t, mQ, mP, pDigits);
    NN_Sub (t, p, t, pDigits);
  }
  NN_ModMult (t, t, qInv, p, pDigits);
  NN_Mult (t, t, q, pDigits);
  NN_Add (t, t, mQ, nDigits);

  *outputLen = (privateKey->bits + 7) / 8;
  NN_Encode (output, *outputLen, t, nDigits);

  /* Zeroize sensitive information.
   */
  YGECMemset ((UCHAR *)c, 0, sizeof (c));
  YGECMemset ((UCHAR *)cP, 0, sizeof (cP));
  YGECMemset ((UCHAR *)cQ, 0, sizeof (cQ));
  YGECMemset ((UCHAR *)dP, 0, sizeof (dP));
  YGECMemset ((UCHAR *)dQ, 0, sizeof (dQ));
  YGECMemset ((UCHAR *)mP, 0, sizeof (mP));
  YGECMemset ((UCHAR *)mQ, 0, sizeof (mQ));
  YGECMemset ((UCHAR *)p, 0, sizeof (p));
  YGECMemset ((UCHAR *)q, 0, sizeof (q));
  YGECMemset ((UCHAR *)qInv, 0, sizeof (qInv));
  YGECMemset ((UCHAR *)t, 0, sizeof (t));

  return (0);
}

ULONG YGRSAGetOrgiBuffLen(ULONG ulKeyLen,ULONG ulEcyLen)
{
	ULONG ulModulusLen = (ulKeyLen + 7) / 8;
	ulEcyLen /= ulModulusLen;
	ulEcyLen *= (ulModulusLen-11);
	return ulEcyLen;
}

ULONG YGRSAGetEcyBuffLen(ULONG ulKeyLen,ULONG ulOrgiLen)
{
	ULONG ulModulusLen = (ulKeyLen + 7) / 8;
	ulOrgiLen = (ulOrgiLen + (ulModulusLen-10))/(ulModulusLen-11);
	ulOrgiLen *= ulModulusLen;
	return ulOrgiLen;
}

/* Generates an RSA key pair with a given length and public exponent.
 */
long YGRSAGeneratePEMKeys(R_RSA_PUBLIC_KEY *publicKey,                          /* new RSA public key */
					  R_RSA_PRIVATE_KEY *privateKey,                       /* new RSA private key */
					  ULONG ulKeyLen)                             /* RSA prototype key */
					                            /* random structure */
{
  ULONG d[MAX_NN_DIGITS], dP[MAX_NN_DIGITS], dQ[MAX_NN_DIGITS],
		e[MAX_NN_DIGITS], n[MAX_NN_DIGITS], p[MAX_NN_DIGITS], phiN[MAX_NN_DIGITS],
		pMinus1[MAX_NN_DIGITS], q[MAX_NN_DIGITS], qInv[MAX_NN_DIGITS],
		qMinus1[MAX_NN_DIGITS], t[MAX_NN_DIGITS], u[MAX_NN_DIGITS],
		v[MAX_NN_DIGITS];
  long status;
  ULONG nDigits, pBits, pDigits, qBits;
  R_RANDOM_STRUCT randomStruct;
  YGECMemset((UCHAR *)&randomStruct,0,sizeof(R_RANDOM_STRUCT));
  if ((ulKeyLen < MIN_RSA_MODULUS_BITS) || 
      (ulKeyLen > MAX_RSA_MODULUS_BITS))
    return (RE_MODULUS_LEN);
  nDigits = (ulKeyLen + NN_DIGIT_BITS - 1) / NN_DIGIT_BITS;
  pDigits = (nDigits + 1) / 2;
  pBits = (ulKeyLen + 1) / 2;
  qBits = ulKeyLen - pBits;

  /* NOTE: for 65537, this assumes ULONG is at least 17 bits. */
  NN_ASSIGN_DIGIT
    (e, (ULONG)3, nDigits);

  /* Generate prime p between 3*2^(pBits-2) and 2^pBits-1, searching
       in steps of 2, until one satisfies gcd (p-1, e) = 1.
   */
  NN_Assign2Exp (t, pBits-1, pDigits);
  NN_Assign2Exp (u, pBits-2, pDigits);
  NN_Add (t, t, u, pDigits);
  NN_ASSIGN_DIGIT (v, 1, pDigits);
  NN_Sub (v, t, v, pDigits);
  NN_Add (u, u, v, pDigits);
  NN_ASSIGN_DIGIT (v, 2, pDigits);
  do {
    if (status = GeneratePrime (p, t, u, v, pDigits,&randomStruct))
      return (status);
  }
  while (! RSAFilter (p, pDigits, e, 1));
  
  /* Generate prime q between 3*2^(qBits-2) and 2^qBits-1, searching
       in steps of 2, until one satisfies gcd (q-1, e) = 1.
   */
  NN_Assign2Exp (t, qBits-1, pDigits);
  NN_Assign2Exp (u, qBits-2, pDigits);
  NN_Add (t, t, u, pDigits);
  NN_ASSIGN_DIGIT (v, 1, pDigits);
  NN_Sub (v, t, v, pDigits);
  NN_Add (u, u, v, pDigits);
  NN_ASSIGN_DIGIT (v, 2, pDigits);
  do {
    if (status = GeneratePrime (q, t, u, v, pDigits,&randomStruct))
      return (status);
  }
  while (! RSAFilter (q, pDigits, e, 1));
  
  /* Sort so that p > q. (p = q case is extremely unlikely.)
   */
  if (NN_Cmp (p, q, pDigits) < 0) {
    NN_Assign (t, p, pDigits);
    NN_Assign (p, q, pDigits);
    NN_Assign (q, t, pDigits);
  }

  /* Compute n = pq, qInv = q^{-1} mod p, d = e^{-1} mod (p-1)(q-1),
     dP = d mod p-1, dQ = d mod q-1.
   */
  NN_Mult (n, p, q, pDigits);
  NN_ModInv (qInv, q, p, pDigits);
  
  NN_ASSIGN_DIGIT (t, 1, pDigits);
  NN_Sub (pMinus1, p, t, pDigits);
  NN_Sub (qMinus1, q, t, pDigits);
  NN_Mult (phiN, pMinus1, qMinus1, pDigits);

  NN_ModInv (d, e, phiN, nDigits);
  NN_Mod (dP, d, nDigits, pMinus1, pDigits);
  NN_Mod (dQ, d, nDigits, qMinus1, pDigits);
  
  publicKey->bits = privateKey->bits = ulKeyLen;
  NN_Encode (publicKey->modulus, MAX_RSA_MODULUS_LEN, n, nDigits);
  NN_Encode (publicKey->exponent, MAX_RSA_MODULUS_LEN, e, 1);
  YGECMemcpy 
    ((UCHAR *)privateKey->modulus, (UCHAR *)publicKey->modulus,
     MAX_RSA_MODULUS_LEN);
  YGECMemcpy
    ((UCHAR *)privateKey->publicExponent, (UCHAR *)publicKey->exponent,
     MAX_RSA_MODULUS_LEN);
  NN_Encode (privateKey->exponent, MAX_RSA_MODULUS_LEN, d, nDigits);
  NN_Encode (privateKey->prime[0], MAX_RSA_PRIME_LEN, p, pDigits);
  NN_Encode (privateKey->prime[1], MAX_RSA_PRIME_LEN, q, pDigits);
  NN_Encode (privateKey->primeExponent[0], MAX_RSA_PRIME_LEN, dP, pDigits);
  NN_Encode (privateKey->primeExponent[1], MAX_RSA_PRIME_LEN, dQ, pDigits);
  NN_Encode (privateKey->coefficient, MAX_RSA_PRIME_LEN, qInv, pDigits);
   
  /* Zeroize sensitive information.
   */
  YGECMemset ((UCHAR *)d, 0, sizeof (d));
  YGECMemset ((UCHAR *)dP, 0, sizeof (dP));
  YGECMemset ((UCHAR *)dQ, 0, sizeof (dQ));
  YGECMemset ((UCHAR *)p, 0, sizeof (p));
  YGECMemset ((UCHAR *)phiN, 0, sizeof (phiN));
  YGECMemset ((UCHAR *)pMinus1, 0, sizeof (pMinus1));
  YGECMemset ((UCHAR *)q, 0, sizeof (q));
  YGECMemset ((UCHAR *)qInv, 0, sizeof (qInv));
  YGECMemset ((UCHAR *)qMinus1, 0, sizeof (qMinus1));
  YGECMemset ((UCHAR *)t, 0, sizeof (t));
  
  return (0);
}

/* Returns nonzero iff GCD (a-1, b) = 1.

   Lengths: a[aDigits], b[bDigits].
   Assumes aDigits < MAX_NN_DIGITS, bDigits < MAX_NN_DIGITS.
 */
long RSAFilter(ULONG *a,ULONG aDigits,ULONG *b, ULONG bDigits)
{
  long status;
  ULONG aMinus1[MAX_NN_DIGITS], t[MAX_NN_DIGITS];
  
  NN_ASSIGN_DIGIT (t, 1, aDigits);
  NN_Sub (aMinus1, a, t, aDigits);
  
  status = RelativelyPrime (aMinus1, aDigits, b, bDigits);

  /* Zeroize sensitive information.
   */
  YGECMemset ((UCHAR *)aMinus1, 0, sizeof (aMinus1));
  
  return (status);
}

/* Returns nonzero iff a and b are relatively prime.

   Lengths: a[aDigits], b[bDigits].
   Assumes aDigits >= bDigits, aDigits < MAX_NN_DIGITS.
 */
long RelativelyPrime (ULONG *a,ULONG aDigits, ULONG *b,ULONG bDigits)
{
  long status;
  ULONG t[MAX_NN_DIGITS], u[MAX_NN_DIGITS];
  
  NN_AssignZero (t, aDigits);
  NN_Assign (t, b, bDigits);
  NN_Gcd (t, a, t, aDigits);
  NN_ASSIGN_DIGIT (u, 1, aDigits);

  status = NN_EQUAL (t, u, aDigits);
  
  /* Zeroize sensitive information.
   */
  YGECMemset ((UCHAR *)t, 0, sizeof (t));
  
  return (status);
}
/* Generates a probable prime a between b and c such that a-1 is
   divisible by d.

   Lengths: a[digits], b[digits], c[digits], d[digits].
   Assumes b < c, digits < MAX_NN_DIGITS.
   
   Returns RE_NEED_RANDOM if randomStruct not seeded, RE_DATA if
   unsuccessful.
 */
long GeneratePrime(ULONG *a,ULONG *b,ULONG *c,ULONG *d,ULONG digits,R_RANDOM_STRUCT *randomStruct)
{
  long status;
  UCHAR block[MAX_NN_DIGITS * NN_DIGIT_LEN];
  ULONG t[MAX_NN_DIGITS], u[MAX_NN_DIGITS];

  /* Generate random number between b and c.
   */
  if (status = R_GenerateBytes (block, digits * NN_DIGIT_LEN,randomStruct))
    return (status);
  NN_Decode (a, digits, block, digits * NN_DIGIT_LEN);
  NN_Sub (t, c, b, digits);
  NN_ASSIGN_DIGIT (u, 1, digits);
  NN_Add (t, t, u, digits);
  NN_Mod (a, a, digits, t, digits);
  NN_Add (a, a, b, digits);

  /* Adjust so that a-1 is divisible by d.
   */
  NN_Mod (t, a, digits, d, digits);
  NN_Sub (a, a, t, digits);
  NN_Add (a, a, u, digits);
  if (NN_Cmp (a, b, digits) < 0)
    NN_Add (a, a, d, digits);
  if (NN_Cmp (a, c, digits) > 0)
    NN_Sub (a, a, d, digits);

  /* Search to c in steps of d.
   */
  NN_Assign (t, c, digits);
  NN_Sub (t, t, d, digits);

  while (! ProbablePrime (a, digits)) {
    if (NN_Cmp (a, t, digits) > 0)
      return (RE_DATA);
    NN_Add (a, a, d, digits);
  }

  return (0);
}

/* Returns nonzero iff a is a probable prime.

   Lengths: a[aDigits].
   Assumes aDigits < MAX_NN_DIGITS.
 */
long ProbablePrime(ULONG *a,ULONG aDigits)
{
  return (! SmallFactor (a, aDigits) && FermatTest (a, aDigits));
}

/* Returns nonzero iff a has a prime factor in SMALL_PRIMES.

   Lengths: a[aDigits].
   Assumes aDigits < MAX_NN_DIGITS.
 */
long SmallFactor(ULONG *a,ULONG aDigits)
{
  long status;
  ULONG t[1];
  ULONG i;
  
  status = 0;
  
  for (i = 0; i < SMALL_PRIME_COUNT; i++) {
    NN_ASSIGN_DIGIT (t, SMALL_PRIMES[i], 1);
    if ((aDigits == 1) && ! NN_Cmp (a, t, 1))
      break;
    NN_Mod (t, a, aDigits, t, 1);
    if (NN_Zero (t, 1)) {
      status = 1;
      break;
    }
  }
  
  /* Zeroize sensitive information.
   */
  i = 0;
  YGECMemset ((UCHAR *)t, 0, sizeof (t));

  return (status);
}

/* Returns nonzero iff a passes Fermat's test for witness 2.
   (All primes pass the test, and nearly all composites fail.)
     
   Lengths: a[aDigits].
   Assumes aDigits < MAX_NN_DIGITS.
 */
long FermatTest(ULONG *a,ULONG aDigits)
{
  long status;
  ULONG t[MAX_NN_DIGITS], u[MAX_NN_DIGITS];
  
  NN_ASSIGN_DIGIT (t, 2, aDigits);
  NN_ModExp (u, t, a, aDigits, a, aDigits);
  
  status = NN_EQUAL (t, u, aDigits);
  
  /* Zeroize sensitive information.
   */
  YGECMemset ((UCHAR *)u, 0, sizeof (u));
  
  return (status);
}

long R_GenerateBytes(UCHAR *block,ULONG blockLen,  R_RANDOM_STRUCT *randomStruct)
{
  MD5_CTX context;
  ULONG available, i;

  available =  randomStruct->outputAvailable ;
  
  while (blockLen > available) 
  {
    YGECMemcpy((UCHAR *)block, (UCHAR *)&randomStruct->output[16-available],available);
    block += available;
    blockLen -= available;

    /* generate new output */
    MD5Init (&context);
    MD5Update (&context, randomStruct->state, 16);
    MD5Final (randomStruct->output, &context);
    available = 16;

    /* increment state */
    for (i = 0; i < 16; i++)
      if (randomStruct->state[15-i]++)
        break;
  }

  YGECMemcpy((UCHAR *)block, (UCHAR *)&randomStruct->output[16-available], blockLen);
  randomStruct->outputAvailable = available - blockLen;

  return (0);
}

