#include "fLData.h"
#include "contsts.h"
#include "../common/Pipe.h"
#include "../common/common.h"
#include "cMain.h"

fLData::fLData(wxWindow* Parent, long ord) : wxFrame(Parent, wxID_ANY, "Readed info", { (Parent->GetPosition().x + mwind_size.x / 4), (Parent->GetPosition().y - 50) }, pwind_size)
{
	if (readed_info != nullptr)
		return;
	readed_info = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_READONLY | wxTE_MULTILINE | wxSUNKEN_BORDER);
	std::list<msg> lMessages = *cMain::GetInstance()->GetData();
	auto it = lMessages.begin();
	std::advance(it, ord);
	mess = new std::string((*it).message);
	length_count += 1000;
	Add_Text();
}

fLData::~fLData()
{
	delete(readed_info);
}

void fLData::Add_Text()
{
	DWORD fstop = 0;
	std::string to_screen = *mess;
	//for (auto it = to_screen.begin(); it != to_screen.end();it++) {
	//	if (*it == '.') 
	//		*it = '\n';
	//}
	DWORD div = 0;
	wxStreamToTextRedirector redirect(readed_info);
	std::cout << to_screen;
}