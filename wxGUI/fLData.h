#pragma once
#ifndef F_L_DATA_H
#define F_L_DATA_H

#include "wx/listctrl.h"
#include "wx/wx.h"

class fLData : public wxFrame
{
public:
	fLData(wxWindow* Parent, long ord);
	~fLData();
private:
	void Add_Text();
	wxTextCtrl* readed_info = nullptr;
	wxBoxSizer* m_sizer = nullptr;
	std::string* mess = nullptr;
	DWORD length_count = 0;
};

#endif // !1