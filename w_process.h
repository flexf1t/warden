#pragma once

#include <sys/types.h>
#include <signal.h>

struct WSignals 
{
	enum sig
	{
		//W_STOP_SIGNAL = SIGTERM,
		W_QUIT_SIGNAL = SIGQUIT,
		W_RESTART_SIGNAL = SIGUSR1,
		W_RELOAD_SIGNAL = SIGUSR2
	};
};
//#define W_STOP "stop"
#define W_QUIT "quit"
#define W_RESTART "restart"
#define W_RELOAD "reload"
#define W_PID_FILE "warden.pid"

class Warden;
pid_t getWorkingPid();
bool isRunningPid(pid_t pid);
bool writePid();
bool sendSignal(pid_t pid, int signal);
typedef void (*signalHandlerFunc)(int);
bool setSignalHandler(int signal, signalHandlerFunc handler);
int getSignalFromCommandName(char *name);