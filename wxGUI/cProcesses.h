#pragma once
#ifndef _C_PROCESSES_H_
#define _C_PROCESSES_H_


#include <windows.h>
#include "contsts.h"
#include <list>

typedef struct P {
	DWORD pid;
	wxString name;
	wxString path;
}Process;


class cProcesses
{
public:
	cProcesses();
	~cProcesses();
	void Init_list();
	void refresh_list();

private:
	void del_list();
	DWORD cProc;

public:
	std::list<Process> process;
};

#endif
