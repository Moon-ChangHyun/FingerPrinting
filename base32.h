#pragma once
#include "abstractEncoder.h"
#include<fstream>

class BASE32 : public abstractEncoder
{
public:
	BASE32() = default;
	void encode(std::filesystem::path& inputFilePath, std::filesystem::path& outputFilePath) override;
	bool decode(std::filesystem::path& inputFilePath, std::filesystem::path& outputFilePath) override;
	int getAmount() override;
	int getComputed() override;

protected:
	int amount;
	int computed;

private:
	static constexpr int unitBlockSize = 1600; // must be multiple of 40
	static const char table[33];
	static int getIdx(char);
};