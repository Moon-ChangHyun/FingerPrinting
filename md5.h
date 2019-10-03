#pragma once
#include "abstractHash.h"
#include <iostream>
#include <string>

class MD5 : public abstractHash
{
public:
	MD5();
	MD5(const std::string&);
	void update(const std::string&) override;
	void update(std::istream&) override;
	std::string finalize() override;

private:
	static const uint32_t DIGEST_INTS = 128 / 32;
	static const uint32_t BLOCK_INTS = 512 / 32;
	static const uint32_t BLOCK_BYTES = 512 / 8;

	static void buffer_to_block(const std::string&, uint32_t[]);
	static void read(std::istream& is, std::string&, int);

	uint32_t digest[DIGEST_INTS];
	std::string buffer;
	uint64_t transforms;

	void reset();
	void transform(uint32_t[]);

	// Constants for MD5Transform routine.
	static constexpr uint32_t S11 = 7;
	static constexpr uint32_t S12 = 12;
	static constexpr uint32_t S13 = 17;
	static constexpr uint32_t S14 = 22;
	static constexpr uint32_t S21 = 5;
	static constexpr uint32_t S22 = 9;
	static constexpr uint32_t S23 = 14;
	static constexpr uint32_t S24 = 20;
	static constexpr uint32_t S31 = 4;
	static constexpr uint32_t S32 = 11;
	static constexpr uint32_t S33 = 16;
	static constexpr uint32_t S34 = 23;
	static constexpr uint32_t S41 = 6;
	static constexpr uint32_t S42 = 10;
	static constexpr uint32_t S43 = 15;
	static constexpr uint32_t S44 = 21;

	// low level logic operations
	static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t rotate_left(uint32_t x, int n);
	static inline void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
	static inline void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
	static inline void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
	static inline void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
};