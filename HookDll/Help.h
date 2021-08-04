#pragma once
#ifndef _HELP_H_
#define _HELP_H_

#include <future>
#include "dllcPipe.h"

extern std::future<void> async_thread_1;

extern dllcPipe *pipe;
#define SINGLE_MSG_SIZE 1024

#endif 