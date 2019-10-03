#pragma once
#include "abstractEncoder.h"
#include<fstream>

class BASE64 : public abstractEncoder
{
public:
	BASE64() = default;
	void encode(std::filesystem::path& inputFilePath, std::filesystem::path& outputFilePath) override;
	bool decode(std::filesystem::path& inputFilePath, std::filesystem::path& outputFilePath) override;
	int getAmount() override;
	int getComputed() override;

protected:
	int amount;
	int computed;

private:
	static constexpr int unitBlockSize = 1728; // must be multiple of 12
	static const char table[65];
	static int getIdx(char);
};