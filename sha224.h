#pragma once
#include "abstractHash.h"
#include <string>

class SHA224 : public abstractHash
{
public:
	SHA224();
	SHA224(const std::string&);
	void update(const std::string&) override;
	void update(std::istream&) override;
	std::string finalize() override;

private:
	static const uint32_t DIGEST_INTS = (256 / 32);
	static const uint32_t BLOCK_INTS = (512 / 32);
	static const uint32_t BLOCK_BYTES = (512 / 8);
	static const uint32_t ROUNDS = 64;
	static const uint32_t K[];

	uint32_t digest[DIGEST_INTS];
	std::string buffer;
	uint64_t transformCnt;

	void reset();
	void transform(uint32_t[]);

	static void buffer_to_block(const std::string&, uint32_t[]);
	static void read(std::istream&, std::string&, int);
};