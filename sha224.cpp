#include "sha224.h"
#include <sstream>
#include <fstream>
#include <iomanip>

#define SHA2_SHFR(x, n)    (x >> n)
#define SHA2_ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define SHA2_ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define SHA2_CH(x, y, z)  ((x & y) ^ (~x & z))
#define SHA2_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define SHA256_F1(x) (SHA2_ROTR(x,  2) ^ SHA2_ROTR(x, 13) ^ SHA2_ROTR(x, 22))
#define SHA256_F2(x) (SHA2_ROTR(x,  6) ^ SHA2_ROTR(x, 11) ^ SHA2_ROTR(x, 25))
#define SHA256_F3(x) (SHA2_ROTR(x,  7) ^ SHA2_ROTR(x, 18) ^ SHA2_SHFR(x,  3))
#define SHA256_F4(x) (SHA2_ROTR(x, 17) ^ SHA2_ROTR(x, 19) ^ SHA2_SHFR(x, 10))

using namespace std;

const uint32_t SHA224::K[ROUNDS] = {
	0x428a2f98U, 0x71374491U, 0xb5c0fbcfU, 0xe9b5dba5U,
	0x3956c25bU, 0x59f111f1U, 0x923f82a4U, 0xab1c5ed5U,
	0xd807aa98U, 0x12835b01U, 0x243185beU, 0x550c7dc3U,
	0x72be5d74U, 0x80deb1feU, 0x9bdc06a7U, 0xc19bf174U,
	0xe49b69c1U, 0xefbe4786U, 0x0fc19dc6U, 0x240ca1ccU,
	0x2de92c6fU, 0x4a7484aaU, 0x5cb0a9dcU, 0x76f988daU,
	0x983e5152U, 0xa831c66dU, 0xb00327c8U, 0xbf597fc7U,
	0xc6e00bf3U, 0xd5a79147U, 0x06ca6351U, 0x14292967U,
	0x27b70a85U, 0x2e1b2138U, 0x4d2c6dfcU, 0x53380d13U,
	0x650a7354U, 0x766a0abbU, 0x81c2c92eU, 0x92722c85U,
	0xa2bfe8a1U, 0xa81a664bU, 0xc24b8b70U, 0xc76c51a3U,
	0xd192e819U, 0xd6990624U, 0xf40e3585U, 0x106aa070U,
	0x19a4c116U, 0x1e376c08U, 0x2748774cU, 0x34b0bcb5U,
	0x391c0cb3U, 0x4ed8aa4aU, 0x5b9cca4fU, 0x682e6ff3U,
	0x748f82eeU, 0x78a5636fU, 0x84c87814U, 0x8cc70208U,
	0x90befffaU, 0xa4506cebU, 0xbef9a3f7U, 0xc67178f2U
};

void SHA224::buffer_to_block(const string& buffer, uint32_t block[])
{
	/* Convert the std::string (byte buffer) to a uint32_t array (MSB) */
	for (unsigned i = 0; i < BLOCK_INTS; ++i) {
		block[i] = (buffer[(i << 2) + 3] & 0xff)
			| (buffer[(i << 2) + 2] & 0xff) << 8
			| (buffer[(i << 2) + 1] & 0xff) << 16
			| (buffer[(i << 2) + 0] & 0xff) << 24;
	}
}

void SHA224::read(std::istream& is, string& s, int max)
{
	char sbuf[BLOCK_BYTES];
	is.read(sbuf, max);
	s.assign(sbuf, is.gcount());
}

void SHA224::reset()
{
	digest[0] = 0xc1059ed8U;
	digest[1] = 0x367cd507U;
	digest[2] = 0x3070dd17U;
	digest[3] = 0xf70e5939U;
	digest[4] = 0xffc00b31U;
	digest[5] = 0x68581511U;
	digest[6] = 0x64f98fa7U;
	digest[7] = 0xbefa4fa4U;
	transformCnt = 0;
	buffer = ""s;
}

void SHA224::update(istream& is)
{
	string restOfBuffer;
	read(is, restOfBuffer, BLOCK_BYTES - buffer.size());
	buffer += restOfBuffer;

	while (is) {
		uint32_t block[BLOCK_INTS];
		buffer_to_block(buffer, block);
		transform(block);
		read(is, buffer, BLOCK_BYTES);
	}
}

void SHA224::update(const string& s)
{
	istringstream is(s);
	update(is);
}

SHA224::SHA224()
{
	reset();
}

SHA224::SHA224(const string& s) : SHA224()
{
	update(s);
}

void SHA224::transform(uint32_t block[])
{
	uint32_t state[DIGEST_INTS];
	uint32_t W[ROUNDS];
	for (unsigned i = 0; i != DIGEST_INTS; ++i)
		state[i] = digest[i];
	for (unsigned i = 0; i != BLOCK_INTS; ++i)
		W[i] = block[i];
	for (unsigned i = BLOCK_INTS; i != ROUNDS; ++i)
		W[i] = W[i - 16] + SHA256_F3(W[i - 15]) + W[i - 7] + SHA256_F4(W[i - 2]);

	for (int i = 0; i != ROUNDS; ++i) {
		uint32_t tmp1 = SHA256_F1(state[0]) + SHA2_MAJ(state[0], state[1], state[2]);
		uint32_t tmp2 = K[i] + W[i] + SHA256_F2(state[4]) + SHA2_CH(state[4], state[5], state[6]) + state[7];
		for (int j = DIGEST_INTS; --j; )
			state[j] = state[j - 1];
		state[4] += tmp2;
		state[0] = tmp1 + tmp2;
	}
	for (unsigned i = 0; i != DIGEST_INTS; ++i)
		digest[i] += state[i];
	++transformCnt;
}

string SHA224::finalize()
{
	uint64_t totalBits = (transformCnt * BLOCK_BYTES + buffer.size()) << 3;

	buffer += 0x80;
	unsigned orgSize = buffer.size();
	while (buffer.size() < BLOCK_BYTES) {
		buffer += char{ 0x00 };
	}

	uint32_t block[BLOCK_INTS];
	buffer_to_block(buffer, block);

	if (orgSize > BLOCK_BYTES - 8U) {
		transform(block);
		for (int i = BLOCK_INTS - 2; ~--i;)
			block[i] = 0;
	}

	block[BLOCK_INTS - 1] = (uint32_t)totalBits;
	block[BLOCK_INTS - 2] = (uint32_t)(totalBits >> 32);
	transform(block);

	ostringstream result;
	for (unsigned i = 0; i < DIGEST_INTS - 1; ++i)
		result << hex << setfill('0') << setw(8) << digest[i];

	reset();

	return result.str();
}