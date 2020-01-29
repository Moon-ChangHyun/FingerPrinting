#pragma once
#include<memory>
#include<nana/gui.hpp>
#include<nana/gui/widgets/tabbar.hpp>
#include<nana/gui/widgets/panel.hpp>
#include<nana/gui/widgets/combox.hpp>
#include<nana/gui/widgets/textbox.hpp>
#include<nana/gui/place.hpp>

class app {
private:
	static const char* pageFieldName[3];
	static const std::wstring pageName[3];
	nana::form mForm;
	std::vector<std::unique_ptr<nana::panel<false>>> mPages;
	nana::tabbar<size_t> mTab;
public:
	app();
};