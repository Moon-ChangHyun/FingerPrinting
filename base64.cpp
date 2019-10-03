#include "base64.h"

const char BASE64::table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int BASE64::getIdx(char _6bitChar)
{
	if (_6bitChar == '+')
		return 62;
	if (_6bitChar == '/')
		return 63;
	if (0x30 <= _6bitChar && _6bitChar <= 0x39)
		return (_6bitChar - 0x30) + 52;
	if (0x41 <= _6bitChar && _6bitChar <= 0x5A)
		return (_6bitChar - 0x41);
	if (0x61 <= _6bitChar && _6bitChar <= 0x7A)
		return (_6bitChar - 0x61) + 26;
	return -1;
}

void BASE64::encode(std::filesystem::path& inputFilePath, std::filesystem::path& outputFilePath)
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
		int blockNum = unitBlockSize / 3;
		output.clear();
		output.reserve(blockNum << 2);
		for (int blockCnt = 0; blockCnt < blockNum; ++blockCnt) {
			size_t idx = blockCnt * 3;
			output += table[(input[idx] & 0xFC) >> 2];
			output += table[((input[idx] & 0x03) << 4) | ((input[idx + 1] & 0xF0) >> 4)];
			output += table[((input[idx + 1] & 0x0F) << 2) | ((input[idx + 2] & 0xC0) >> 6)];
			output += table[input[idx + 2] & 0x3F];
		}
		ofs.write(output.data(), output.size());
	}

	int remain = fileLen - ifs.tellg();
	input.resize(remain);
	ifs.read(input.data(), input.size());
	int blockNum = (remain + 2) / 3;
	int paddings = 3 * blockNum - remain;
	for (int i = paddings; ~--i;)
		input += char{ 0x00 };
	output.clear();
	output.reserve(blockNum << 2);
	for (int blockCnt = 0; blockCnt < blockNum; ++blockCnt) {
		int idx = blockCnt * 3;
		output += table[(input[idx] & 0xFC) >> 2];
		output += table[((input[idx] & 0x03) << 4) | ((input[idx + 1] & 0xF0) >> 4)];
		output += table[((input[idx + 1] & 0x0F) << 2) | ((input[idx + 2] & 0xC0) >> 6)];
		output += table[input[idx + 2] & 0x3F];
	}
	for (int i = 0; i != paddings; ++i)
		output[output.size() - 1 - i] = '=';
	ofs.write(output.data(), output.size());
	amount = 0;
	computed = 0;
}

bool BASE64::decode(std::filesystem::path& inputFilePath, std::filesystem::path& outputFilePath)
{
	std::ifstream ifs(inputFilePath.c_str(), std::ios::binary);
	std::ofstream ofs(outputFilePath.c_str(), std::ios::binary);
	if (!(ifs && ofs))
		return false;
	ifs.seekg(0, std::ios::end);
	auto fileLen = ifs.tellg();
	if (fileLen % 4 != 0)
		return false;
	ifs.seekg(0, std::ios::beg);
	amount = ((long long)fileLen + unitBlockSize - 1) / unitBlockSize;
	computed = 0;
	std::string input, output;

	for (; computed < amount - 1; ++computed) {
		input.resize(unitBlockSize);
		ifs.read(input.data(), input.size());
		int blockNum = (input.size() >> 2);
		output.clear();
		output.reserve(blockNum * 3);
		for (int blockCnt = 0; blockCnt < blockNum; ++blockCnt) {
			int idx = blockCnt << 2;
			uint8_t _6bitVal[4];
			for (int i = 0; i != 4; ++i) {
				int val = getIdx(input[idx + i]);
				if (-1 == val) {
					return false;
				}
				_6bitVal[i] = val;
			}
			output += (_6bitVal[0] << 2) | ((_6bitVal[1] & 0x30) >> 4);
			output += ((_6bitVal[1] & 0x0F) << 4) | ((_6bitVal[2] & 0x3C) >> 2);
			output += ((_6bitVal[2] & 0x03) << 6) | _6bitVal[3];
		}
		ofs.write(output.data(), output.size());
	}

	int remain = fileLen - ifs.tellg();
	input.resize(remain);
	ifs.read(input.data(), input.size());
	int blockNum = (input.size() >> 2);
	auto paddingIdx = input.find_last_not_of('=');
	int paddings = input.size() - (paddingIdx + 1);
	if (paddings > 2)
		return false;
	output.clear();
	output.reserve(blockNum * 3);
	for (int blockCnt = 0; blockCnt < blockNum; ++blockCnt) {
		int idx = blockCnt << 2;
		uint8_t _6bitVal[4];
		for (int i = 0; i != 4; ++i) {
			int val = getIdx(input[idx + i]);
			if (-1 == val) {
				if (blockCnt == blockNum - 1) {
					if (i < 4 - paddings)
						return false;
				}
				else
					return false;
			}
			_6bitVal[i] = val;
		}
		output += (_6bitVal[0] << 2) | ((_6bitVal[1] & 0x30) >> 4);
		output += ((_6bitVal[1] & 0x0F) << 4) | ((_6bitVal[2] & 0x3C) >> 2);
		output += ((_6bitVal[2] & 0x03) << 6) | _6bitVal[3];
	}
	output.erase(output.size() - paddings);
	ofs.write(output.data(), output.size());
	
	amount = 0;
	computed = 0;
	return true;
}

int BASE64::getAmount() {
	return amount;
}

int BASE64::getComputed() {
	return computed;
}