#pragma once
#ifndef _HELP_H_
#define _HELP_H_

#include <future>
#include "fPipe.h"

extern std::future<void> async_thread_1;

extern fPipe *pipe;
#define SINGLE_MSG_SIZE 1024

#endif 