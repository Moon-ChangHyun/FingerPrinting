#pragma once
#include "abstractHash.h"
#include <string>

class SHA512 : public abstractHash
{
public:
	SHA512();
	SHA512(const std::string&);
	void update(const std::string&) override;
	void update(std::istream&) override;
	std::string finalize() override;

private:
	static const uint32_t DIGEST_LLS = (512 / 64);
	static const uint32_t BLOCK_LLS = (1024 / 64);
	static const uint32_t BLOCK_BYTES = (1024 / 8);
	static const uint32_t ROUNDS = 80;
	static const uint64_t K[];

	uint64_t digest[DIGEST_LLS];
	std::string buffer;
	uint64_t transformCnt;

	void reset();
	void transform(uint64_t[]);
	
	static void buffer_to_block(const std::string&, uint64_t[]);
	static void read(std::istream&, std::string&, int);
};