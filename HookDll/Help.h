#pragma once
#ifndef _HELP_H_
#define _HELP_H_

#include <future>
#include "fPipe.h"

extern std::future<void> async_thread_1;

extern fPipe *pipe;
#define SINGLE_MSG_SIZE 1024

//struct sMessage {
//	const char start_seq[2] = { '2','2' };
//	int int_size;
//	char packORD[4] = { 48,48,48,48 }, CMD[3], size[3];
//	char char_string[1012];
//};
//
//std::vector<char> PIHAY_V_VECTOR(char* str, int size);
//bool parse_messge(STRING rcv, sMessage* result);
//STRING make_msg(char snd[1012], int sz, const char command[3] = "SND");
//bool equal_strings(char* st, char* nd, int size);
#endif 