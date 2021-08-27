#pragma once
#ifndef _CMAIN_H_
#define _CMAIN_H_

#include <future>
#include <windows.h>

#include "wx/display.h"
#include "wx/dirdlg.h"
#include "wx/event.h"
#include "wx/listctrl.h"
#include "wx/wx.h"

#include "contsts.h"
#include "cProcesses.h"
#include "sysfunk.h"
#include "../common/common.h"

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
	// Process Selection window
	void SelectProcess(wxCommandEvent& evt);
	// Set selected proc parameters to wxListBox
	void SetSelected(Process* p);	
	// Inject outside class for emulate purposes
	void InjectOutside();
	// Get info of selected process for emulate purposes
	Process GetProcess();
	// Singletone
	static cMain* GetInstance();
	cMain(cMain& other) = delete;
	void operator=(const cMain&) = delete;

private:
	// Meun Bar Funcs
	void OnLoad(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	// wxListBox -es synchronization func
	void MsgSync();
	// Inject exe, (warp acually, never use Inject itself)
	void InjectWithin(wxCommandEvent& evt);
	// Inject code
	void Inject();
	// Button Listen routine
	void ListenMode(wxCommandEvent& evt);
	// Button Emulate routine
	void EmulateMode(wxCommandEvent& evt);
	// Selected exe inject flag
	bool injected = false;
	// Gui handles
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
	// MsgSync async handle
	std::future<void>* m_msg_sync = nullptr;
	std::list<msg>* LoadSaveData = nullptr;
	//std::list<sysfunk> funks_data;
	// MsgSync exit code
	int sync_exit_code;
	wxDECLARE_EVENT_TABLE();
protected:
	// Singletone
	static cMain* cmain;
	cMain();
	~cMain();
};



#endif 
