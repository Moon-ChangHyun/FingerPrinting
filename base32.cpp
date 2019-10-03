#include "base32.h"

const char BASE32::table[33] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

int BASE32::getIdx(char _5bitChar)
{
	if (0x32 <= _5bitChar && _5bitChar <= 0x37)
		return (_5bitChar - 0x30) + 24;
	if (0x41 <= _5bitChar && _5bitChar <= 0x5A)
		return (_5bitChar - 0x41);
	return -1;
}

void BASE32::encode(std::filesystem::path& inputFilePath, std::filesystem::path& outputFilePath)
{
	std::ifstream ifs(inputFilePath.c_str(), std::ios::in | std::ios::binary);
	std::ofstream ofs(outputFilePath.c_str(), std::ios::out | std::ios::binary);
	if (!(ifs && ofs))
		return;
	ifs.seekg(0, std::ios::end);
	auto fileLen = ifs.tellg();
	ifs.seekg(0, std::ios::beg);
	amount = ((long long)fileLen + unitBlockSize - 1) / unitBlockSize;
	computed = 0;
	std::string input, output;

	for (; computed < amount - 1; ++computed) {
		input.resize(unitBlockSize);
		ifs.read(input.data(), input.size());
		int blockNum = unitBlockSize / 5;
		output.clear();
		output.reserve(blockNum << 3);
		for (int blockCnt = 0; blockCnt < blockNum; ++blockCnt) {
			int idx = blockCnt * 5;
			output += table[(input[idx] & 0xF8) >> 3];
			output += table[((input[idx] & 0x07) << 2) | ((input[idx + 1] & 0xC0) >> 6)];
			output += table[(input[idx + 1] & 0x3E) >> 1];
			output += table[((input[idx + 1] & 0x01) << 4) | ((input[idx + 2] & 0xF0) >> 4)];
			output += table[((input[idx + 2] & 0x0F) << 1) | ((input[idx + 3] & 0x80) >> 7)];
			output += table[(input[idx + 3] & 0x7C) >> 2];
			output += table[((input[idx + 3] & 0x03) << 3) | ((input[idx + 4] & 0xE0) >> 5)];
			output += table[input[idx + 4] & 0x1F];
		}
		ofs.write(output.data(), output.size());
	}

	int remain = fileLen - ifs.tellg();
	input.resize(remain);
	ifs.read(input.data(), input.size());
	int blockNum = (remain + 4) / 5;
	int paddings = 5 * blockNum - remain;
	for (int i = paddings; ~--i;)
		input += char{ 0x00 };
	output.clear();
	output.reserve(blockNum << 3);
	for (int blockCnt = 0; blockCnt < blockNum; ++blockCnt) {
		int idx = blockCnt * 5;
		output += table[(input[idx] & 0xF8) >> 3];
		output += table[((input[idx] & 0x07) << 2) | ((input[idx + 1] & 0xC0) >> 6)];
		output += table[(input[idx + 1] & 0x3E) >> 1];
		output += table[((input[idx + 1] & 0x01) << 4) | ((input[idx + 2] & 0xF0) >> 4)];
		output += table[((input[idx + 2] & 0x0F) << 1) | ((input[idx + 3] & 0x80) >> 7)];
		output += table[(input[idx + 3] & 0x7C) >> 2];
		output += table[((input[idx + 3] & 0x03) << 3) | ((input[idx + 4] & 0xE0) >> 5)];
		output += table[input[idx + 4] & 0x1F];
	}
	switch (paddings) {
	case 4:
		paddings = 6;
		break;
	case 3:
		paddings = 4;
		break;
	case 2:
		paddings = 3;
	}
	for (int i = 0; i != paddings; ++i)
		output[output.size() - 1 - i] = '=';
	ofs.write(output.data(), output.size());
	amount = 0;
	computed = 0;
}

bool BASE32::decode(std::filesystem::path& inputFilePath, std::filesystem::path& outputFilePath)
{
	std::ifstream ifs(inputFilePath.c_str(), std::ios::binary);
	std::ofstream ofs(outputFilePath.c_str(), std::ios::binary);
	if (!(ifs && ofs))
		return false;
	ifs.seekg(0, std::ios::end);
	auto fileLen = ifs.tellg();
	if (fileLen % 8 != 0)
		return false;
	ifs.seekg(0, std::ios::beg);
	amount = ((long long)fileLen + unitBlockSize - 1) / unitBlockSize;
	computed = 0;
	std::string input, output;

	for (; computed < amount - 1; ++computed) {
		input.resize(unitBlockSize);
		ifs.read(input.data(), input.size());
		int blockNum = (input.size() >> 3);
		output.clear();
		output.reserve(blockNum * 5);
		for (int blockCnt = 0; blockCnt < blockNum; ++blockCnt) {
			int idx = blockCnt << 3;
			uint8_t _5bitVal[8];
			for (int i = 0; i != 8; ++i) {
				int val = getIdx(input[idx + i]);
				if (-1 == val) {
					return false;
				}
				_5bitVal[i] = val;
			}
			output += (_5bitVal[0] << 3) | (_5bitVal[1] >> 2);
			output += (char)(_5bitVal[1] << 6) | (_5bitVal[2] << 1) | (_5bitVal[3] >> 4);
			output += (char)(_5bitVal[3] << 4) | (_5bitVal[4] >> 1);
			output += (char)(_5bitVal[4] << 7) | (_5bitVal[5] << 2) | (_5bitVal[6] >> 3);
			output += (char)(_5bitVal[6] << 5) | (_5bitVal[7]);
		}
		ofs.write(output.data(), output.size());
	}

	int remain = fileLen - ifs.tellg();
	input.resize(remain);
	ifs.read(input.data(), input.size());
	int blockNum = (input.size() >> 3);
	auto paddingIdx = input.find_last_not_of('=');
	int paddings = input.size() - (paddingIdx + 1);
	if (paddings != 6 && paddings != 4 && paddings != 3 && paddings != 1 && paddings != 0)
		return false;
	output.clear();
	output.reserve(blockNum * 5);
	for (int blockCnt = 0; blockCnt < blockNum; ++blockCnt) {
		int idx = blockCnt << 3;
		uint8_t _5bitVal[8];
		for (int i = 0; i != 8; ++i) {
			int val = getIdx(input[idx + i]);
			if (-1 == val) {
				if (blockCnt == blockNum - 1) {
					if (i < 8 - paddings)
						return false;
				}
				else
					return false;
			}
			_5bitVal[i] = val;
		}
		output += (_5bitVal[0] << 3) | (_5bitVal[1] >> 2);
		output += (char)(_5bitVal[1] << 6) | (_5bitVal[2] << 1) | (_5bitVal[3] >> 4);
		output += (char)(_5bitVal[3] << 4) | (_5bitVal[4] >> 1);
		output += (char)(_5bitVal[4] << 7) | (_5bitVal[5] << 2) | (_5bitVal[6] >> 3);
		output += (char)(_5bitVal[6] << 5) | (_5bitVal[7]);
	}
	switch (paddings) {
	case 6:
		paddings = 4;
		break;
	case 4:
		paddings = 3;
		break;
	case 3:
		paddings = 2;
	}
	output.erase(output.size() - paddings);
	ofs.write(output.data(), output.size());

	amount = 0;
	computed = 0;
	return true;
}

int BASE32::getAmount() {
	return amount;
}

int BASE32::getComputed() {
	return computed;
}