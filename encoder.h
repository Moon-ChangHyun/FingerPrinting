#pragma once
#include "abstractEncoder.h"
#include<array>
#include<memory>
#include<fstream>
#include<nana/gui/widgets/panel.hpp>
#include<nana/gui/place.hpp>
#include<nana/gui/widgets/combox.hpp>
#include<nana/gui/widgets/textbox.hpp>
#include<nana/gui/widgets/progress.hpp>
#include<nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/threads/pool.hpp>

class encoder : public nana::panel<false>
{
private:
	static constexpr int funcNum = 2;
	static const std::array<std::string, funcNum> sEncodingFuncNames;
	static const std::array<std::unique_ptr<abstractEncoder>, funcNum> sEncoder;
	static void execEncode(int, std::filesystem::path&, std::filesystem::path&);
	static void execEncode(int, std::filesystem::path&&, std::filesystem::path&&);
	static bool execDecode(int, std::filesystem::path&, std::filesystem::path&);
	static bool execDecode(int, std::filesystem::path&&, std::filesystem::path&&);
	nana::place mPlace;
	nana::label mLabelFunc;
	nana::combox mEncodingFunc;
	nana::label mLabelInput;
	nana::textbox mInputPath;
	nana::label mLabelOutput;
	nana::textbox mOutputPath;
	nana::progress mProgress;
	nana::button mButtonEncode;
	nana::button mButtonDecode;
	nana::threads::pool mThreadPool;
	bool progressRun;

public:
	encoder(nana::window);
	virtual ~encoder();
};