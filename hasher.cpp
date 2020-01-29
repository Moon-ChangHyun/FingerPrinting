#include "hasher.h"
#include <nana/gui/filebox.hpp>
#include<fstream>
#include<cmath>
#include<stdexcept>
#include<string>
#include<thread>
using namespace nana;
using namespace std;

hasher::hasher(window wd) : panel<false>(wd), mPlace(*this), mCheckGroup(*this), mOutputGroup(*this), mpDropbox(make_unique<panel<true>>(this->handle())), mDrawing(mpDropbox->handle()), mRunningCnt(0), mThreadPool(funcNum) {
	mLabels.reserve(funcNum);
	mTextBoxes.reserve(funcNum);
	mOutputs.reserve(funcNum); //active outputs
	mCheckBoxes.reserve(funcNum);

	for (int i = 0; i != funcNum; ++i) {
		mLabels.push_back(make_shared<label>(mOutputGroup.handle()));
		mTextBoxes.push_back(make_shared<textbox>(mOutputGroup.handle()));
		mLabels.back()->caption("<size=11 center>"s + sHashFuncNames[i] + "</>"s);
		mLabels.back()->text_align(align::left, align_v::center);
		mLabels.back()->format(true);
		mTextBoxes.back()->multi_lines(false);
	}

	mPlace.div(R"(
		<weight=20>
		<vert
			<weight=20>
			<F_checkBoxes weight=60>
			<F_dropBox>
			<weight=20>
		>
		<weight=20>
		<vert
			<weight=20>
			<F_output fit>
			<weight=20>
		>
		<weight=20>
	)");

	mPlace["F_dropBox"] << *mpDropbox;
	mPlace["F_output"] << mOutputGroup;
	mPlace["F_checkBoxes"] << mCheckGroup;
	mPlace.collocate();

	mOutputGroup.caption("Results");
	stringstream divStr;
	divStr << "<weight=10><vert";
	for (size_t i = 0; i != funcNum; ++i) {
		mOutputs.push_back({ mLabels[i], mTextBoxes[i] });
		divStr << "<weight=20<F_lb" << to_string(i) << " weight=50><F_tb" << to_string(i) << ">>";
	}
	divStr << "><weight=10>";
	mOutputGroup.div(divStr.str().c_str());
	for (size_t i = 0; i != funcNum; ++i) {
		mOutputGroup[("F_lb"s + to_string(i)).c_str()] << *mOutputs[i].first;
		mOutputGroup[("F_tb"s + to_string(i)).c_str()] << *mOutputs[i].second;
	}

	mCheckGroup.caption("Hash function");
	mCheckGroup.div("<weight=10><F_chks grid=[3,2]><weight=10>");
	for (auto& hashFuncName : sHashFuncNames) {
		mCheckBoxes.push_back(make_unique<checkbox>(this->mCheckGroup.handle()));
		mCheckBoxes.back()->caption(hashFuncName);
		mCheckBoxes.back()->transparent(true);
		mCheckBoxes.back()->check(true);
		mCheckBoxes.back()->events().checked([this](const arg_checkbox& arg) {
			auto selected = arg.widget->handle();
			auto checked = arg.widget->checked();
			for (size_t i = 0; i != funcNum; ++i) {
				if (selected == mCheckBoxes[i]->handle()) {
					mOutputs[i].second->enabled(checked);
				}
			}
		});
		mCheckGroup["F_chks"] << *mCheckBoxes.back();
	}

	mDrawing.draw([this](paint::graphics& gp) {
		rectangle rec = { 0, 5, this->mpDropbox->size().width, this->mpDropbox->size().height - 10 };
		gp.round_rectangle(rec, 5, 5, colors::blue, true, colors::light_gray);
		auto textSize = gp.text_extent_size(L"Drop the file or click to pick a file"s);
		gp.string({(int)((this->mpDropbox->size().width - textSize.width) >> 1), (int)(5 + ((this->mpDropbox->size().height - 10 - textSize.height) >> 1))}, L"Drop the file or click to pick a file"s, colors::blue);
	});

	mpDropbox->enable_dropfiles(true);
	mpDropbox->events().mouse_dropfiles([this](const arg_dropfiles& arg) {
		if (!(this->mOutputs.empty()) && arg.files.size() == 1) {
			auto path = arg.files[0];
			calculate(path);
		}
	});
	mpDropbox->events().click([this](const arg_click& arg) {
		filebox fb(nullptr, true);
		fb.allow_multi_select(false);
		auto files = fb.show();
		if (!(mOutputs.empty()) && !files.empty()) {
			auto path = files.front();
			calculate(path);
		}
	});
}

hasher::~hasher()
{
	mThreadPool.wait_for_finished();
}

void hasher::calculate(filesystem::path& path)
{
	if(mRunningCnt != 0)
		return;
	
	for (auto& chkBox : mCheckBoxes)
		chkBox->enabled(false);
	for (auto& textBox : mTextBoxes)
		textBox->caption(""s);

	for (size_t i = 0; i != mOutputs.size(); ++i) {
		if (!mOutputs[i].second->enabled())
			continue;
		auto& outputTB = *(mOutputs[i].second);
		auto pIfs = make_shared<ifstream>(path.c_str(), ios::binary);

		mThreadPool.push([&outputTB, i, pIfs, this]() {
			outputTB.caption("calculating..."s);
			outputTB.editable(false);
			++mRunningCnt;

			if (pIfs->is_open()) {
				try {
					outputTB.caption(getHash(i, *pIfs));
				}
				catch (exception& e) {
					msgbox mb(this->handle(), "error"s);
					(mb << e.what()).show();
					outputTB.caption(""s);
				}
				catch (...) {
					msgbox mb(this->handle(), "error"s);
					(mb << "Unknown error"s).show();
					outputTB.caption(""s);
				}
			}

			outputTB.editable(true);
			if (--mRunningCnt == 0) {
				for (auto& chkBox : mCheckBoxes)
					chkBox->enabled(true);
			}
		});
	}
}