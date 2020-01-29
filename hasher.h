#pragma once
#include "hashManager.h"
#include<memory>
#include<array>
#include<string>
#include<sstream>
#include<atomic>
#include<nana/gui.hpp>
#include<nana/gui/widgets/group.hpp>
#include<nana/gui/widgets/panel.hpp>
#include<nana/gui/place.hpp>
#include<nana/gui/widgets/checkbox.hpp>
#include<nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include<nana/gui/drawing.hpp>
#include <nana/threads/pool.hpp>

class hasher : public nana::panel<false>, public hashManager
{
private:
	nana::place mPlace;
	nana::group mCheckGroup;
	std::vector<std::unique_ptr<nana::checkbox>> mCheckBoxes;
	std::vector<std::shared_ptr<nana::textbox>> mTextBoxes;
	std::vector<std::shared_ptr<nana::label>> mLabels;
	std::vector<std::pair<std::shared_ptr<nana::label>, std::shared_ptr<nana::textbox>>> mOutputs;
	nana::group mOutputGroup;
	std::unique_ptr<nana::panel<true>> mpDropbox;
	nana::drawing mDrawing;
	std::atomic<int> mRunningCnt;
	nana::threads::pool mThreadPool;

	void calculate(std::filesystem::path&);

public:
	hasher(nana::window wd);
	virtual ~hasher();
};