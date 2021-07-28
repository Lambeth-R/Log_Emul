#pragma once
#ifndef _F_PROCESS_H_
#define _F_PROCESS_H_

#include "wx/event.h"
#include "wx/listctrl.h"
#include "wx/wx.h"
#include "cProcesses.h"

class MyListCtrl : public wxListCtrl
{
public:
	MyListCtrl(wxWindow* Parent, cProcesses* cP);
	~MyListCtrl(); 
	void OnColClick(wxListEvent& event);
	void OnFocused(wxListEvent& event);
	Process* getSelected();
	void Sort_by(int col);
	void OnSearch(wxString str);
	wxDECLARE_NO_COPY_CLASS(MyListCtrl);
	wxDECLARE_EVENT_TABLE();
	Process m_selected;

private:
	int* m_item_count;
	cProcesses* m_cP;
	struct Sorted {
		bool _st = false;
		bool _nd = false;
	};
	Sorted* m_sorted;
};

class fProcess : public wxFrame
{
public:
	fProcess(wxWindow* Parent, cProcesses* cP);
	~fProcess();
	void OnButtonClick(wxCommandEvent& evt);
	void OnSearch(wxCommandEvent& evt);
	
private:
	wxWindow* m_empty = nullptr;
	wxButton* m_Sel_btn = nullptr;
	MyListCtrl* m_list_ctrl = nullptr;
	wxBoxSizer* m_sizer = nullptr;
	wxTextCtrl* m_tsearch = nullptr;
public:
	wxDECLARE_NO_COPY_CLASS(fProcess);
	wxDECLARE_EVENT_TABLE();
};



enum {	LIST_CTRL = 1000,
		PSELECTOR,
		PSEARCH
};

#endif 