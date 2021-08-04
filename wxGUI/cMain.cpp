#include "contsts.h"
#include "cMain.h"
#include "cProcesses.h"
#include "fProcess.h"
#include "fLData.h"


wxBEGIN_EVENT_TABLE(cMain, wxFrame)
    EVT_BUTTON(FOPENID, cMain::SelectProcess)
	EVT_BUTTON(INJECTID, cMain::InjectWithin)
	EVT_BUTTON(LISTEN, cMain::ListenMode)
	EVT_BUTTON(EMULATE, cMain::EmulateMode)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(cMList, wxListCtrl)
	EVT_LIST_ITEM_SELECTED(MSG_LIST, cMList::OnSelected)
wxEND_EVENT_TABLE()

void cMain::SelectProcess(wxCommandEvent& evt)
{
	cProcesses* c = cProcesses::GetInstance();
	fProcess* fpFrame = new fProcess(this);
	fpFrame->Show(true);
}

void cMain::SetSelected(Process* p)
{
	if (!m_pselected)
		m_pselected = new Process();
	m_pselected->name = p->name;
	m_pselected->path = p->path;
	m_pselected->pid = p->pid;
	m_Sel_file->Clear();
	wxString res;
	res.Printf("Pid: %d\t Name: %s", m_pselected->pid, m_pselected->name);
	m_Sel_file->Insert(res, 0);
	m_Sel_file->Refresh();
	injected = false;
}

cMain::cMain() : wxFrame(nullptr, wxID_ANY, tMain_wind.c_str(), wxPoint((wxDisplay().GetGeometry().GetWidth() - mwind_size.x ) / 2, (wxDisplay().GetGeometry().GetHeight() - mwind_size.x) / 2), wxSize(mwind_size.x,mwind_size.y))
{
	m_empty = new wxWindow(this, wxID_ANY, wxPoint(0, 0), mwind_size);
	m_Inj_dial = new wxDirDialog(m_empty, tDir_Inj.c_str(), "",  wxDD_DIR_MUST_EXIST, wxDefaultPosition, wxSize(400, 25));
    m_Sel_file = new wxListBox(m_empty, wxID_ANY, wxPoint(20, 20), wxSize(500, 25));
    m_btn_sel_file = new wxButton(m_empty, FOPENID, "<-", wxPoint(518, 19), wxSize(25, 27));
    m_btn_Inj = new wxButton(m_empty, INJECTID, tBtn_Inj.c_str(), wxPoint(600, 12), wxSize(150, 40));
	m_recieved_msgs = new cMList(m_empty, MSG_LIST, wxPoint(20, 80), wxSize(540, 450), wxLC_LIST);
	m_log_btn = new wxButton(m_empty, LISTEN, tBtn_Log.c_str(), wxPoint(600, 60), wxSize(150, 40));
	m_eml_btn = new wxButton(m_empty, EMULATE, tBtn_Eml.c_str(), wxPoint(600, 110), wxSize(150, 40));
	m_log_msgs = new wxListBox(m_empty, wxID_ANY, wxPoint(600, 300), wxSize(150, 200));
	sync_exit_code = -1;
	//m_sizer = new wxBoxSizer(wxVERTICAL);
	//m_sizer->Add(m_empty, wxSizerFlags(1).Expand().Border());
	//m_sizer->Add(m_Inj_dial, wxSizerFlags(2).Expand().Border());
	//m_sizer->Add(m_Sel_file, wxSizerFlags(3).Expand().Border());
	//m_sizer->Add(m_btn_sel_file, wxSizerFlags(4).Expand().Border());
	//m_sizer->Add(m_btn_Inj, wxSizerFlags(5).Expand().Border());
}

cMain::~cMain()
{    
	sync_exit_code = 0;
	delete (m_btn_Inj);
	delete (m_btn_sel_file);
	delete (m_Sel_file);
    if(m_Inj_dial) delete (m_Inj_dial);
	delete (m_empty);
}

void cMain::MsgSync()
{
	cPipe* pipe = cPipe::GetInstance();
	while (sync_exit_code < 0)
	{
		Sleep(100);
		if (pipe->SetLock()) {
			for (auto i = pipe->LogMessages->begin(); i != pipe->LogMessages->end(); i++)
			{
				if (!i._Ptr->_Myval.displayed) {
					m_log_msgs->Insert(i._Ptr->_Myval.message, i._Ptr->_Myval.order);
					i._Ptr->_Myval.displayed = true;
					m_log_msgs->Refresh();
				}
			}
			for (auto i = pipe->ListenedMessages->begin(); i != pipe->ListenedMessages->end(); i++)
			{
				if (!i._Ptr->_Myval.displayed) {
					m_recieved_msgs->InsertItem(i._Ptr->_Myval.order, i._Ptr->_Myval.message);
					i._Ptr->_Myval.displayed = true;
					m_recieved_msgs->Refresh();
				}
			}
			pipe->UnLock();
		}

	}
}

void cMain::InjectOutside()
{
	if (injected) {
		wxMessageBox("Process already injected", "Error", wxOK);
		return;
	}
	Inject();
}

Process cMain::GetProcess()
{
	return *m_pselected;
}

void cMain::InjectWithin(wxCommandEvent& evt)
{
	if (injected) {
		wxMessageBox("Process already injected", "Error", wxOK);
		return;
	}
	Inject();
	if (m_msg_sync == nullptr)
		m_msg_sync = new std::future<void>(std::async(std::launch::async, &cMain::MsgSync, this));
}

void cMain::Inject()
{
	DWORD dwMemSize;
	HANDLE hProcess;
	LPVOID lpRemoteMemory, lpLoadLibrary;
	char szPath[MAX_PATH];
	GetFullPathNameA(t_myDll.c_str(), MAX_PATH, szPath, NULL);
	if (_access(szPath, 0) != 0 || m_pselected == NULL)
		return;	
	dwMemSize = strlen(szPath) + 1;
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, TRUE, m_pselected->pid);
	if (hProcess) {
		lpRemoteMemory = VirtualAllocEx(hProcess, NULL, dwMemSize, MEM_COMMIT, PAGE_READWRITE);
		if (lpRemoteMemory != 0) {
			WriteProcessMemory(hProcess, lpRemoteMemory, (LPCVOID)szPath, dwMemSize, NULL);
			//Shit warning wanna get it off
			HMODULE kerHandle = GetModuleHandleA("Kernel32.dll");
			if (!kerHandle)
				return;
			lpLoadLibrary = GetProcAddress(kerHandle, "LoadLibraryA");
			if (CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)lpLoadLibrary, lpRemoteMemory, NULL, NULL))
			{
				Sleep(10);
				VirtualFreeEx(hProcess, (LPVOID)lpRemoteMemory, 0, MEM_RELEASE);
				CloseHandle(hProcess);
				wxMessageBox("Injection was successful", "Result", wxOK);
				injected = true;
			}
		}
	}
	else {
		DWORD errID = GetLastError();
		wxString err;
		err.Printf("Cannot inject executable error: %d", errID);
		wxMessageBox(err, "Result", wxOK);		
	}	
}

void cMain::ListenMode(wxCommandEvent& evt)
{
	if (!injected)
		return;
	cPipe* pipe = cPipe::GetInstance();
	if (m_pselected == NULL)
		return;
	pipe->ClearLog();
	pipe->SetAction(COMMANDS::Cloggin);
}

void cMain::EmulateMode(wxCommandEvent& evt)
{
	if (!injected)
		return;
	cPipe* pipe = cPipe::GetInstance();
	if (m_pselected == NULL)
		return;
	pipe->ClearLog();
	pipe->SetAction(COMMANDS::Cemul);
	pipe->Emulate();
}

void cMList::OnSelected(wxListEvent& event)
{
	fLData* linfo = new fLData(this, event.GetItem().m_itemId);
	linfo->Show();
}

cMain* cMain::GetInstance()
{
	if (cmain == nullptr) {
		cmain = new cMain;
	}
	return cmain;
}

cMain* cMain::cmain = nullptr;