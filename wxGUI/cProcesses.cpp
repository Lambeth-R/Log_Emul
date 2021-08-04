#include "cProcesses.h"
#include <psapi.h>
#include <corecrt_wstring.h>

cProcesses::cProcesses()
{
    this->Init();
}

void cProcesses::Init() 
{
    DWORD aProcesses[1024], cbNeeded;
    EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded);
    ProcCount = cbNeeded / sizeof(DWORD);
    for (int i = 0; i < ProcCount; i++)
    {
        if (aProcesses[i] != -1)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
            TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
            TCHAR lpFilename[MAX_PATH] = TEXT("<unknown>");
            if (hProcess != NULL)
            {
                HMODULE hMod;
                DWORD cbNeeded;
                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
                {
                    GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
                    GetModuleFileNameEx(hProcess, NULL, lpFilename, sizeof(lpFilename) / sizeof(TCHAR));
                }
            }
            process.push_back({ aProcesses[i], szProcessName,  lpFilename });
            if (hProcess) CloseHandle(hProcess);
        }
    }
}

void cProcesses::Clear() {
    while (!process.empty()) {
        process.pop_back();
    }
}

void cProcesses::Refresh() {
    this->Clear();
    this->Init();
}

cProcesses::~cProcesses()
{
    this->Clear();
}

cProcesses* cProcesses::GetInstance()
{
    if (cprocesses == nullptr) {
        cprocesses = new cProcesses;
    }
    return cprocesses;
}

cProcesses* cProcesses::cprocesses = nullptr;