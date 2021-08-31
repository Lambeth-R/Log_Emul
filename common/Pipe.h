#pragma once
#include <string>
#include <list>
#include <map>
#include <Windows.h>
#include <future>

#include "../common/common.h"

#define SINGLE_MSG_SIZE 1024
// Work Type enum
enum tPipe {
	PIPE_SEND = 1,
	PIPE_RECIEVE = 2,
	PIPE_CREATE = 4,
	PIPE_CONNECT = 8 
};
typedef BOOL (WINAPI *True_ReadFile)(
	HANDLE,
	LPVOID,
	DWORD,
	LPDWORD,
	LPOVERLAPPED);

typedef BOOL(WINAPI *True_WriteFile)(
	HANDLE,
	LPCVOID,
	DWORD,
	LPDWORD,
	LPOVERLAPPED
);
// Mulititon impl
template <typename Key, typename T> class Multiton
{
public:
	static void destroy()
	{
		for (typename std::map<Key, T*>::iterator it = instances.begin(); it != instances.end(); ++it) {
			delete (*it).second;
		}
	}
	static bool Remove(const Key& key)
	{
		typename std::map<Key, T*>::iterator it = instances.find(key);
		if (it != instances.end()) {
			delete (*it).second;
			return true;
		}
		return false;
	}
	static T* GetInstance(const Key& key, DWORD type = 0)
	{
		typename std::map<Key, T*>::iterator it = instances.find(key);

		if (it != instances.end()) {
			return (T*)(it->second);
		}

		T* instance = new T(key, type);
		instances[key] = instance;
		return instance;
	}

protected:
	Multiton() {}
	virtual ~Multiton() {}

private:
	Multiton(const Multiton&) {}
	Multiton& operator= (const Multiton&) { return *this; }

	static std::map<Key, T*> instances;
};

template <typename Key, typename T> std::map<Key, T*> Multiton<Key, T>::instances;

class Pipe : public Multiton<std::wstring, Pipe>
{
public:
	Pipe(
		std::wstring name,
		DWORD type			// Listen / Emulate
	);
	~Pipe();
	// Funcs to access messages
	std::list<msg> GetLogMessages();
	void PutLogMessages(std::list<msg> mList);
	void PutSingleLogMessage(std::string sMsg);
	void PutMessages(std::list<msg> mList);
	void PutSingleMessage(std::string sMsg);
	std::list<msg> GetMessages();
	void ClearLog();
	void SetRightFuncs(True_WriteFile TrueWriteFile, True_ReadFile TrueReadFile);
	std::condition_variable* cvExtern;
	std::mutex* muxExtern;
private:
	// Acuall WriteFile, ReadFile, for correct pipe work inside dll
	True_WriteFile lpWriteFile;
	True_ReadFile lpReadFile;
	bool correct_init = false;
	// Pipe Send / Reiceve func
	void WorkThread();
	// Place str to pipe
	bool PushToPipe(std::string mToSend);
	// Parce message data;
	bool parsable(char* buffer, int readed, int* size);
	void wait(int size);

	// Pipe init/work funcs
	void Create(std::wstring pName);
	void Connect(std::wstring pName);
	void Send();	
	void Recieve();
	
	// Some variables using across class
	HANDLE hPipe = nullptr;
	int exit_code;
	std::mutex muxPipe, muxData, muxLog, muxWait, muxInit;
	std::condition_variable cvBlock, cvInit;
	std::wstring name;
	DWORD type;
	DWORD error;
	std::string keyword;

	// Connect log 
	std::list<msg>* LogMessages;
	// Listined data
	std::list<msg>* Messages;
	// Pipe Send / Reiceve thread
	std::future<void>* hThread = nullptr;
};

