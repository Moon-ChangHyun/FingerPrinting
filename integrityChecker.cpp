#include "integrityChecker.h"
#include <nana/gui/filebox.hpp>
#include<fstream>
#include<cmath>
#include<stdexcept>
#include<string>
#include<thread>
#include<regex>
using namespace nana;
using namespace std;

integrityChecker::integrityChecker(window wd) : panel<false>(wd), mPlace(*this), mLabelFunc(*this), mHashFunc(*this), mLabelInput1(*this), mInput1(*this), mLabelInput2(*this), mInput2(*this), mButton(*this), mThreadPool(2) {

	mPlace.div(R"(
		<weight=20>
		<vert
			<weight=20>
			<>
			<weight=30
				<F_LabelFunc weight=55>
				<weight=5>
				<F_HashFunc>
			>
			<weight=5>
			<weight=30
				<F_LabelInput1 weight=55>
				<weight=5>
				<F_Input1>
			>
			<weight=5>
			<weight=30
				<F_LabelInput2 weight=55>
				<weight=5>
				<F_Input2>
			>
			<weight=10>
			<weight=30
				<>
				<F_Button>
				<>
			>
			<>
			<weight=20>
		>
		<weight=20>
	)");

	mPlace["F_LabelFunc"] << mLabelFunc;
	mPlace["F_HashFunc"] << mHashFunc;
	mPlace["F_LabelInput1"] << mLabelInput1;
	mPlace["F_Input1"] << mInput1;
	mPlace["F_LabelInput2"] << mLabelInput2;
	mPlace["F_Input2"] << mInput2;
	mPlace["F_Button"] << mButton;
	mPlace.collocate();

	mLabelFunc.caption(L"function"s);
	mLabelInput1.caption(L"input 1"s);
	mLabelInput2.caption(L"input 2"s);
	mLabelFunc.text_align(align::center, align_v::center);
	mLabelInput1.text_align(align::center, align_v::center);
	mLabelInput2.text_align(align::center, align_v::center);

	for (auto& HashFuncName : sHashFuncNames)
		mHashFunc.push_back(HashFuncName);
	mHashFunc.option(0);

	mHashFunc.events().selected([this](const arg_combox& arg) {
		mInput1.caption("");
		mInput2.caption("");
	});

	mInput1.editable(true);
	mInput1.enable_dropfiles(true);
	mInput1.multi_lines(false);
	mInput1.events().mouse_dropfiles([this](const arg_dropfiles& arg) {
		if (arg.files.size() == 1) {
			auto path = arg.files[0];
			mInput1.enabled(false);
			mHashFunc.enabled(false);
			mThreadPool.push([this, path]() {
				++mRunningCnt;
				mInput1.caption("calculating...");
				auto ifs = ifstream(path.c_str(), ios::binary);
				mInput1.caption(getHash(mHashFunc.option(), ifs));
				mInput1.enabled(true);
				if (--mRunningCnt == 0)
					mHashFunc.enabled(true);
			});
		}
	});

	mInput2.editable(true);
	mInput2.enable_dropfiles(true);
	mInput2.multi_lines(false);
	mInput2.events().mouse_dropfiles([this](const arg_dropfiles& arg) {
		if (arg.files.size() == 1) {
			auto path = arg.files[0];
			mInput2.enabled(false);
			mHashFunc.enabled(false);
			mThreadPool.push([this, path]() {
				++mRunningCnt;
				mInput2.caption("calculating...");
				auto ifs = ifstream(path.c_str(), ios::binary);
				mInput2.caption(getHash(mHashFunc.option(), ifs));
				mInput2.enabled(true);
				if (--mRunningCnt == 0)
					mHashFunc.enabled(true);
			});
		}
	});

	mButton.caption(L"compare"s);
	mButton.events().click([this](const arg_click& arg) {
		if (mRunningCnt == 0) {
			auto len = sHashLen[mHashFunc.option()];
			string res1 = mInput1.caption();
			string res2 = mInput2.caption();
			if (res1.length() != len || res2.length() != len)
				(msgbox(*this, "result") << "invalid value").show();
			else if (!all_of(res1.begin(), res1.end(), checkHex) || !all_of(res2.begin(), res2.end(), checkHex))
				(msgbox(*this, "result") << "invalid value").show();
			else if (res1 == res2)
				(msgbox(*this, "result") << "match").show();
			else
				(msgbox(*this, "result") << "mismatch").show();
		}
	});
}

integrityChecker::~integrityChecker()
{
	mThreadPool.wait_for_finished();
}

bool integrityChecker::checkHex(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}