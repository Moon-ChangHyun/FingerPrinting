#include "app.h"
#include "hasher.h"
#include "encoder.h"
#include <algorithm>

using namespace std;
using namespace nana;

const char* app::pageFieldName[2] = { "F_Hash", "F_Encoding" };
const wstring app::pageName[2] = {L"해쉬"s, L"인코딩&디코딩"s};

app::app() : mForm{ API::make_center(500, 300) }, mTab{ mForm } {
	mForm.caption("FingerPrinting");
	mForm.div(R"(
		vert
		<F_tabbar weight=20>
		<switchable 
			<F_Hash>
			<F_Encoding>
		>
	)");
	/*mForm.events().resized([this](const arg_resized& arg) {
		this->mForm.size({ max(arg.width, 300U), max(arg.height, 200U) });
	});*/

	mPages.reserve(2);
	mPages.push_back(make_unique<hasher>(mForm));
	mPages.push_back(make_unique<encoder>(mForm));

	for (size_t i = 0; i != 2; ++i) {
		mTab.append(pageName[i], *mPages[i], i);
		mForm[pageFieldName[i]] << *mPages[i];
	}
	
	mForm["F_tabbar"] << mTab;
	mForm.collocate();

	mTab.events().activated([this](const arg_tabbar<size_t>& e) {
		this->mForm.get_place().field_display(pageFieldName[e.value], true);
		this->mForm.collocate();
	});
	mTab.activated(0);
	
	mForm.show();
	exec();
}