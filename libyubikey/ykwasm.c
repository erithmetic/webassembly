/* ykgenerate.c --- Example command line interface for generating OTPs.
 *
 * Written by Olov Danielson <olov@yubico.com>.
 * Copyright (c) 2006-2012 Yubico AB
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "yubikey.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <limits.h>

char *ykgenerate (
  char *aeskey, 
  char *yk_internalname,
  char *yk_counter,
  char *yk_low,
  char *yk_high,
  char *yk_use
)
{
  uint8_t key[YUBIKEY_KEY_SIZE];
  char *otp = (char *) malloc(YUBIKEY_OTP_SIZE + 1);
  yubikey_token_st tok;

  /* Initiate pseudo-random generator */
  srand (time (NULL));

  if (strlen (aeskey) != 32)
  {
    return "error: Hex encoded AES-key must be 32 characters.";
  }

  if (strlen (yk_internalname) != 12)
  {
    return "error: Hex encoded yk_internalname must be 12 characters.";
  }

  if (strlen (yk_counter) != 4)
  {
    return "error: Hex encoded yk_counter must be 4 characters.";
  }

  if (strlen (yk_low) != 4)
  {
    return "error: Hex encoded yk_low must be 4 characters.";
  }

  if (strlen (yk_high) != 2)
  {
    return "error: Hex encoded yk_high must be 2 characters.";
  }

  if (strlen (yk_use) != 2)
  {
    return "error: Hex encoded yk_use must be 2 characters.";
  }

  yubikey_hex_decode ((char *) key, aeskey, YUBIKEY_KEY_SIZE);

  /* Fill upp tok with values */
  yubikey_hex_decode ((char *) &tok.uid, yk_internalname, 6);
  {
    unsigned long i;
    char *endptr;

    i = strtoul (yk_counter, &endptr, 16);
    if (*endptr != '\0' || i == ULONG_MAX)
    {
      return "error: Hex encoded yk_counter must be 4 hex characters.";
    }
    tok.ctr = i;

    i = strtoul (yk_low, &endptr, 16);
    if (*endptr != '\0' || i == ULONG_MAX)
    {
      return "error: Hex encoded yk_low must be 4 hex characters.";
    }
    tok.tstpl = i;
  }
  yubikey_hex_decode ((char *) &tok.tstph, yk_high, 1);
  yubikey_hex_decode ((char *) &tok.use, yk_use, 1);
  tok.rnd = rand ();
  tok.crc = ~yubikey_crc16 ((void *) &tok, sizeof (tok) - sizeof (tok.crc));

  yubikey_generate ((void *) &tok, key, otp);

  printf ("%s\n", otp);

  return otp;
}
