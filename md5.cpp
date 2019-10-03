#include "md5.h"
#include <sstream>
#include <iomanip>
#include <fstream>

using namespace std;
///////////////////////////////////////////////

// F, G, H and I are basic MD5 functions.
inline uint32_t MD5::F(uint32_t x, uint32_t y, uint32_t z) {
	return x & y | ~x & z;
}

inline uint32_t MD5::G(uint32_t x, uint32_t y, uint32_t z) {
	return x & z | y & ~z;
}

inline uint32_t MD5::H(uint32_t x, uint32_t y, uint32_t z) {
	return x ^ y ^ z;
}

inline uint32_t MD5::I(uint32_t x, uint32_t y, uint32_t z) {
	return y ^ (x | ~z);
}

// rotate_left rotates x left n bits.
inline uint32_t MD5::rotate_left(uint32_t x, int n) {
	return (x << n) | (x >> (32 - n));
}

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
// Rotation is separate from addition to prevent recomputation.
inline void MD5::FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
	a = rotate_left(a + F(b, c, d) + x + ac, s) + b;
}

inline void MD5::GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
	a = rotate_left(a + G(b, c, d) + x + ac, s) + b;
}

inline void MD5::HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
	a = rotate_left(a + H(b, c, d) + x + ac, s) + b;
}

inline void MD5::II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
	a = rotate_left(a + I(b, c, d) + x + ac, s) + b;
}

//////////////////////////////////////////////

MD5::MD5()
{
	reset();
}

MD5::MD5(const std::string& s) : MD5()
{
	update(s);
}


void MD5::update(const std::string& s)
{
	std::istringstream is(s);
	update(is);
}


void MD5::update(std::istream& is)
{
	std::string rest_of_buffer;
	read(is, rest_of_buffer, BLOCK_BYTES - buffer.size());
	buffer += rest_of_buffer;

	while (is) {
		uint32_t block[BLOCK_INTS];
		buffer_to_block(buffer, block);
		transform(block);
		read(is, buffer, BLOCK_BYTES);
	}
}

void MD5::reset()
{
	/* SHA1 initialization constants */
	digest[0] = 0x67452301U;
	digest[1] = 0xefcdab89U;
	digest[2] = 0x98badcfeU;
	digest[3] = 0x10325476U;

	transforms = 0;
	buffer = ""s;
}

std::string MD5::finalize()
{
	/* Total number of hashed bits */
	uint64_t total_bits = (transforms * BLOCK_BYTES + buffer.size()) << 3;

	/* Padding */
	buffer += 0x80;
	unsigned int org_size = buffer.size();
	while (buffer.size() < BLOCK_BYTES)
		buffer += char{ 0x00 };

	uint32_t block[BLOCK_INTS];
	buffer_to_block(buffer, block);

	if (org_size > BLOCK_BYTES - 8) {
		transform(block);
		for (unsigned int i = 0; i < BLOCK_INTS - 2; ++i)
			block[i] = 0;
	}

	/* Append total_bits, split this uint64_t into two uint32_t */
	block[BLOCK_INTS - 2] = total_bits;
	block[BLOCK_INTS - 1] = (total_bits >> 32);
	transform(block);

	/* Hex std::string */
	
	ostringstream result;
	for (unsigned int i = 0; i < DIGEST_INTS; ++i)
		for (unsigned int j = 0; j < 4; ++j)
			result << hex << setw(2) << setfill('0') << (int)((uint8_t*)(digest + i))[j];
	/* Reset for next run */
	reset();

	return result.str();
}


void MD5::transform(uint32_t block[])
{
	uint32_t a = digest[0], b = digest[1], c = digest[2], d = digest[3];

	/* Round 1 */
	FF(a, b, c, d, block[0], S11, 0xd76aa478U); /* 1 */
	FF(d, a, b, c, block[1], S12, 0xe8c7b756U); /* 2 */
	FF(c, d, a, b, block[2], S13, 0x242070dbU); /* 3 */
	FF(b, c, d, a, block[3], S14, 0xc1bdceeeU); /* 4 */
	FF(a, b, c, d, block[4], S11, 0xf57c0fafU); /* 5 */
	FF(d, a, b, c, block[5], S12, 0x4787c62aU); /* 6 */
	FF(c, d, a, b, block[6], S13, 0xa8304613U); /* 7 */
	FF(b, c, d, a, block[7], S14, 0xfd469501U); /* 8 */
	FF(a, b, c, d, block[8], S11, 0x698098d8U); /* 9 */
	FF(d, a, b, c, block[9], S12, 0x8b44f7afU); /* 10 */
	FF(c, d, a, b, block[10], S13, 0xffff5bb1U); /* 11 */
	FF(b, c, d, a, block[11], S14, 0x895cd7beU); /* 12 */
	FF(a, b, c, d, block[12], S11, 0x6b901122U); /* 13 */
	FF(d, a, b, c, block[13], S12, 0xfd987193U); /* 14 */
	FF(c, d, a, b, block[14], S13, 0xa679438eU); /* 15 */
	FF(b, c, d, a, block[15], S14, 0x49b40821U); /* 16 */

	/* Round 2 */
	GG(a, b, c, d, block[1], S21, 0xf61e2562U); /* 17 */
	GG(d, a, b, c, block[6], S22, 0xc040b340U); /* 18 */
	GG(c, d, a, b, block[11], S23, 0x265e5a51U); /* 19 */
	GG(b, c, d, a, block[0], S24, 0xe9b6c7aaU); /* 20 */
	GG(a, b, c, d, block[5], S21, 0xd62f105dU); /* 21 */
	GG(d, a, b, c, block[10], S22, 0x2441453U); /* 22 */
	GG(c, d, a, b, block[15], S23, 0xd8a1e681U); /* 23 */
	GG(b, c, d, a, block[4], S24, 0xe7d3fbc8U); /* 24 */
	GG(a, b, c, d, block[9], S21, 0x21e1cde6U); /* 25 */
	GG(d, a, b, c, block[14], S22, 0xc33707d6U); /* 26 */
	GG(c, d, a, b, block[3], S23, 0xf4d50d87U); /* 27 */
	GG(b, c, d, a, block[8], S24, 0x455a14edU); /* 28 */
	GG(a, b, c, d, block[13], S21, 0xa9e3e905U); /* 29 */
	GG(d, a, b, c, block[2], S22, 0xfcefa3f8U); /* 30 */
	GG(c, d, a, b, block[7], S23, 0x676f02d9U); /* 31 */
	GG(b, c, d, a, block[12], S24, 0x8d2a4c8aU); /* 32 */

	/* Round 3 */
	HH(a, b, c, d, block[5], S31, 0xfffa3942U); /* 33 */
	HH(d, a, b, c, block[8], S32, 0x8771f681U); /* 34 */
	HH(c, d, a, b, block[11], S33, 0x6d9d6122U); /* 35 */
	HH(b, c, d, a, block[14], S34, 0xfde5380cU); /* 36 */
	HH(a, b, c, d, block[1], S31, 0xa4beea44U); /* 37 */
	HH(d, a, b, c, block[4], S32, 0x4bdecfa9U); /* 38 */
	HH(c, d, a, b, block[7], S33, 0xf6bb4b60U); /* 39 */
	HH(b, c, d, a, block[10], S34, 0xbebfbc70U); /* 40 */
	HH(a, b, c, d, block[13], S31, 0x289b7ec6U); /* 41 */
	HH(d, a, b, c, block[0], S32, 0xeaa127faU); /* 42 */
	HH(c, d, a, b, block[3], S33, 0xd4ef3085U); /* 43 */
	HH(b, c, d, a, block[6], S34, 0x4881d05U); /* 44 */
	HH(a, b, c, d, block[9], S31, 0xd9d4d039U); /* 45 */
	HH(d, a, b, c, block[12], S32, 0xe6db99e5U); /* 46 */
	HH(c, d, a, b, block[15], S33, 0x1fa27cf8U); /* 47 */
	HH(b, c, d, a, block[2], S34, 0xc4ac5665U); /* 48 */

	/* Round 4 */
	II(a, b, c, d, block[0], S41, 0xf4292244U); /* 49 */
	II(d, a, b, c, block[7], S42, 0x432aff97U); /* 50 */
	II(c, d, a, b, block[14], S43, 0xab9423a7U); /* 51 */
	II(b, c, d, a, block[5], S44, 0xfc93a039U); /* 52 */
	II(a, b, c, d, block[12], S41, 0x655b59c3U); /* 53 */
	II(d, a, b, c, block[3], S42, 0x8f0ccc92U); /* 54 */
	II(c, d, a, b, block[10], S43, 0xffeff47dU); /* 55 */
	II(b, c, d, a, block[1], S44, 0x85845dd1U); /* 56 */
	II(a, b, c, d, block[8], S41, 0x6fa87e4fU); /* 57 */
	II(d, a, b, c, block[15], S42, 0xfe2ce6e0U); /* 58 */
	II(c, d, a, b, block[6], S43, 0xa3014314U); /* 59 */
	II(b, c, d, a, block[13], S44, 0x4e0811a1U); /* 60 */
	II(a, b, c, d, block[4], S41, 0xf7537e82U); /* 61 */
	II(d, a, b, c, block[11], S42, 0xbd3af235U); /* 62 */
	II(c, d, a, b, block[2], S43, 0x2ad7d2bbU); /* 63 */
	II(b, c, d, a, block[9], S44, 0xeb86d391U); /* 64 */

	digest[0] += a;
	digest[1] += b;
	digest[2] += c;
	digest[3] += d;

	++transforms;
}

void MD5::buffer_to_block(const std::string& buffer, uint32_t block[])
{
	/* Convert the std::string (byte buffer) to a uint32_t array (MSB) */
	for (unsigned int i = 0; i < BLOCK_INTS; ++i) {
		block[i] = (buffer[i << 2] & 0xff)
			| (buffer[(i << 2) + 1] & 0xff) << 8
			| (buffer[(i << 2) + 2] & 0xff) << 16
			| (buffer[(i << 2) + 3] & 0xff) << 24;
	}
}


void MD5::read(std::istream& is, std::string& s, int max)
{
	char sbuf[BLOCK_BYTES];
	is.read(sbuf, max);
	s.assign(sbuf, is.gcount());	
}