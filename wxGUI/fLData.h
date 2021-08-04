#pragma once
#ifndef F_L_DATA_H
#define F_L_DATA_H

#include "wx/listctrl.h"
#include "wx/wx.h"

//Class for showing linstened strings. TODO (sad work cant hande huge strings)
class fLData : public wxFrame
{
public:
	fLData(wxWindow* Parent, long ord);
	~fLData();
private:
	// Add text to window
	void Add_Text();
	wxTextCtrl* readed_info = nullptr;
	wxBoxSizer* m_sizer = nullptr;
	//Text data
	std::string* mess = nullptr;
	DWORD length_count = 0;
};

#endif // !1