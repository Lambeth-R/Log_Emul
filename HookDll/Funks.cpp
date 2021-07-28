#define _CRT_SECURE_NO_WARNINGS	
#include "Funks.h"
#include "Help.h"
#include <string>

HANDLE pipe_command, pipe_log;
DWORD packID;

//HANDLE LMBT_CreateFileW(
//	LPCWSTR               lpFileName,
//	DWORD                 dwDesiredAccess,
//	DWORD                 dwShareMode,
//	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
//	DWORD                 dwCreationDisposition,
//	DWORD                 dwFlagsAndAttributes,
//	HANDLE                hTemplateFile
//) {
//	pipe_log = CreateNamedPipe(TEXT("\\\\.\\pipe\\lmbtpipe_log"),
//		PIPE_ACCESS_DUPLEX,
//		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
//		1,
//		1024,
//		1024,
//		NMPWAIT_USE_DEFAULT_WAIT,
//		NULL);
//	HANDLE retValue = NULL;
//	retValue = True_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
//	DWORD dwWritten, dwRead;
//	std::string t = "Listen_";
//	t += std::to_string(*lpNumberOfBytesRead);
//	STRING init_msg = make_msg((char*)t.c_str(), t.size(), "ini");
//	WriteFile(pipe_log,
//		init_msg.data(),
//		1024,   // = length of string + terminating '\0' !!!
//		&dwRead,
//		NULL);
//	char msg[256];
//	std::string out;
//	out += "CreateFile(";
//	sprintf(msg, "%p", hFile); //Handle
//	out += msg;
//	out += "|||";
//	for (int i = 0; i < *lpNumberOfBytesRead; i++) //Buff
//	{
//		out += ((char*)lpBuffer)[i];
//	}
//	out += "|||";
//	out += std::to_string(nNumberOfBytesToRead); //To read
//	out += "|||";
//	out += std::to_string(*lpNumberOfBytesRead);	//Readed
//	out += "|||";
//	out += "NULL";	//Null???
//	out += "|||";
//	//Ѕить на пакеты!
//	for (int i = 0; i < out.size(); i += 1012) {
//		int sz_to_send = 1012;
//		if (i + 1024 > out.size())
//			sz_to_send = out.size() - i;
//		STRING packet = make_msg((char*)out.substr(i, i + 1012).c_str(), sz_to_send);
//		WriteFile(pipe_log,
//			packet.data(),
//			1024,
//			&dwWritten,
//			NULL);
//	}
//	//STRING init = make_msg((char*)"STOP", 5, "SND");
//	//WriteFile(pipe_command,
//	//	init.data(),
//	//	1024,   // = length of string + terminating '\0' !!!
//	//	&dwRead,
//	//	NULL);
//	DisconnectNamedPipe(pipe_log);
//	return retValue;
//}

__declspec(dllexport) BOOL WINAPI LMBT_ReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped) {
	pipe_log = CreateNamedPipe(TEXT("\\\\.\\pipe\\lmbtpipe_log"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		2,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	bool retValue = 0;
	if (ConnectNamedPipe(pipe_log, NULL))   // wait for someone to connect to the pipe
	{		
		retValue = True_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
		DWORD dwWritten, dwRead;
		Sleep(1000);
		std::string t = "Listen_";
		t += std::to_string(*lpNumberOfBytesRead);
		STRING init_msg = make_msg((char*)t.c_str(), t.size(), "ini");
		WriteFile(pipe_log,
			init_msg.data(),
			1024,   // = length of string + terminating '\0' !!!
			&dwRead,
			NULL);
		char msg[256];
		std::string out;
		out += "ReadFile(";
		sprintf(msg, "%p", hFile); //Handle
		out += msg;
		out += "|||";
		for (int i = 0; i < *lpNumberOfBytesRead; i++) //Buff
		{
			out += ((char*)lpBuffer)[i];
		}
		out += "|||";
		out += std::to_string(nNumberOfBytesToRead); //To read
		out += "|||";
		out += std::to_string(*lpNumberOfBytesRead);	//Readed
		out += "|||";
		out += "NULL";	//Null???
		out += "|||";
		//Ѕить на пакеты!
		for (int i = 0; i < out.size(); i += 1012) {
			int sz_to_send = 1012;
			if (i + 1024 > out.size())
				sz_to_send = out.size() - i;
			STRING packet = make_msg((char*)out.substr(i, i + 1012).c_str(), sz_to_send);
			WriteFile(pipe_log,
				packet.data(),
				1024,
				&dwWritten,
				NULL);
		}
	}
	DisconnectNamedPipe(pipe_log);
	return retValue;
}

__declspec(dllexport) BOOL WINAPI emul_LMBT_ReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped) {
	pipe_log = CreateNamedPipe(TEXT("\\\\.\\pipe\\lmbtpipe_log"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		2,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	if (ConnectNamedPipe(pipe_log, NULL))   // wait for someone to connect to the pipe
	{
		std::vector<STRING> backpack;
		char buffer[1024];
		DWORD dwRead;
		while (True_ReadFile(pipe_log, buffer, 1024, &dwRead, NULL) != FALSE)
		{
			STRING B = PIHAY_V_VECTOR(buffer, 1024);
			sMessage readed_message;
			parse_messge(B, &readed_message);
			B = PIHAY_V_VECTOR((char*)readed_message.char_string, readed_message.int_size);
			backpack.push_back(B);
		}
		DWORD prev_size = 0;
		bool start = false;
		for (int i = 0; i < backpack[0].size(); i++) {
			if (backpack[0].data()[i] == '_') {
				start = true;
				continue;
			}
			if (!start) {
				continue;
			}
			prev_size *= 10;
			prev_size = backpack[0].data()[i];
			*lpNumberOfBytesRead = prev_size;
		}
		lpBuffer = new char[prev_size];
		DWORD b_start = 0;
		while (backpack[1].data()[b_start] != '|' && backpack[1].data()[b_start + 1] != '|' && backpack[1].data()[b_start + 2] != '|')
			b_start++;
		b_start += 3;
		memcpy(lpBuffer, &(backpack[1].data()[b_start]), backpack[1].size() - b_start);
		b_start = backpack[1].size() - b_start;
		for (int i = 2; i < backpack.size(); i++) {
			std::string str(backpack[i].data());
			DWORD end = str.find("|||");
			if (end == std::string::npos) {
				memcpy(&((char*)lpBuffer)[b_start], backpack[i].data(), end);
			}
			else {
				memcpy(&((char*)lpBuffer)[b_start], backpack[i].data(), backpack[i].size());
				b_start += backpack[i].size();
			}
		}
	}
	DisconnectNamedPipe(pipe_log);
	return 0;
}