/* NN.C - natural numbers routines
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */

/* CHANGES MADE TO THIS FILE UNDER RSAREF 2.0 license clause 1(c):

   For the MIT PGP 2.5 distribution, this file was modified to permit
   replacement of the NN_ModExp routine by an equivalent routine contained 
   in the PGP 2.5 sources.  To enable this change, an #ifdef was added to this 
   file (search for #ifndef USEMPILIB below).  RSAREF *must* be compiled with 
   USEMPILIB defined for this change to occur.

   Change made May 8, 1994.  */

#include "misc.h"
#include "nn.h"
#include "..\Export\Crypto.h"

/* Decodes character string b into a, where character string is ordered
   from most to least significant.

   Lengths: a[digits], b[len].
   Assumes b[i] = 0 for i < len - digits * NN_DIGIT_LEN. (Otherwise most
   significant bytes are truncated.)
 */
void NN_Decode(ULONG *a,ULONG digits,UCHAR *b,ULONG len)
{
  ULONG t;
  long j;
  ULONG i, u;
  
  for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
    t = 0;
    for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
      t |= ((ULONG)b[j]) << u;
    a[i] = t;
  }
  
  for (; i < digits; i++)
    a[i] = 0;
}

/* Encodes b into character string a, where character string is ordered
   from most to least significant.

   Lengths: a[len], b[digits].
   Assumes NN_Bits (b, digits) <= 8 * len. (Otherwise most significant
   digits are truncated.)
 */
void NN_Encode (UCHAR * a,ULONG len,ULONG *b,ULONG digits)
{
  ULONG t;
  long j;
  ULONG i, u;

  for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
    t = b[i];
    for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
      a[j] = (UCHAR)(t >> u);
  }

  for (; j >= 0; j--)
    a[j] = 0;
}

/* Assigns a = b.

   Lengths: a[digits], b[digits].
 */
void NN_Assign (ULONG *a,ULONG *b,ULONG digits)
{
  ULONG i;

  for (i = 0; i < digits; i++)
    a[i] = b[i];
}

/* Assigns a = 0.

   Lengths: a[digits].
 */
void NN_AssignZero(ULONG *a,ULONG digits)
{
  ULONG i;

  for (i = 0; i < digits; i++)
    a[i] = 0;
}

/* Assigns a = 2^b.

   Lengths: a[digits].
   Requires b < digits * NN_DIGIT_BITS.
 */
void NN_Assign2Exp(ULONG *a,ULONG b,ULONG digits)
{
  NN_AssignZero (a, digits);

  if (b >= digits * NN_DIGIT_BITS)
    return;

  a[b / NN_DIGIT_BITS] = (ULONG)1 << (b % NN_DIGIT_BITS);
}

/* Computes a = b + c. Returns carry.

   Lengths: a[digits], b[digits], c[digits].
 */
ULONG NN_Add(ULONG *a,ULONG *b,ULONG *c,ULONG digits)
{
  ULONG ai, carry;
  ULONG i;

  carry = 0;

  for (i = 0; i < digits; i++) {
    if ((ai = b[i] + carry) < carry)
      ai = c[i];
    else if ((ai += c[i]) < c[i])
      carry = 1;
    else
      carry = 0;
    a[i] = ai;
  }

  return (carry);
}

/* Computes a = b - c. Returns borrow.

   Lengths: a[digits], b[digits], c[digits].
 */
ULONG NN_Sub(ULONG *a,ULONG *b,ULONG *c,ULONG digits)
{
  ULONG ai, borrow;
  ULONG i;

  borrow = 0;

  for (i = 0; i < digits; i++) {
    if ((ai = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
      ai = MAX_NN_DIGIT - c[i];
    else if ((ai -= c[i]) > (MAX_NN_DIGIT - c[i]))
      borrow = 1;
    else
      borrow = 0;
    a[i] = ai;
  }

  return (borrow);
}

/* Computes a = b * c.

   Lengths: a[2*digits], b[digits], c[digits].
   Assumes digits < MAX_NN_DIGITS.
 */
void NN_Mult(ULONG *a,ULONG *b,ULONG *c,ULONG digits)
{
  ULONG t[2*MAX_NN_DIGITS];
  ULONG bDigits, cDigits, i;

  NN_AssignZero (t, 2 * digits);
  
  bDigits = NN_Digits (b, digits);
  cDigits = NN_Digits (c, digits);

  for (i = 0; i < bDigits; i++)
    t[i+cDigits] += NN_AddDigitMult (&t[i], &t[i], b[i], c, cDigits);
  
  NN_Assign (a, t, 2 * digits);
  
  /* Zeroize potentially sensitive information.
   */
  YGECMemset ((UCHAR *)t, 0, sizeof (t));
}

/* Computes a = b * 2^c (i.e., shifts left c bits), returning carry.

   Lengths: a[digits], b[digits].
   Requires c < NN_DIGIT_BITS.
 */
ULONG NN_LShift(ULONG *a,ULONG *b,ULONG c,ULONG digits)
{
  ULONG bi, carry;
  ULONG i, t;
  
  if (c >= NN_DIGIT_BITS)
    return (0);
  
  t = NN_DIGIT_BITS - c;

  carry = 0;

  for (i = 0; i < digits; i++) {
    bi = b[i];
    a[i] = (bi << c) | carry;
    carry = c ? (bi >> t) : 0;
  }
  
  return (carry);
}

/* Computes a = c div 2^c (i.e., shifts right c bits), returning carry.

   Lengths: a[digits], b[digits].
   Requires: c < NN_DIGIT_BITS.
 */
ULONG NN_RShift(ULONG *a,ULONG *b,ULONG c,ULONG digits)
{
  ULONG bi, carry;
  long i;
  ULONG t;
  
  if (c >= NN_DIGIT_BITS)
    return (0);
  
  t = NN_DIGIT_BITS - c;

  carry = 0;

  for (i = digits - 1; i >= 0; i--) {
    bi = b[i];
    a[i] = (bi >> c) | carry;
    carry = c ? (bi << t) : 0;
  }
  
  return (carry);
}

/* Computes a = c div d and b = c mod d.

   Lengths: a[cDigits], b[dDigits], c[cDigits], d[dDigits].
   Assumes d > 0, cDigits < 2 * MAX_NN_DIGITS,
           dDigits < MAX_NN_DIGITS.
 */
void NN_Div (ULONG *a,ULONG *b,ULONG *c,ULONG cDigits,ULONG *d,ULONG dDigits)
{
  ULONG ai, cc[2*MAX_NN_DIGITS+1], dd[MAX_NN_DIGITS], t;
  long i;
  ULONG ddDigits, shift;
  
  ddDigits = NN_Digits (d, dDigits);
  if (ddDigits == 0)
    return;
  
  /* Normalize operands.
   */
  shift = NN_DIGIT_BITS - NN_DigitBits (d[ddDigits-1]);
  NN_AssignZero (cc, ddDigits);
  cc[cDigits] = NN_LShift (cc, c, shift, cDigits);
  NN_LShift (dd, d, shift, ddDigits);
  t = dd[ddDigits-1];
  
  NN_AssignZero (a, cDigits);

  for (i = cDigits-ddDigits; i >= 0; i--) {
    /* Underestimate quotient digit and subtract.
     */
    if (t == MAX_NN_DIGIT)
      ai = cc[i+ddDigits];
    else
      NN_DigitDiv (&ai, &cc[i+ddDigits-1], t + 1);
    cc[i+ddDigits] -= NN_SubDigitMult (&cc[i], &cc[i], ai, dd, ddDigits);

    /* Correct estimate.
     */
    while (cc[i+ddDigits] || (NN_Cmp (&cc[i], dd, ddDigits) >= 0)) {
      ai++;
      cc[i+ddDigits] -= NN_Sub (&cc[i], &cc[i], dd, ddDigits);
    }
    
    a[i] = ai;
  }
  
  /* Restore result.
   */
  NN_AssignZero (b, dDigits);
  NN_RShift (b, cc, shift, ddDigits);

  /* Zeroize potentially sensitive information.
   */
  YGECMemset ((UCHAR *)cc, 0, sizeof (cc));
  YGECMemset ((UCHAR *)dd, 0, sizeof (dd));
}

/* Computes a = b mod c.

   Lengths: a[cDigits], b[bDigits], c[cDigits].
   Assumes c > 0, bDigits < 2 * MAX_NN_DIGITS, cDigits < MAX_NN_DIGITS.
 */
void NN_Mod(ULONG *a,ULONG *b,ULONG bDigits,ULONG *c,ULONG cDigits)
{
  ULONG t[2 * MAX_NN_DIGITS];
  
  NN_Div (t, a, b, bDigits, c, cDigits);
  
  /* Zeroize potentially sensitive information.
   */
  YGECMemset ((UCHAR *)t, 0, sizeof (t));
}

/* Computes a = b * c mod d.

   Lengths: a[digits], b[digits], c[digits], d[digits].
   Assumes d > 0, digits < MAX_NN_DIGITS.
 */
void NN_ModMult (ULONG *a,ULONG *b,ULONG *c,ULONG *d,ULONG digits)
{
  ULONG t[2*MAX_NN_DIGITS];

  NN_Mult (t, b, c, digits);
  NN_Mod (a, t, 2 * digits, d, digits);
  
  /* Zeroize potentially sensitive information.
   */
  YGECMemset ((UCHAR *)t, 0, sizeof (t));
}

/* Computes a = b^c mod d.

   Lengths: a[dDigits], b[dDigits], c[cDigits], d[dDigits].
   Assumes d > 0, cDigits > 0, dDigits < MAX_NN_DIGITS.
 */

/* PGP 2.5's mpilib contains a faster modular exponentiation routine, mp_modexp.
   If USEMPILIB is defined, NN_ModExp is replaced in the PGP 2.5 sources with a 
   stub call to mp_modexp.  If USEMPILIB is not defined, we'll get a pure (albeit
   slower) RSAREF implementation.

   The RSAREF 2.0 license, clause 1(c), permits "...modify[ing] the Program in any
   manner for porting or performance improvement purposes..." */

void NN_ModExp(ULONG *a,ULONG *b,ULONG *c,ULONG cDigits,ULONG *d,ULONG dDigits)
{
  ULONG bPower[3][MAX_NN_DIGITS], ci, t[MAX_NN_DIGITS];
  long i;
  ULONG ciBits, j, s;

  /* Store b, b^2 mod d, and b^3 mod d.
   */
  NN_Assign (bPower[0], b, dDigits);
  NN_ModMult (bPower[1], bPower[0], b, d, dDigits);
  NN_ModMult (bPower[2], bPower[1], b, d, dDigits);
  
  NN_ASSIGN_DIGIT (t, 1, dDigits);

  cDigits = NN_Digits (c, cDigits);
  for (i = cDigits - 1; i >= 0; i--) {
    ci = c[i];
    ciBits = NN_DIGIT_BITS;
    
    /* Scan past leading zero bits of most significant digit.
     */
    if (i == (long)(cDigits - 1)) {
      while (! DIGIT_2MSB (ci)) {
        ci <<= 2;
        ciBits -= 2;
      }
    }

    for (j = 0; j < ciBits; j += 2, ci <<= 2) {
      /* Compute t = t^4 * b^s mod d, where s = two MSB's of ci.
       */
      NN_ModMult (t, t, t, d, dDigits);
      NN_ModMult (t, t, t, d, dDigits);
      if ((s = DIGIT_2MSB (ci)) != 0)
        NN_ModMult (t, t, bPower[s-1], d, dDigits);
    }
  }
  
  NN_Assign (a, t, dDigits);
  
  /* Zeroize potentially sensitive information.
   */
  YGECMemset ((UCHAR *)bPower, 0, sizeof (bPower));
  YGECMemset ((UCHAR *)t, 0, sizeof (t));
}

/* Compute a = 1/b mod c, assuming inverse exists.
   
   Lengths: a[digits], b[digits], c[digits].
   Assumes gcd (b, c) = 1, digits < MAX_NN_DIGITS.
 */
void NN_ModInv(ULONG *a,ULONG *b,ULONG *c,ULONG digits)
{
  ULONG q[MAX_NN_DIGITS], t1[MAX_NN_DIGITS], t3[MAX_NN_DIGITS],
    u1[MAX_NN_DIGITS], u3[MAX_NN_DIGITS], v1[MAX_NN_DIGITS],
    v3[MAX_NN_DIGITS], w[2*MAX_NN_DIGITS];
  long u1Sign;

  /* Apply extended Euclidean algorithm, modified to avoid negative
     numbers.
   */
  NN_ASSIGN_DIGIT (u1, 1, digits);
  NN_AssignZero (v1, digits);
  NN_Assign (u3, b, digits);
  NN_Assign (v3, c, digits);
  u1Sign = 1;

  while (! NN_Zero (v3, digits)) {
    NN_Div (q, t3, u3, digits, v3, digits);
    NN_Mult (w, q, v1, digits);
    NN_Add (t1, u1, w, digits);
    NN_Assign (u1, v1, digits);
    NN_Assign (v1, t1, digits);
    NN_Assign (u3, v3, digits);
    NN_Assign (v3, t3, digits);
    u1Sign = -u1Sign;
  }
  
  /* Negate result if sign is negative.
    */
  if (u1Sign < 0)
    NN_Sub (a, c, u1, digits);
  else
    NN_Assign (a, u1, digits);

  /* Zeroize potentially sensitive information.
   */
  YGECMemset ((UCHAR *)q, 0, sizeof (q));
  YGECMemset ((UCHAR *)t1, 0, sizeof (t1));
  YGECMemset ((UCHAR *)t3, 0, sizeof (t3));
  YGECMemset ((UCHAR *)u1, 0, sizeof (u1));
  YGECMemset ((UCHAR *)u3, 0, sizeof (u3));
  YGECMemset ((UCHAR *)v1, 0, sizeof (v1));
  YGECMemset ((UCHAR *)v3, 0, sizeof (v3));
  YGECMemset ((UCHAR *)w, 0, sizeof (w));
}

/* Computes a = gcd(b, c).

   Lengths: a[digits], b[digits], c[digits].
   Assumes b > c, digits < MAX_NN_DIGITS.
 */
void NN_Gcd(ULONG *a,ULONG *b,ULONG *c,ULONG digits)
{
  ULONG t[MAX_NN_DIGITS], u[MAX_NN_DIGITS], v[MAX_NN_DIGITS];

  NN_Assign (u, b, digits);
  NN_Assign (v, c, digits);

  while (! NN_Zero (v, digits)) {
    NN_Mod (t, u, digits, v, digits);
    NN_Assign (u, v, digits);
    NN_Assign (v, t, digits);
  }

  NN_Assign (a, u, digits);

  /* Zeroize potentially sensitive information.
   */
  YGECMemset ((UCHAR *)t, 0, sizeof (t));
  YGECMemset ((UCHAR *)u, 0, sizeof (u));
  YGECMemset ((UCHAR *)v, 0, sizeof (v));
}

/* Returns sign of a - b.

   Lengths: a[digits], b[digits].
 */
long NN_Cmp(ULONG *a,ULONG *b,ULONG digits)
{
  long i;
  
  for (i = digits - 1; i >= 0; i--) {
    if (a[i] > b[i])
      return (1);
    if (a[i] < b[i])
      return (-1);
  }

  return (0);
}

/* Returns nonzero iff a is zero.

   Lengths: a[digits].
 */
long NN_Zero(ULONG *a,ULONG digits)
{
  ULONG i;
  
  for (i = 0; i < digits; i++)
    if (a[i])
      return (0);
    
  return (1);
}

/* Returns the significant length of a in bits.

   Lengths: a[digits].
 */
ULONG NN_Bits(ULONG *a,ULONG digits)
{
  if ((digits = NN_Digits (a, digits)) == 0)
    return (0);

  return ((digits - 1) * NN_DIGIT_BITS + NN_DigitBits (a[digits-1]));
}

/* Returns the significant length of a in digits.

   Lengths: a[digits].
 */
ULONG NN_Digits(ULONG *a,ULONG digits)
{
  long i;
  
  for (i = digits - 1; i >= 0; i--)
    if (a[i])
      break;

  return (i + 1);
}

/* Computes a = b + c*d, where c is a digit. Returns carry.

   Lengths: a[digits], b[digits], d[digits].
 */
ULONG NN_AddDigitMult (ULONG *a,ULONG *b,ULONG c,ULONG *d,ULONG digits)
{
  ULONG carry, t[2];
  ULONG i;

  if (c == 0)    return (0);

  carry = 0;
  for (i = 0; i < digits; i++) {
    NN_DigitMult (t, c, d[i]);
    if ((a[i] = b[i] + carry) < carry)
      carry = 1;
    else
      carry = 0;
    if ((a[i] += t[0]) < t[0])
      carry++;
    carry += t[1];
  }
  
  return (carry);
}

/* Computes a = b - c*d, where c is a digit. Returns borrow.

   Lengths: a[digits], b[digits], d[digits].
 */
ULONG NN_SubDigitMult (ULONG *a,ULONG *b,ULONG c,ULONG *d,ULONG digits)
{
  ULONG borrow, t[2];
  ULONG i;

  if (c == 0)
    return (0);

  borrow = 0;
  for (i = 0; i < digits; i++) {
    NN_DigitMult (t, c, d[i]);
    if ((a[i] = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
      borrow = 1;
    else
      borrow = 0;
    if ((a[i] -= t[0]) > (MAX_NN_DIGIT - t[0]))
      borrow++;
    borrow += t[1];
  }
  
  return (borrow);
}

/* Returns the significant length of a in bits, where a is a digit.
 */
ULONG NN_DigitBits(ULONG a)
{
  ULONG i;
  
  for (i = 0; i < NN_DIGIT_BITS; i++, a >>= 1)
    if (a == 0)
      break;
    
  return (i);
}
/* Computes a = b * c, where b and c are digits.

   Lengths: a[2].
 */
void NN_DigitMult(ULONG a[2],ULONG b,ULONG c)
{
  ULONG t, u;
  USHORT bHigh, bLow, cHigh, cLow;

  bHigh = (USHORT)HIGH_HALF (b);
  bLow = (USHORT)LOW_HALF (b);
  cHigh = (USHORT)HIGH_HALF (c);
  cLow = (USHORT)LOW_HALF (c);

  a[0] = (ULONG)bLow * (ULONG)cLow;
  t = (ULONG)bLow * (ULONG)cHigh;
  u = (ULONG)bHigh * (ULONG)cLow;
  a[1] = (ULONG)bHigh * (ULONG)cHigh;
  
  if ((t += u) < u)
    a[1] += TO_HIGH_HALF (1);
  u = TO_HIGH_HALF (t);
  
  if ((a[0] += u) < u)
    a[1]++;
  a[1] += HIGH_HALF (t);
}

/* Sets a = b / c, where a and c are digits.

   Lengths: b[2].
   Assumes b[1] < c and HIGH_HALF (c) > 0. For efficiency, c should be
   normalized.
 */
void NN_DigitDiv(ULONG *a,ULONG b[2],ULONG c)
{
  ULONG t[2], u, v;
  USHORT aHigh, aLow, cHigh, cLow;

  cHigh = (USHORT)HIGH_HALF (c);
  cLow = (USHORT)LOW_HALF (c);

  t[0] = b[0];
  t[1] = b[1];

  /* Underestimate high half of quotient and subtract.
   */
  if (cHigh == MAX_NN_HALF_DIGIT)
    aHigh = (USHORT)HIGH_HALF (t[1]);
  else
    aHigh = (USHORT)(t[1] / (cHigh + 1));
  u = (ULONG)aHigh * (ULONG)cLow;
  v = (ULONG)aHigh * (ULONG)cHigh;
  if ((t[0] -= TO_HIGH_HALF (u)) > (MAX_NN_DIGIT - TO_HIGH_HALF (u)))
    t[1]--;
  t[1] -= HIGH_HALF (u);
  t[1] -= v;

  /* Correct estimate.
   */
  while ((t[1] > cHigh) ||
         ((t[1] == cHigh) && (t[0] >= TO_HIGH_HALF (cLow)))) {
    if ((t[0] -= TO_HIGH_HALF (cLow)) > MAX_NN_DIGIT - TO_HIGH_HALF (cLow))
      t[1]--;
    t[1] -= cHigh;
    aHigh++;
  }

  /* Underestimate low half of quotient and subtract.
   */
  if (cHigh == MAX_NN_HALF_DIGIT)
    aLow = (USHORT)LOW_HALF (t[1]);
  else
    aLow = 
      (USHORT)((TO_HIGH_HALF (t[1]) + HIGH_HALF (t[0])) / (cHigh + 1));
  u = (ULONG)aLow * (ULONG)cLow;
  v = (ULONG)aLow * (ULONG)cHigh;
  if ((t[0] -= u) > (MAX_NN_DIGIT - u))
    t[1]--;
  if ((t[0] -= TO_HIGH_HALF (v)) > (MAX_NN_DIGIT - TO_HIGH_HALF (v)))
    t[1]--;
  t[1] -= HIGH_HALF (v);

  /* Correct estimate.
   */
  while ((t[1] > 0) || ((t[1] == 0) && t[0] >= c)) {
    if ((t[0] -= c) > (MAX_NN_DIGIT - c))
      t[1]--;
    aLow++;
  }
  
  *a = TO_HIGH_HALF (aHigh) + aLow;
}
