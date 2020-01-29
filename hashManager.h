#pragma once
#include "abstractHash.h"
#include<memory>
#include<array>

class hashManager
{
protected:
	static constexpr int funcNum = 6;
	static const std::array<std::string, funcNum> sHashFuncNames;
	static const std::array<std::unique_ptr<abstractHash>, funcNum> sHash;
	static const std::array<int, funcNum> sHashLen;
	static std::string getHash(int, std::istream&);
};

