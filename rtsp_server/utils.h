#pragma once
#include <thread>
#include <string>
#include <signal.h>
static volatile sig_atomic_t stop = false;

static void XSleep(int ms)
{
	std::chrono::milliseconds du(ms);
	std::this_thread::sleep_for(du);
}
