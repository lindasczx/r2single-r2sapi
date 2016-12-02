// Based on public domain code written in 2012 by Samuel Neves
#ifndef _RAR_BLAKE2_
#define _RAR_BLAKE2_

///////////////////////////////////////////////////////////////
#include <stdint.h>
//#include <stdlib.h>
#include <memory.h>
#define _forceinline inline
#define ALIGN_VALUE(v, a) ((size_t)(v) + ( (~(size_t)(v) + 1) & ((a) - 1) ) )
#ifdef _WIN64
#define _WIN_64
#else
#define _WIN_32
#endif
#define USE_SSE
typedef unsigned char byte;
static inline uint32_t RawGet4(const byte *D)
{
  return D[0]|(D[1]<<8)|(D[2]<<16)|(D[3]<<24);
}
static inline void RawPut4(uint32_t w, byte *D)
{
  D[0]=(byte)(w);
  D[1]=(byte)(w>>8);
  D[2]=(byte)(w>>16);
  D[3]=(byte)(w>>24);
}
///////////////////////////////////////////////////////////////


#define BLAKE2_DIGEST_SIZE 32

enum blake2s_constant
{
  BLAKE2S_BLOCKBYTES = 64,
  BLAKE2S_OUTBYTES   = 32
};

#define BLAKE_ALIGNMENT 64
#define BLAKE_DATA_SIZE (48 + 2 * BLAKE2S_BLOCKBYTES)
// Alignment to 64 improves performance of both SSE and non-SSE versions.
// Alignment to n*16 is required for SSE version, so we selected 64.
// We use the custom alignment scheme instead of __declspec(align(x)),
// because it is less compiler dependent. Also the compiler directive
// does not help if structure is a member of class allocated through
// 'new' operator.
// 使用该结构体之前必须调用Init函数，否则后果很严重
typedef struct blake2s_state
{
	// buffer and uint32 h[8], t[2], f[2];
	byte ubuf[BLAKE_DATA_SIZE + BLAKE_ALIGNMENT];

	byte *buf;       // byte   buf[2 * BLAKE2S_BLOCKBYTES].
	uint32_t *h, *t, *f; // uint32 h[8], t[2], f[2].

	size_t buflen;
	byte last_node;
} blake2s_state;

#ifdef RAR_SMP
class ThreadPool;
#endif

typedef struct blake2sp_state
{
  blake2s_state S[8];
  blake2s_state R;
  byte buf[8 * BLAKE2S_BLOCKBYTES];
  size_t buflen;

#ifdef RAR_SMP
  ThreadPool *ThPool;
  uint MaxThreads;
#endif
} blake2sp_state;

void blake2sp_init( blake2sp_state *S );
void blake2sp_update( blake2sp_state *S, const byte *in, size_t inlen );
void blake2sp_final( blake2sp_state *S, byte *digest );
void blake2s_init_param( blake2s_state *S, uint32_t node_offset, uint32_t node_depth);
void blake2s_update( blake2s_state *S, const byte *in, size_t inlen );
void blake2s_final( blake2s_state *S, byte *digest );
int blake2s_compress_sse( blake2s_state *S, const byte block[BLAKE2S_BLOCKBYTES] );
void sse_prefetch_L1(char * p);

#endif

