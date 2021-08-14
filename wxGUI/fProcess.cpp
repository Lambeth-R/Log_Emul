#include "fProcess.h"
#include "contsts.h"
#include "cMain.h"

wxBEGIN_EVENT_TABLE(MyListCtrl, wxListCtrl)
	EVT_LIST_COL_CLICK(LIST_CTRL, MyListCtrl::OnColClick)
	EVT_LIST_ITEM_FOCUSED(LIST_CTRL, MyListCtrl::OnFocused)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(fProcess, wxFrame)
	EVT_BUTTON(PSELECTOR, fProcess::OnButtonClick)
	EVT_TEXT(PSEARCH, fProcess::OnSearch)
wxEND_EVENT_TABLE()

fProcess::fProcess(wxWindow* Parent) : wxFrame(Parent, wxID_ANY, "Select Process", { (Parent->GetPosition().x  + mwind_size.x / 4), (Parent->GetPosition().y - 50) }, pwind_size)
{
	m_list_ctrl = new MyListCtrl(this);
	m_Sel_btn = new wxButton(this, PSELECTOR, "Select");
	m_sizer = new wxBoxSizer(wxVERTICAL);
	m_tsearch = new wxTextCtrl(this, PSEARCH);
	m_sizer->Add(m_list_ctrl, wxSizerFlags(1).Expand().Border());
	m_sizer->Add(m_tsearch, wxSizerFlags(2).Expand().Border());
	m_sizer->Add(m_Sel_btn, wxSizerFlags(3).Expand().Border());
	this->SetSizer(m_sizer);
}

void fProcess::OnSearch(wxCommandEvent& evt)
{
	wxString str = evt.GetString();
	m_list_ctrl->OnSearch(str);
}

void fProcess::OnButtonClick(wxCommandEvent& evt)
{
	cMain* myParent = reinterpret_cast<cMain*>(this->GetParent());
	myParent->SetSelected(m_list_ctrl->GetSelected());
	this->Close();
}

fProcess::~fProcess()
{
	delete(m_list_ctrl);
	delete(m_Sel_btn);
	m_sizer->Clear();
}

Process* MyListCtrl::GetSelected()
{
	return &m_selected;
}

void MyListCtrl::OnSearch(wxString str)
{
	Hide();
	m_cP->Refresh();
	int deleted = 0;
	int base_size = m_cP->process.size();
	while (deleted != base_size) {
		auto temp = m_cP->process.front();
		m_cP->process.pop_front();
		if (temp.name.Find(str) != -1)
			m_cP->process.push_back(temp);
		deleted++;
	}
	Sort_by(-1);
	Show();
}

void MyListCtrl::Sort_by(int col)
{
	DeleteAllItems();
	*m_item_count = -1;
	if (col == 0 ) {
		if (!m_sorted->_st) {
			m_cP->process.sort([](const Process& a, const Process& b) {
				return a.pid < b.pid; });
			m_sorted->_st = true;
		}
		else {
			m_cP->process.sort([](const Process& a, const Process& b) {
				return a.pid > b.pid; });
			m_sorted->_st = false;
		}
	}
	else if (col == 1) {
		if (!m_sorted->_nd) {
			m_cP->process.sort([](const Process& a, const Process& b) {
				return a.name < b.name; });
			m_sorted->_nd = true;
		}
		else {
			m_cP->process.sort([](const Process& a, const Process& b) {
				return a.name > b.name; });
			m_sorted->_nd = false;
		}
	}

	for (auto c : m_cP->process) {
		(*m_item_count)++;
		wxString* buf = new wxString();
		buf->Printf("%d", c.pid);
		long tmp = InsertItem(*m_item_count, *buf, 0);
		SetItemData(tmp, *m_item_count);
		SetItem(tmp, 1, c.name);
		delete(buf);
	}
}

void MyListCtrl::OnColClick(wxListEvent& event)
{
	int col = event.GetColumn();
	if (col == -1)
	{
		return; 
	}
	Hide();
	Sort_by(col);
	Show();
}

void MyListCtrl::OnFocused(wxListEvent& event)
{
	auto t = event.GetData();
	auto pIter = m_cP->process.begin();
	std::advance(pIter, t);
	m_selected = { pIter->pid, pIter->name, pIter->path};
	return;
}

MyListCtrl::MyListCtrl(wxWindow* Parent) : wxListCtrl(Parent, LIST_CTRL, { 5,2 }, { pwind_size.x - 25 , (int)7 * (pwind_size.y / 9) }, wxLC_REPORT | wxBORDER_THEME | wxLC_EDIT_LABELS)
{
	Hide();
	m_sorted = new Sorted();
	InsertColumn(0, "Pid");
	InsertColumn(1, "Name");
	this->m_cP = cProcesses::GetInstance();
	SetColumnWidth(0, 50);
	SetColumnWidth(1, 200);
	m_item_count = new int();
	*m_item_count = -1;
	for (auto c : m_cP->process) {
		(*m_item_count)++;
		if (c.name.IsSameAs("<unknown>")) continue;
		wxString buf;
		buf.Printf("%d", c.pid);
		long tmp = InsertItem(*m_item_count, buf, 0);
		SetItemData(tmp, *m_item_count);
		SetItem(tmp, 1, c.name);
	}
	Show();
}

MyListCtrl::~MyListCtrl() 
{
	delete (m_sorted);
	delete (m_item_count);
}