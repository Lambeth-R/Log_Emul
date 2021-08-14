#pragma once
#include <string>
#include <list>
#include <map>
#include <Windows.h>
#include <future>

#include "../common/common.h"

#define SINGLE_MSG_SIZE 1024

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
	static T* GetInstance(const Key& key, DWORD type)
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


// TODO  constructor - name + type + get/send
class Pipe : public Multiton<std::wstring, Pipe>
{
public:
	Pipe(
		std::wstring name,
		DWORD type			// Listen / Emulate
	);
	~Pipe();
	std::list<msg> GetLogMessages();
	void PutLogMessages(std::list<msg> mList);
	std::list<msg> GetMessages();
	void PutMessages(std::list<msg> mList);
	void AddSingleMessage(std::string message);
	void ClearLog();
private:
	True_WriteFile lpWriteFile;
	True_ReadFile lpReadFile;
	void WorkThread();
	bool PushToPipe(std::string mToSend);
	bool Log_parse(char* buffer, int readed, int* size);
	bool Cmd_parse(char* buffer, int readed);
	void Create(std::wstring pName);
	void Connect(std::wstring pName);
	void Send();	
	void Recieve();
	HANDLE hPipe = nullptr;
	int exit_code;
	std::mutex pipeMutex, dataMutex, logMutex;
	std::wstring name;
	DWORD type;
	DWORD error;
	// Connect log 
	std::list<msg>* LogMessages;
	// Listined data
	std::list<msg>* Messages;
	std::future<void>* hThread = nullptr;
};

