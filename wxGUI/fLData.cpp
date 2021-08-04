#include "fLData.h"
#include "contsts.h"
#include "cPipe.h"

fLData::fLData(wxWindow* Parent, long ord) : wxFrame(Parent, wxID_ANY, "Readed info", { (Parent->GetPosition().x + mwind_size.x / 4), (Parent->GetPosition().y - 50) }, pwind_size)
{
	if (readed_info != nullptr)
		return;
	readed_info = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_READONLY | wxTE_MULTILINE | wxSUNKEN_BORDER);
	cPipe* pipe = cPipe::GetInstance();
	auto it = pipe->ListenedMessages->begin();
	while ((*it).order != (DWORD)ord || it == pipe->ListenedMessages->end()) {
		it++;
	}
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
	DWORD div = 0;
	std::ostream stream(readed_info);
	stream << to_screen;
	stream.flush();
}