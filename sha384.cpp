#include "sha384.h"
#include <sstream>
#include <fstream>
#include <iomanip>

#define SHA2_SHFR(x, n)    (x >> n)
#define SHA2_ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define SHA2_ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define SHA2_CH(x, y, z)  ((x & y) ^ (~x & z))
#define SHA2_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define SHA384_F1(x) (SHA2_ROTR(x, 28) ^ SHA2_ROTR(x, 34) ^ SHA2_ROTR(x, 39))
#define SHA384_F2(x) (SHA2_ROTR(x, 14) ^ SHA2_ROTR(x, 18) ^ SHA2_ROTR(x, 41))
#define SHA384_F3(x) (SHA2_ROTR(x,  1) ^ SHA2_ROTR(x,  8) ^ SHA2_SHFR(x,  7))
#define SHA384_F4(x) (SHA2_ROTR(x, 19) ^ SHA2_ROTR(x, 61) ^ SHA2_SHFR(x,  6))

using namespace std;

const unsigned long long SHA384::K[ROUNDS] = {
	0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
	0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
	0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
	0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
	0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
	0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
	0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
	0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
	0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
	0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
	0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
	0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
	0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
	0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
	0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
	0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
	0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
	0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
	0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
	0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
	0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
	0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
	0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
	0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
	0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
	0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
	0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
	0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
	0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
	0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
	0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
	0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
	0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
	0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
	0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
	0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
	0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
	0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
	0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
	0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

void SHA384::buffer_to_block(const string& buffer, uint64_t block[])
{
	/* Convert the std::string (byte buffer) to a uint64_t array (MSB) */
	for (unsigned i = 0; i < BLOCK_LLS; ++i) {
		block[i] = (uint64_t)(buffer[(i << 3) + 7] & 0xff)
			| (uint64_t)(buffer[(i << 3) + 6] & 0xff) << 8
			| (uint64_t)(buffer[(i << 3) + 5] & 0xff) << 16
			| (uint64_t)(buffer[(i << 3) + 4] & 0xff) << 24
			| (uint64_t)(buffer[(i << 3) + 3] & 0xff) << 32
			| (uint64_t)(buffer[(i << 3) + 2] & 0xff) << 40
			| (uint64_t)(buffer[(i << 3) + 1] & 0xff) << 48
			| (uint64_t)(buffer[(i << 3) + 0] & 0xff) << 56;
	}
}

void SHA384::read(std::istream& is, string& s, int max)
{
	char sbuf[BLOCK_BYTES];
	is.read(sbuf, max);
	s.assign(sbuf, is.gcount());
}

void SHA384::reset()
{
	digest[0] = 0xcbbb9d5dc1059ed8ULL;
	digest[1] = 0x629a292a367cd507ULL;
	digest[2] = 0x9159015a3070dd17ULL;
	digest[3] = 0x152fecd8f70e5939ULL;
	digest[4] = 0x67332667ffc00b31ULL;
	digest[5] = 0x8eb44a8768581511ULL;
	digest[6] = 0xdb0c2e0d64f98fa7ULL;
	digest[7] = 0x47b5481dbefa4fa4ULL;
	transformCnt = 0;
	buffer = ""s;
}

void SHA384::update(istream& is)
{
	string restOfBuffer;
	read(is, restOfBuffer, BLOCK_BYTES - buffer.size());
	buffer += restOfBuffer;

	while (is) {
		uint64_t block[BLOCK_LLS];
		buffer_to_block(buffer, block);
		transform(block);
		read(is, buffer, BLOCK_BYTES);
	}
}

void SHA384::update(const string& s)
{
	istringstream is(s);
	update(is);
}

SHA384::SHA384()
{
	reset();
}

SHA384::SHA384(const string& s) : SHA384()
{
	reset();
	update(s);
}

void SHA384::transform(uint64_t block[])
{
	uint64_t state[DIGEST_LLS];
	uint64_t W[ROUNDS];
	for (unsigned i = 0; i != DIGEST_LLS; ++i)
		state[i] = digest[i];
	for (unsigned i = 0; i != BLOCK_LLS; ++i)
		W[i] = block[i];
	for (unsigned i = BLOCK_LLS; i != ROUNDS; ++i)
		W[i] = W[i - 16] + SHA384_F3(W[i - 15]) + W[i - 7] + SHA384_F4(W[i - 2]);

	for (int i = 0; i != ROUNDS; ++i) {
		uint64_t tmp1 = SHA384_F1(state[0]) + SHA2_MAJ(state[0], state[1], state[2]);
		uint64_t tmp2 = K[i] + W[i] + SHA384_F2(state[4]) + SHA2_CH(state[4], state[5], state[6]) + state[7];
		for (int j = DIGEST_LLS; --j; )
			state[j] = state[j - 1];
		state[4] += tmp2;
		state[0] = tmp1 + tmp2;
	}
	for (unsigned i = 0; i != DIGEST_LLS; ++i)
		digest[i] += state[i];
	++transformCnt;
}

string SHA384::finalize()
{
	uint64_t len_lo = (transformCnt * BLOCK_BYTES + buffer.size()) << 3;
	uint64_t len_hi = (transformCnt >> 54);

	buffer += 0x80;
	unsigned orgSize = buffer.size();
	while (buffer.size() < BLOCK_BYTES)
		buffer += char{ 0x00 };

	uint64_t block[BLOCK_LLS];
	buffer_to_block(buffer, block);

	if (orgSize > BLOCK_BYTES - 16U) {
		transform(block);
		for (int i = BLOCK_LLS - 2; ~--i;)
			block[i] = 0;
	}

	block[BLOCK_LLS - 1] = len_lo;
	block[BLOCK_LLS - 2] = len_hi;
	transform(block);

	ostringstream result;
	for (unsigned i = 0; i < DIGEST_LLS - 2; ++i)
		result << hex << setfill('0') << setw(16) << digest[i];

	reset();

	return result.str();
}