#pragma once
#include "hashManager.h"
#include<memory>
#include<array>
#include<string>
#include<sstream>
#include<atomic>
#include<nana/gui.hpp>
#include<nana/gui/widgets/group.hpp>
#include<nana/gui/place.hpp>
#include<nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include<nana/gui/drawing.hpp>
#include <nana/threads/pool.hpp>
#include<nana/gui/widgets/panel.hpp>
#include<nana/gui/widgets/combox.hpp>
#include<nana/gui/widgets/progress.hpp>
#include<nana/gui/widgets/button.hpp>

class integrityChecker : public nana::panel<false>, public hashManager
{
private:
	nana::place mPlace;
	nana::label mLabelFunc;
	nana::combox mHashFunc;
	nana::label mLabelInput1;
	nana::textbox mInput1;
	nana::label mLabelInput2;
	nana::textbox mInput2;
	nana::button mButton;
	nana::threads::pool mThreadPool;
	std::atomic<int> mRunningCnt;
	static bool checkHex(char);

public:
	integrityChecker(nana::window wd);
	virtual ~integrityChecker();
};