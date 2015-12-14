/* sha3.h

   The sha3 hash function (aka Keccak).

   Copyright (C) 2012 Niels Möller

   This file is part of GNU Nettle.

   GNU Nettle is free software: you can redistribute it and/or
   modify it under the terms of either:

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at your
       option) any later version.

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at your
       option) any later version.

   or both in parallel, as here.

   GNU Nettle is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see http://www.gnu.org/licenses/.
*/
 
#ifndef NETTLE_SHA3_H_INCLUDED
#define NETTLE_SHA3_H_INCLUDED

#include "nettle-types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define sha3_permute nettle_sha3_permute
#define _sha3_update _nettle_sha3_update
#define _sha3_pad _nettle_sha3_pad
#define sha3_224_init nettle_sha3_224_init
#define sha3_224_update nettle_sha3_224_update
#define sha3_224_digest nettle_sha3_224_digest
#define sha3_256_init nettle_sha3_256_init
#define sha3_256_update nettle_sha3_256_update
#define sha3_256_digest nettle_sha3_256_digest
#define sha3_384_init nettle_sha3_384_init
#define sha3_384_update nettle_sha3_384_update
#define sha3_384_digest nettle_sha3_384_digest
#define sha3_512_init nettle_sha3_512_init
#define sha3_512_update nettle_sha3_512_update
#define sha3_512_digest nettle_sha3_512_digest

#define _shake_pad _nettle_shake_pad
#define shake128_init nettle_shake128_init
#define shake128_update nettle_shake128_update
#define shake128_digest nettle_shake128_digest
#define shake256_init nettle_shake256_init
#define shake256_update nettle_shake256_update
#define shake256_digest nettle_shake256_digest

/* The sha3 state is a 5x5 matrix of 64-bit words. In the notation of
   Keccak description, S[x,y] is element x + 5*y, so if x is
   interpreted as the row index and y the column index, it is stored
   in column-major order. */
#define SHA3_STATE_LENGTH 25

/* The "width" is 1600 bits or 200 octets */
struct sha3_state
{
  uint64_t a[SHA3_STATE_LENGTH];
};

void
sha3_permute (struct sha3_state *state);

unsigned
_sha3_update (struct sha3_state *state,
	      unsigned block_size, uint8_t *block,
	      unsigned pos,
	      size_t length, const uint8_t *data);
void
_sha3_pad (struct sha3_state *state,
	   unsigned block_size, uint8_t *block, unsigned pos);

void
_shake_pad (struct sha3_state *state,
	   unsigned block_size, uint8_t *block, unsigned pos);

/* The "capacity" is set to 2*(digest size), 512 bits or 64 octets.
   The "rate" is the width - capacity, or width - 2 * (digest
   size). */

#define SHA3_224_DIGEST_SIZE 28
#define SHA3_224_BLOCK_SIZE 144

#define SHA3_256_DIGEST_SIZE 32
#define SHA3_256_BLOCK_SIZE 136

#define SHA3_384_DIGEST_SIZE 48
#define SHA3_384_BLOCK_SIZE 104

#define SHA3_512_DIGEST_SIZE 64
#define SHA3_512_BLOCK_SIZE 72

#define SHAKE128_BLOCK_SIZE 168

#define SHAKE256_BLOCK_SIZE 136

/* For backwards compatibility */
#define SHA3_224_DATA_SIZE SHA3_224_BLOCK_SIZE
#define SHA3_256_DATA_SIZE SHA3_256_BLOCK_SIZE
#define SHA3_384_DATA_SIZE SHA3_384_BLOCK_SIZE
#define SHA3_512_DATA_SIZE SHA3_512_BLOCK_SIZE

struct sha3_224_ctx
{
  struct sha3_state state;
  unsigned index;
  uint8_t block[SHA3_224_BLOCK_SIZE];
};

void
sha3_224_init (struct sha3_224_ctx *ctx);

void
sha3_224_update (struct sha3_224_ctx *ctx,
		 size_t length,
		 const uint8_t *data);

void
sha3_224_digest(struct sha3_224_ctx *ctx,
		size_t length,
		uint8_t *digest);

struct sha3_256_ctx
{
  struct sha3_state state;
  unsigned index;
  uint8_t block[SHA3_256_BLOCK_SIZE];
};

void
sha3_256_init (struct sha3_256_ctx *ctx);

void
sha3_256_update (struct sha3_256_ctx *ctx,
		 size_t length,
		 const uint8_t *data);

void
sha3_256_digest(struct sha3_256_ctx *ctx,
		size_t length,
		uint8_t *digest);

struct sha3_384_ctx
{
  struct sha3_state state;
  unsigned index;
  uint8_t block[SHA3_384_BLOCK_SIZE];
};

void
sha3_384_init (struct sha3_384_ctx *ctx);

void
sha3_384_update (struct sha3_384_ctx *ctx,
		 size_t length,
		 const uint8_t *data);

void
sha3_384_digest(struct sha3_384_ctx *ctx,
		size_t length,
		uint8_t *digest);

struct sha3_512_ctx
{
  struct sha3_state state;
  unsigned index;
  uint8_t block[SHA3_512_BLOCK_SIZE];
};

void
sha3_512_init (struct sha3_512_ctx *ctx);

void
sha3_512_update (struct sha3_512_ctx *ctx,
		 size_t length,
		 const uint8_t *data);

void
sha3_512_digest(struct sha3_512_ctx *ctx,
		size_t length,
		uint8_t *digest);

struct shake128_ctx
{
  struct sha3_state state;
  unsigned index;
  uint8_t block[SHAKE128_BLOCK_SIZE];
};

void
shake128_init (struct shake128_ctx *ctx);

void
shake128_update (struct shake128_ctx *ctx,
		 size_t length,
		 const uint8_t *data);

void
shake128_digest(struct shake128_ctx *ctx,
		size_t length,
		uint8_t *digest);

struct shake256_ctx
{
  struct sha3_state state;
  unsigned index;
  uint8_t block[SHAKE256_BLOCK_SIZE];
};

void
shake256_init (struct shake256_ctx *ctx);

void
shake256_update (struct shake256_ctx *ctx,
		 size_t length,
		 const uint8_t *data);

void
shake256_digest(struct shake256_ctx *ctx,
		size_t length,
		uint8_t *digest);


#ifdef __cplusplus
}
#endif

#endif /* NETTLE_SHA3_H_INCLUDED */
