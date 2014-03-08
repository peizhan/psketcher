/**************************************************************************\
 * Copyright (c) Kongsberg Oil & Gas Technologies AS
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\**************************************************************************/

/*!
  \class dimeParam dime/Basic.h
  \brief The dimeParam class is a union of the different parameter types.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <math.h> /* isinf(), isnan(), finite() */
#include <float.h> /* _fpclass(), _isnan(), _finite() */

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif /* HAVE_IEEEFP_H */

#ifdef macintosh
char *strdup(const char *istr)
{
  int len;
  char* rstr;
  len = strlen( istr ) + 1;
  rstr = (char*)malloc( (sizeof(char) * len) );
  strcpy(rstr, istr);
  rstr[len]='\0';
  return rstr;
}
#endif // macintosh

/**************************************************************************/
// copied from Coin/src/tdibits.c
//

/* Returns -1 if value equals negative infinity, +1 if it is equal to
   positive infinity, or 0 if the number is not infinite.
   
   Note that on some systems, this method will always return 0
   (i.e. false positives).
*/

int
dime_isinf(double value)
{
#ifdef HAVE_ISINF
  return isinf(value);
#elif defined(HAVE_FPCLASS)
  if (fpclass(value) == FP_NINF) { return -1; }
  if (fpclass(value) == FP_PINF) { return +1; }
  return 0;
#elif defined(HAVE__FPCLASS)
  if (_fpclass(value) == _FPCLASS_NINF) { return -1; }
  if (_fpclass(value) == _FPCLASS_PINF) { return +1; }
  return 0;
#else
  /* FIXME: it might be possible to investigate the fp bits and decide
     in a portable manner whether or not they represent an infinite. A
     groups.google.com search turned up inconclusive. 20030919
     mortene. */
  return 0;
#endif
}

/* Returns 0 if the bitpattern of the \a value argument is not a valid
   floating point number, otherwise returns non-zero.

   Note that on some systems, this method will always return 0
   (i.e. false positives).
*/
int
dime_isnan(double value)
{
#ifdef HAVE_ISNAN
  return isnan(value);
#elif defined(HAVE__ISNAN)
  return _isnan(value);
#elif defined(HAVE_FPCLASS)
  if (fpclass(value) == FP_SNAN) { return 1; }
  if (fpclass(value) == FP_QNAN) { return 1; }
  return 0;
#elif defined(HAVE__FPCLASS)
  if (_fpclass(value) == _FPCLASS_SNAN) { return 1; }
  if (_fpclass(value) == _FPCLASS_QNAN) { return 1; }
  return 0;
#else
  /* FIXME: it might be possible to investigate the fp bits and decide
     in a portable manner whether or not they represent a NaN. A
     groups.google.com search turned up inconclusive. 20030919
     mortene. */
  return 0;
#endif
}

/* Returns 0 if the bitpattern of the \a value argument is not a valid
   floating point number, or an infinite number, otherwise returns
   non-zero.

   Note that on some systems, this method will always return 1
   (i.e. false positives).
*/
int
dime_finite(double value)
{
#ifdef HAVE_FINITE
  return finite(value);
#elif defined(HAVE__FINITE)
  return _finite(value);
#else
  return !dime_isinf(value) && !dime_isnan(value);
#endif
}
