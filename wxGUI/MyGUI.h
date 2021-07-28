#pragma once
#ifndef _MY_GUI_H_
#define _MY_GUI_H_

#include "wx\wx.h"
#include "cMain.h"

class MyGUI : public wxApp
{
public:
	MyGUI();
	~MyGUI();
	bool OnInit();

private:
	cMain* m_frame1 = nullptr;
};

#endif