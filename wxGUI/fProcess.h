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
	MyListCtrl(wxWindow* Parent);
	~MyListCtrl(); 
	// Colunm click routine (for sort purposes)
	void OnColClick(wxListEvent& event);
	// Process selection routine
	void OnFocused(wxListEvent& event);
	// For transfer process info
	Process* GetSelected();
	// Sort routine
	void Sort_by(int col);
	// Search Routine
	void OnSearch(wxString str);
	wxDECLARE_NO_COPY_CLASS(MyListCtrl);
	wxDECLARE_EVENT_TABLE();
	// Selected proc
	Process m_selected;
private:
	// List showed item (for search routine)
	int* m_item_count;
	// All processes data
	cProcesses* m_cP;
	// First or second column sort data
	struct Sorted {
		bool _st = false;
		bool _nd = false;
	};
	Sorted* m_sorted;
};

class fProcess : public wxFrame
{
public:
	fProcess(wxWindow* Parent);
	~fProcess();
	// Select button routine
	void OnButtonClick(wxCommandEvent& evt);
	// Search box routine
	void OnSearch(wxCommandEvent& evt);
	
private:
	// Gui handles
	wxWindow* m_empty = nullptr;
	wxButton* m_Sel_btn = nullptr;
	MyListCtrl* m_list_ctrl = nullptr;
	wxBoxSizer* m_sizer = nullptr;
	wxTextCtrl* m_tsearch = nullptr;
public:
	wxDECLARE_NO_COPY_CLASS(fProcess);
	wxDECLARE_EVENT_TABLE();
};


// Windows id`s enum
enum {	LIST_CTRL = 1000,
		PSELECTOR,
		PSEARCH
};

#endif 