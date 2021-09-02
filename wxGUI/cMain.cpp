#include "contsts.h"
#include "cMain.h"
#include "cProcesses.h"
#include "fProcess.h"
#include "fLData.h"

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
    EVT_BUTTON(FOPENID, cMain::SelectProcess)
	EVT_BUTTON(INJECTID, cMain::InjectWarp)
	EVT_BUTTON(LISTEN, cMain::ListenMode)
	EVT_BUTTON(EMULATE, cMain::EmulateMode)
	EVT_BUTTON(LOAD, cMain::OnLoad)
	EVT_BUTTON(SAVE, cMain::OnSave)
	EVT_BUTTON(EXIT, cMain::OnExit)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(cMList, wxListCtrl)
	EVT_LIST_ITEM_SELECTED(MSG_LIST, cMList::OnSelected)
wxEND_EVENT_TABLE()

void cMain::SelectProcess(wxCommandEvent& evt)
{
	cProcesses* c = cProcesses::GetInstance();
	c->Refresh();
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
	wxMenu* file_menu = new wxMenu();
	file_menu->Append(LOAD, _("&Load"));
	file_menu->Append(SAVE, _("&Save"));
	file_menu->AppendSeparator();
	file_menu->Append(EXIT, "&Exit");
	wxMenuBar* menu_bar = new wxMenuBar();
	menu_bar->Append(file_menu, _("&File"));
	SetMenuBar(menu_bar);
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
	Connect(LOAD, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(cMain::OnLoad));
	Connect(SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(cMain::OnSave));
	Connect(EXIT, wxEVT_COMMAND_MENU_SELECTED,	wxCommandEventHandler(cMain::OnExit));
	Centre();
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

void cMain::wait(Pipe* Pipe, int size, int LogMsg = 0)
{
	if (LogMsg == 0) {
		while (!(Pipe->GetLogMessages().size() > size))
		{
			std::unique_lock<std::mutex> ul(*Pipe->muxExtern);
			Pipe->cvExtern->wait(ul);
		}
		return;
	}
	if (LogMsg == 1) {
		while (!(Pipe->GetMessages().size() > size))
		{
			std::unique_lock<std::mutex> ul(*Pipe->muxExtern);
			Pipe->cvExtern->wait(ul);
		}
		return;
	}
}

void cMain::MsgSync(int idPipe)
{
	// 0 = cmdLogs, 1 = logLogs, 2 = emlLogs, 3 = logMsg
	if (idPipe > 3)
		return;
	if (idPipe < 3) {
		Pipe* pipe = Pipe::GetInstance(pipename[idPipe]);
		std::list<msg> messages;
		while (sync_exit_code < 0) {
			wait(pipe, messages.size());
			auto temp = pipe->GetLogMessages();
			auto it = temp.begin();
			std::advance(it, messages.size());
			while (it != temp.end()) {
				messages.push_back(*it);
				it++;
			}
			for (auto i = messages.begin(); i != messages.end(); i++)
			{
				if (!i._Ptr->_Myval.displayed) {
					m_log_msgs->Insert(i._Ptr->_Myval.message, i._Ptr->_Myval.order);
					i._Ptr->_Myval.displayed = true;
					m_log_msgs->Refresh();
				}
			}
		}
	}
	if (idPipe == 3) {
		idPipe = 1;
		Pipe* pipe = Pipe::GetInstance(pipename[idPipe]);
		if (LoadSaveData == nullptr)
			LoadSaveData = new std::list<msg>;
		while (sync_exit_code < 0) {
			wait(pipe, LoadSaveData->size(),1);
			auto temp = pipe->GetMessages();
			auto it = temp.begin();
			std::advance(it, LoadSaveData->size());
			while (it != temp.end()) {
				LoadSaveData->push_back(*it);
				it++;
			}
			for (auto i = LoadSaveData->begin(); i != LoadSaveData->end(); i++)
			{
				if (!i._Ptr->_Myval.displayed) {
					m_recieved_msgs->InsertItem(i._Ptr->_Myval.order, i._Ptr->_Myval.message);
					i._Ptr->_Myval.displayed = true;
					m_recieved_msgs->Refresh();
				}
			}
		}
	}
}

Process cMain::GetProcess()
{
	return *m_pselected;
}

void cMain::InjectWarp(wxCommandEvent& evt)
{
	if (injected) {
		wxMessageBox("Process already injected", "Error", wxOK);
		return;
	}
	Inject();
	if (m_msg_sync1 == nullptr) {
		Pipe* cPipe = Pipe::GetInstance(pipename[0], PIPE_CONNECT | PIPE_SEND);
		cPipe->SetRightFuncs(&WriteFile, &ReadFile);
		m_msg_sync1 = new std::future<void>(std::async(std::launch::async, &cMain::MsgSync, this, 0));
		Sleep(10);
		m_recieved_msgs->ClearAll();
		m_recieved_msgs->Refresh();
		LoadSaveData->clear();
	}
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
	if (m_pselected == NULL)
		return;
	Pipe* cPipe = Pipe::GetInstance(pipename[0]);
	Pipe* lPipe = Pipe::GetInstance(pipename[1], PIPE_CONNECT | PIPE_RECIEVE);
	lPipe->SetRightFuncs(&WriteFile, &ReadFile);
	if (m_msg_sync2 == nullptr)
		m_msg_sync2 = new std::future<void>(std::async(std::launch::async, &cMain::MsgSync, this, 1));
	Sleep(10);
	if (m_msg_sync4 == nullptr)
		m_msg_sync4 = new std::future<void>(std::async(std::launch::async, &cMain::MsgSync, this, 3));
	Sleep(10);
	cPipe->PutSingleMessage("log");
}

void cMain::EmulateMode(wxCommandEvent& evt)
{
	if (!injected)
		return;
	if (m_pselected == NULL)
		return;
	if (LoadSaveData == nullptr)
		return;
	Pipe* cPipe = Pipe::GetInstance(pipename[0]);
	Pipe* ePipe = Pipe::GetInstance(pipename[2], PIPE_CONNECT | PIPE_SEND);
	ePipe->SetRightFuncs(&WriteFile, &ReadFile);
	if (m_msg_sync3 == nullptr)
		m_msg_sync3 = new std::future<void>(std::async(std::launch::async, &cMain::MsgSync, this, 2));
	Sleep(10);
	cPipe->PutSingleMessage("eml");
	ePipe->PutMessages(*LoadSaveData);
}

std::list<msg>* cMain::GetData() {
	return LoadSaveData;
}

void cMList::OnSelected(wxListEvent& event)
{
	fLData* linfo = new fLData(this, event.GetItem().m_itemId);
	linfo->Show();
}

void cMain::OnSave(wxCommandEvent& event)
{
	Pipe* lPipe = Pipe::GetInstance(pipename[1], PIPE_CONNECT | PIPE_RECIEVE);
	//LoadSaveData = new std::list<msg>(lPipe->GetMessages());
	std::string* fname = new std::string;
	*fname = "";
	if (CreateLogFile(*LoadSaveData, fname)) {
		wxString msg;
		msg.append("Log file \"");
		msg.append(*fname);
		msg.append("\"  was successfully created.");
		wxMessageBox(msg, "Result", wxOK);
	}
}

void cMain::OnLoad(wxCommandEvent& event)
{
	wxFileDialog openFileDialog(this, _("Open XYZ file"), "", "", "DAT files (*.DAT)|*.dat", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changes his mind...
	LoadSaveData = ReadFromLog(openFileDialog.GetPath().c_str().AsChar());
	for (auto i : *LoadSaveData)
	{
		if (!i.displayed) {
			m_recieved_msgs->InsertItem(i.order, i.message);
			i.displayed = true;
			m_recieved_msgs->Refresh();
		}
	}
}

void cMain::OnExit(wxCommandEvent& event)
{
	sync_exit_code = 1;
	Close(true);
}

cMain* cMain::GetInstance()
{
	if (cmain == nullptr) {
		cmain = new cMain;
	}
	return cmain;
}

cMain* cMain::cmain = nullptr;