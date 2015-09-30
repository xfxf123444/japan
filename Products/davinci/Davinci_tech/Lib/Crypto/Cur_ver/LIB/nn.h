/* NN.H - header file for NN.C
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */

/* Type definitions.
 */

/* Constants.

   Note: MAX_NN_DIGITS is long enough to hold any RSA modulus, plus
   one more digit as required by R_GeneratePEMKeys (for n and phiN,
   whose lengths must be even). All natural numbers have at most
   MAX_NN_DIGITS digits, except for double-length intermediate values
   in NN_Mult (t), NN_ModMult (t), NN_ModInv (w), and NN_Div (c).
 */
/* Length of digit in bits */
#define NN_DIGIT_BITS 32
#define NN_HALF_DIGIT_BITS 16
/* Length of digit in bytes */
#define NN_DIGIT_LEN (NN_DIGIT_BITS / 8)
/* Maximum length in digits */
#define MAX_NN_DIGITS \
  ((MAX_RSA_MODULUS_LEN + NN_DIGIT_LEN - 1) / NN_DIGIT_LEN + 1)
/* Maximum digits */
#define MAX_NN_DIGIT 0xffffffff
#define MAX_NN_HALF_DIGIT 0xffff

/* Macros.
 */
#define LOW_HALF(x) ((x) & MAX_NN_HALF_DIGIT)
#define HIGH_HALF(x) (((x) >> NN_HALF_DIGIT_BITS) & MAX_NN_HALF_DIGIT)
#define TO_HIGH_HALF(x) (((ULONG)(x)) << NN_HALF_DIGIT_BITS)
#define DIGIT_MSB(x) (ULONG)(((x) >> (NN_DIGIT_BITS - 1)) & 1)
#define DIGIT_2MSB(x) (ULONG)(((x) >> (NN_DIGIT_BITS - 2)) & 3)

void NN_Decode(ULONG *, ULONG, UCHAR *, ULONG);
void NN_Encode(UCHAR *, ULONG, ULONG *, ULONG);

void NN_Assign(ULONG *, ULONG *, ULONG);
void NN_AssignZero(ULONG *, ULONG);
void NN_Assign2Exp(ULONG *, ULONG, ULONG);

ULONG NN_Add(ULONG *, ULONG *, ULONG *, ULONG);
ULONG NN_Sub(ULONG *, ULONG *, ULONG *, ULONG);
void NN_Mult(ULONG *, ULONG *, ULONG *, ULONG);
void NN_Div(ULONG *, ULONG *, ULONG *, ULONG, ULONG *, ULONG);
ULONG NN_LShift(ULONG *, ULONG *, ULONG, ULONG);
ULONG NN_RShift(ULONG *, ULONG *, ULONG, ULONG);

void NN_Mod(ULONG *, ULONG *, ULONG, ULONG *, ULONG);
void NN_ModMult(ULONG *, ULONG *, ULONG *, ULONG *, ULONG);
void NN_ModExp(ULONG *, ULONG *, ULONG *, ULONG, ULONG *, ULONG);
void NN_ModInv(ULONG *, ULONG *, ULONG *, ULONG);
void NN_Gcd(ULONG *, ULONG *, ULONG *, ULONG);

long NN_Cmp(ULONG *, ULONG *, ULONG);
long NN_Zero(ULONG *, ULONG);
ULONG NN_Bits(ULONG *, ULONG);
ULONG NN_Digits(ULONG *, ULONG);

void NN_DigitMult(ULONG [2], ULONG, ULONG);
void NN_DigitDiv(ULONG *, ULONG [2], ULONG);
ULONG NN_AddDigitMult(ULONG *, ULONG *, ULONG, ULONG *, ULONG);
ULONG NN_SubDigitMult(ULONG *, ULONG *, ULONG, ULONG *, ULONG);

ULONG NN_DigitBits(ULONG);

#define NN_ASSIGN_DIGIT(a, b, digits) {NN_AssignZero (a, digits); a[0] = b;}
#define NN_EQUAL(a, b, digits) (! NN_Cmp (a, b, digits))
#define NN_EVEN(a, digits) (((digits) == 0) || ! (a[0] & 1))
