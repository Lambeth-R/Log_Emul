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
#include "../common/common.h"
#include "../common/Pipe.h"

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
	// Get info of selected process for emulate purposes
	Process GetProcess();
	std::list<msg>* GetData();
	// Singletone (Acually useless by now, was used in prev. iterations. But let it be)
	static cMain* GetInstance();
	cMain(cMain& other) = delete;
	void operator=(const cMain&) = delete;

private:
	// Meun Bar Funcs
	void OnLoad(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void ReinitMSG();
	// wxListBox -es synchronization func
	void MsgSync(int idPipe);
	// Inject exe, (warp acually, never use Inject itself)
	void InjectWarp(wxCommandEvent& evt);
	// Inject code
	void Inject();
	// Button Listen routine
	void ListenMode(wxCommandEvent& evt);
	// Button Emulate routine
	void EmulateMode(wxCommandEvent& evt);
	void wait(Pipe* Pipe, int size, int MsgLog);
	// Selected exe inject flag
	bool injected = false;
	// Gui handles
	wxMenu* file_menu = nullptr;
	wxMenuBar* menu_bar = nullptr;
	wxButton* m_btn_Inj = nullptr;
	wxButton* m_btn_sel_file = nullptr;
	wxListBox* m_Sel_file = nullptr;
	wxDirDialog* m_Inj_dial = nullptr;
	Process* m_pselected = nullptr;
	cMList* m_recieved_msgs = nullptr;
	wxListBox* m_log_msgs = nullptr;
	wxButton* m_log_btn = nullptr;
	wxButton* m_eml_btn = nullptr;
	wxBoxSizer* m_sizer = nullptr;
	wxBoxSizer* m_sizer_file = nullptr;
	wxBoxSizer* m_sizer_left = nullptr;
	wxBoxSizer* m_sizer_right = nullptr;
	// MsgSync async handle
	std::future<void> *m_msg_sync1 = nullptr, * m_msg_sync2 = nullptr, * m_msg_sync3 = nullptr, * m_msg_sync4 = nullptr;
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
