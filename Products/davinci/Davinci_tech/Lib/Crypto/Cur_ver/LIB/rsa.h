/* RSA.H - header file for RSA.C
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */
#ifndef _RSAREF_H_
#define _RSAREF_H_ 1
#include "..\Export\Crypto.h"
/* Random structure.
 */
typedef struct 
{
  ULONG bytesNeeded;
  UCHAR state[16];
  ULONG outputAvailable;
  UCHAR output[16];
} R_RANDOM_STRUCT;

long RSAPublicBlock(UCHAR *, ULONG *, UCHAR *, ULONG,R_RSA_PUBLIC_KEY *);
long RSAPrivateBlock(UCHAR *, ULONG *, UCHAR *, ULONG,R_RSA_PRIVATE_KEY *);
long RSAFilter(ULONG *, ULONG, ULONG *, ULONG);
long RelativelyPrime(ULONG *, ULONG, ULONG *, ULONG);
long GeneratePrime(ULONG *, ULONG *, ULONG *, ULONG *, ULONG,R_RANDOM_STRUCT*);
long R_GenerateBytes(UCHAR *, ULONG,R_RANDOM_STRUCT *);
long ProbablePrime(ULONG *, ULONG);
long SmallFactor(ULONG *, ULONG);
long FermatTest(ULONG *, ULONG);

#endif
