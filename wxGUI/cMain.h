#pragma once
#ifndef _CMAIN_H_
#define _CMAIN_H_

#include <future>
#include <windows.h>


#include "wx/display.h"
#include "wx/dirdlg.h"
#include "wx/event.h"
#include <wx/listctrl.h>
#include "wx/wx.h"

#include "contsts.h"
#include "cProcesses.h"
#include "cPipe.h"
#include "sysfunk.h"

class cMList : public wxListCtrl
{
public:
	cMList(wxWindow* parent,
		const wxWindowID id,
		const wxPoint& pos,
		const wxSize& size,
		long style)
		: wxListCtrl(parent, id, pos, size, style)
	{
		m_updated = -1;

	}
	void OnSelected(wxListEvent& event);
private:
	long m_updated;
	wxDECLARE_NO_COPY_CLASS(cMList);
	wxDECLARE_EVENT_TABLE();
};

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();
	void SelectProcess(wxCommandEvent& evt);
	void SetSelected(Process* p);	
private:
	void MsgSync();
	void Inject(wxCommandEvent& evt);
	void Listen_mode(wxCommandEvent& evt);
	void Emulate_mode(wxCommandEvent& evt);
	void OnListSelected(wxCommandEvent& evt);
	wxButton* m_btn_Inj = nullptr;
	wxButton* m_btn_sel_file = nullptr;
	wxListBox* m_Sel_file = nullptr;
	wxWindow* m_empty = nullptr;
	wxDirDialog* m_Inj_dial = nullptr;
	Process* m_pselected = nullptr;
	cMList* m_recieved_msgs = nullptr;
	wxListBox* m_log_msgs = nullptr;
	wxBoxSizer* m_sizer = nullptr;
	wxButton* m_log_btn = nullptr;
	wxButton* m_eml_btn = nullptr;
	std::future<void>* m_msg_sync = nullptr;	
	std::list<sysfunk> funks_data;
	int sync_exit_code;
	wxDECLARE_EVENT_TABLE();
};



#endif 
