#include "loaddll/search.h"

/* taken from https://github.com/floodyberry/siphash */

#define ROTL64(a,b) (((a)<<(b))|((a)>>(64-b)))

static uint64_t
U8TO64_LE(const unsigned char *p) {
	return *(const uint64_t *)p;
}

uint64_t
siphash24(const unsigned char key[16], const unsigned char *m, size_t len) {
	uint64_t v0, v1, v2, v3;
	uint64_t mi, k0, k1;
	uint64_t last7;
	size_t i, blocks;

	k0 = U8TO64_LE(key + 0);
	k1 = U8TO64_LE(key + 8);
	v0 = k0 ^ 0x736f6d6570736575ull;
	v1 = k1 ^ 0x646f72616e646f6dull;
	v2 = k0 ^ 0x6c7967656e657261ull;
	v3 = k1 ^ 0x7465646279746573ull;

	last7 = (uint64_t)(len & 0xff) << 56;

#define sipcompress() \
	v0 += v1; v2 += v3; \
	v1 = ROTL64(v1,13);	v3 = ROTL64(v3,16); \
	v1 ^= v0; v3 ^= v2; \
	v0 = ROTL64(v0,32); \
	v2 += v1; v0 += v3; \
	v1 = ROTL64(v1,17); v3 = ROTL64(v3,21); \
	v1 ^= v2; v3 ^= v0; \
	v2 = ROTL64(v2,32);

	for (i = 0, blocks = (len & ~7); i < blocks; i += 8) {
		mi = U8TO64_LE(m + i);
		v3 ^= mi;
		sipcompress()
		sipcompress()
		v0 ^= mi;
	}

	switch (len - blocks) {
		case 7: last7 |= (uint64_t)m[i + 6] << 48; /* fall through */
		case 6: last7 |= (uint64_t)m[i + 5] << 40; /* fall through */
		case 5: last7 |= (uint64_t)m[i + 4] << 32; /* fall through */
		case 4: last7 |= (uint64_t)m[i + 3] << 24; /* fall through */
		case 3: last7 |= (uint64_t)m[i + 2] << 16; /* fall through */
		case 2: last7 |= (uint64_t)m[i + 1] <<  8; /* fall through */
		case 1: last7 |= (uint64_t)m[i + 0]      ; /* fall through */
		case 0:
		default:;
	};
	v3 ^= last7;
	sipcompress()
	sipcompress()
	v0 ^= last7;
	v2 ^= 0xff;
	sipcompress()
	sipcompress()
	sipcompress()
	sipcompress()
	return v0 ^ v1 ^ v2 ^ v3;
}
