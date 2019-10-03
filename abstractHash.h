#pragma once
#include<string>

class abstractHash
{
public:
	virtual void update(const std::string&) = 0;
	virtual void update(std::istream&) = 0;
	virtual std::string finalize() = 0;
};

