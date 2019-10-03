#pragma once
#include<string>
#include<filesystem>

class abstractEncoder
{
public:
	virtual void encode(std::filesystem::path&, std::filesystem::path&) = 0;
	virtual bool decode(std::filesystem::path&, std::filesystem::path&) = 0;
	virtual int getAmount() = 0;
	virtual int getComputed() = 0;
};

