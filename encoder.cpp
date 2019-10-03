#include "encoder.h"
#include "base64.h"
#include "base32.h"
#include <nana/gui/filebox.hpp>
#include <nana/gui/msgbox.hpp>

using namespace nana;
using namespace std;

const array<string, encoder::funcNum> encoder::sEncodingFuncNames = { "base64"s, "base32"s };
const array<unique_ptr<abstractEncoder>, encoder::funcNum> encoder::sEncoder= { make_unique<BASE64>(), make_unique<BASE32>() };

void encoder::execEncode(int idx, filesystem::path& inputFilePath, filesystem::path& outputFilePath) {
	sEncoder[idx]->encode(inputFilePath, outputFilePath);
}

void encoder::execEncode(int idx, filesystem::path&& inputFilePath, filesystem::path&& outputFilePath) {
	sEncoder[idx]->encode(inputFilePath, outputFilePath);
}

bool encoder::execDecode(int idx, filesystem::path& inputFilePath, filesystem::path& outputFilePath) {
	return sEncoder[idx]->decode(inputFilePath, outputFilePath);
}

bool encoder::execDecode(int idx, filesystem::path&& inputFilePath, filesystem::path&& outputFilePath) {
	return sEncoder[idx]->decode(inputFilePath, outputFilePath);
}

encoder::encoder(window wd) : panel<false>(wd), mPlace(*this), mLabelFunc(*this), mEncodingFunc(*this), mLabelInput(*this), mInputPath(*this), mLabelOutput(*this), mOutputPath(*this), mProgress(*this), mButtonEncode(*this), mButtonDecode(*this), mThreadPool(2)
{
	mPlace.div(R"(
		<weight=20>
		<vert
			<weight=20>
			<>
			<weight=30
				<F_LabelFunc weight=55>
				<weight=5>
				<F_EncodingFunc>
			>
			<weight=5>
			<weight=30
				<F_LabelInput weight=55>
				<weight=5>
				<F_Input>
			>
			<weight=5>
			<weight=30
				<F_LabelOutput weight=55>
				<weight=5>
				<F_Output>
			>
			<weight=10>
			<F_Progress weight=25>
			<weight=10>
			<weight=30
				<>
				<F_ButtonEncode>
				<weight=10>
				<F_ButtonDecode>
				<>
			>
			<>
			<weight=20>
		>
		<weight=20>
	)");

	mPlace["F_LabelFunc"] << mLabelFunc;
	mPlace["F_EncodingFunc"] << mEncodingFunc;
	mPlace["F_LabelInput"] << mLabelInput;
	mPlace["F_Input"] << mInputPath;
	mPlace["F_LabelOutput"] << mLabelOutput;
	mPlace["F_Output"] << mOutputPath;
	mPlace["F_Progress"] << mProgress;
	mPlace["F_ButtonEncode"] << mButtonEncode;
	mPlace["F_ButtonDecode"] << mButtonDecode;
	mPlace.collocate();

	mLabelFunc.caption(L"function"s);
	mLabelInput.caption(L"input"s);
	mLabelOutput.caption(L"output"s);
	mLabelFunc.text_align(align::center, align_v::center);
	mLabelInput.text_align(align::center, align_v::center);
	mLabelOutput.text_align(align::center, align_v::center);

	for (auto& encodingFuncName : sEncodingFuncNames)
		mEncodingFunc.push_back(encodingFuncName);
	mEncodingFunc.option(0);

	mInputPath.editable(false);
	mOutputPath.editable(false);
	mInputPath.enable_dropfiles(true);
	mInputPath.line_wrapped(true);
	mInputPath.multi_lines(false);
	mOutputPath.line_wrapped(true);
	mOutputPath.multi_lines(false);
	mInputPath.events().click([this](const arg_click& arg) {
		filebox fb(nullptr, true);
		fb.allow_multi_select(false);
		auto files = fb.show();
		if (!files.empty()) {
			auto path = files.front();
			mInputPath.caption(path.c_str());
		}
	});
	mInputPath.events().mouse_dropfiles([this](const arg_dropfiles& arg) {
		if (arg.files.size() == 1) {
			auto path = arg.files[0];
			mInputPath.caption(path.c_str());
		}
	});
	mOutputPath.events().click([this](const arg_click& arg) {
		filebox fb(nullptr, false);
		fb.allow_multi_select(false);
		auto files = fb.show();
		if (!files.empty()) {
			auto path = files.front();
			mOutputPath.caption(path.c_str());
		}
	});

	mProgress.unknown(false);

	mButtonEncode.caption(L"encode");
	mButtonDecode.caption(L"decode");
	mButtonEncode.events().click([this](const arg_click& arg){
		mThreadPool.push([&]() {
			progressRun = true;

			mThreadPool.push([&]() {
				while (progressRun) {
					mProgress.amount(sEncoder[mEncodingFunc.option()]->getAmount());
					mProgress.value(sEncoder[mEncodingFunc.option()]->getComputed());
				}
				mProgress.amount(0);
				mProgress.value(0);
			});

			mEncodingFunc.enabled(false);
			mInputPath.enabled(false);
			mOutputPath.enabled(false);
			mButtonEncode.enabled(false);
			mButtonDecode.enabled(false);
			execEncode(mEncodingFunc.option(), mInputPath.caption_wstring(), mOutputPath.caption_wstring());
			mEncodingFunc.enabled(true);
			mInputPath.enabled(true);
			mOutputPath.enabled(true);
			mButtonEncode.enabled(true);
			mButtonDecode.enabled(true);
			progressRun = false;
		});
	});
	mButtonDecode.events().click([this](const arg_click& arg) {
		mThreadPool.push([&]() {
			progressRun = true;

			mThreadPool.push([&]() {
				while (progressRun) {
					mProgress.amount(sEncoder[mEncodingFunc.option()]->getAmount());
					mProgress.value(sEncoder[mEncodingFunc.option()]->getComputed());
				}
				mProgress.amount(0);
				mProgress.value(0);
			});

			mEncodingFunc.enabled(false);
			mInputPath.enabled(false);
			mOutputPath.enabled(false);
			mButtonEncode.enabled(false);
			mButtonDecode.enabled(false);
			if(!execDecode(mEncodingFunc.option(), mInputPath.caption_wstring(), mOutputPath.caption_wstring()))
				(msgbox("error"s) << "Invalid input"s).show();
			mEncodingFunc.enabled(true);
			mInputPath.enabled(true);
			mOutputPath.enabled(true);
			mButtonEncode.enabled(true);
			mButtonDecode.enabled(true);
			progressRun = false;
		});
	});
}

encoder::~encoder()
{
	progressRun = false;
	mThreadPool.wait_for_finished();
}
