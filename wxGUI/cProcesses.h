#pragma once
#ifndef _C_PROCESSES_H_
#define _C_PROCESSES_H_


#include <windows.h>
#include <mutex>
#include "contsts.h"
#include <list>

struct Process {
	DWORD pid;
	wxString name;
	wxString path;
};

// Snapshot class (depends on privleges)
class cProcesses
{
public:
	// Creates current processes snapshot
	void Init();
	// Refresh snapshot
	void Refresh();
	// Snapshot of processes
	std::list<Process> process;
	// Singletone
	static cProcesses* GetInstance();
	cProcesses(cProcesses& other) = delete;
	void operator=(const cProcesses&) = delete;
private:
	// Clear data
	void Clear();
	// Number of processes
	DWORD ProcCount;
	// Mutex for refresh
	std::mutex lock;
protected:
	// Singletone
	static cProcesses* cprocesses;
	cProcesses();
	~cProcesses();
};

#endif
