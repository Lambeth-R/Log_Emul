#include "cProcesses.h"
#include <psapi.h>
#include <corecrt_wstring.h>

cProcesses::cProcesses()
{
    this->Init_list();
}

void cProcesses::Init_list() 
{
    DWORD aProcesses[1024], cbNeeded;
    EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded);
    cProc = cbNeeded / sizeof(DWORD);
    for (int i = 0; i < cProc; i++)
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
void cProcesses::del_list() {
    while (!process.empty()) {
        process.pop_back();
    }
}
void cProcesses::refresh_list() {
    this->del_list();
    this->Init_list();
}

cProcesses::~cProcesses()
{
    this->del_list();
}