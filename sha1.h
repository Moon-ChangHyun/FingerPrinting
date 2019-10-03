#pragma once
#include "abstractHash.h"
#include <iostream>
#include <string>

class SHA1 : public abstractHash
{
public:
	SHA1();
	SHA1(const std::string&);
	void update(const std::string& s) override;
	void update(std::istream& is) override;
	std::string finalize() override;

private:
	static const uint32_t DIGEST_INTS = 160 / 32;
	static const uint32_t BLOCK_INTS = 512 / 32;
	static const uint32_t BLOCK_BYTES = 512 / 8;

	static void buffer_to_block(const std::string& buffer, uint32_t block[]);
	static void read(std::istream& is, std::string& s, int max);

	uint32_t digest[DIGEST_INTS];
	std::string buffer;
	uint64_t transforms;

	void reset();
	void transform(uint32_t block[]);
};