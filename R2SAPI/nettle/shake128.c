/* shake128.c
   
   The sha3 hash function, extended output.

   This file is not part of GNU Nettle.

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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <stddef.h>
#include <string.h>

#include "sha3.h"

#include "nettle-write.h"

#define MIN(a,b) ((a)<(b)?(a):(b))

void
shake128_init (struct shake128_ctx *ctx)
{
  memset (ctx, 0, offsetof (struct shake128_ctx, block));
}

void
shake128_update (struct shake128_ctx *ctx,
		 size_t length,
		 const uint8_t *data)
{
  ctx->index = _sha3_update (&ctx->state, SHAKE128_BLOCK_SIZE, ctx->block,
			     ctx->index, length, data);
}

void
shake128_digest(struct shake128_ctx *ctx,
		size_t length,
		uint8_t *digest)
{
	_shake_pad (&ctx->state, SHAKE128_BLOCK_SIZE, ctx->block, ctx->index);
  
	while (length > 0) {
	        size_t blockSize = MIN(length, SHAKE128_BLOCK_SIZE);
		_nettle_write_le64 (length, digest, ctx->state.a);
		digest += blockSize;
		length -= blockSize;

		if (length > 0) sha3_permute(&ctx->state);
	}

	//shake128_init (ctx);
}
