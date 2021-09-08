#include "MyGUI.h"


wxIMPLEMENT_APP(MyGUI);

MyGUI::MyGUI() 
{
	setlocale(LC_ALL, "Russian");
}

MyGUI::~MyGUI()
{

}

bool MyGUI::OnInit()
{
	m_frame1 = cMain::GetInstance();
	m_frame1->Show();
	return true;
}
